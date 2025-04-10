#include "..\node\node.cpp"
#include <string>

void clearInputBuffer() {
    cin.clear();
    while (cin.get() != '\n');
}

class Tree {
private:
    Node* root;
    Node* currentChief;
    int nextId;
    string filename;
    string contributorsFile;
   
    Node* findNodeById(Node* node, int id) {
        if (node == nullptr) return nullptr;
        if (node->id == id) return node;
       
        Node* found = findNodeById(node->left, id);
        if (found) return found;
        return findNodeById(node->right, id);
    }
   
    bool insertNode(Node* parent, Node* newNode) {
        if (parent == nullptr) {
            return false;
        }
       
        if (parent->countChildren() >= 2) {
            cout << "Error: El padre ya tiene 2 hijos\n";
            return false;
        }
       
        if (parent->left == nullptr) {
            parent->left = newNode;
        } else {
            Node* sibling = parent->left;
            while (sibling->right != nullptr) {
                sibling = sibling->right;
            }
            sibling->right = newNode;
        }
        return true;
    }

    void printTree(Node* node, int level = 0) {
        if (!node) return;
       
        for (int i = 0; i < level; ++i) cout << "  ";
        cout << node->name << " " << node->last_name << " (ID:" << node->id << ")";
        if (node->is_chief) cout << " [JEFE]";
        if (node->is_dead) cout << " [DIFUNTO]";
        cout << "\n";
       
        printTree(node->left, level + 1);
        printTree(node->right, level);
    }

public:
    Tree() : root(nullptr), currentChief(nullptr), nextId(1), filename("clan_data.csv"), contributorsFile("contributors.csv") {}
    ~Tree() { delete root; }
   
    void displayCurrentState() {
        cout << "\n=== ESTADO ACTUAL ===\n";
        if (currentChief) {
            cout << "JEFE ACTUAL:\n";
            currentChief->print();
        } else {
            cout << "Sin jefe actual\n";
        }
       
        cout << "\nÁRBOL COMPLETO:\n";
        printTree(root);
        cout << endl;
    }

    void saveToCSVHelper(Node* node, ofstream& file) {
        if (node == nullptr) return;
       
        file << node->toCSV() << endl;
       
        saveToCSVHelper(node->left, file);
        saveToCSVHelper(node->right, file);
    }
   
    void updateCSV() {
        ofstream file(filename);
        if (!file.is_open()) {
            cerr << "Error: No se pudo abrir " << filename << " para escritura\n";
            return;
        }
       
        file << "id;name;last_name;gender;age;id_father;is_dead;was_chief;is_chief" << endl;
        saveToCSVHelper(root, file);
        file.close();
    }

    bool loadFromCSV() {
        ifstream file(filename);
        if (!file.is_open()) {
            cerr << "Error: No se pudo abrir " << filename << endl;
            return false;
        }
       
        string line;
        getline(file, line); // Read header
       
        while (getline(file, line)) {
            if (!line.empty() && line.back() == '\r') {
                line.pop_back();
            }
           
            string tokens[9];
            int tokenCount = 0;
            stringstream ss(line);
            string token;
           
            while (getline(ss, token, ';') && tokenCount < 9) {
                token.erase(0, token.find_first_not_of(" \t"));
                token.erase(token.find_last_not_of(" \t") + 1);
                tokens[tokenCount++] = token;
            }
           
            if (tokenCount != 9) {
                cerr << "Error: Línea con formato incorrecto - " << line << endl;
                continue;
            }
           
            try {
                int id = stoi(tokens[0]);
                string name = tokens[1];
                string last_name = tokens[2];
                char gender = toupper(tokens[3][0]);
                int age = stoi(tokens[4]);
                int id_father = tokens[5].empty() ? -1 : stoi(tokens[5]);
                bool is_dead = (tokens[6] == "1" || toupper(tokens[6][0]) == 'S');
                bool was_chief = (tokens[7] == "1" || toupper(tokens[7][0]) == 'S');
                bool is_chief = (tokens[8] == "1" || toupper(tokens[8][0]) == 'S');
               
                if (gender != 'F' && gender != 'M') {
                    throw runtime_error("Género inválido. Debe ser F o M");
                }
               
                Node* newNode = new Node(id, name, last_name, gender, age, id_father, is_dead, was_chief, is_chief);
               
                if (id >= nextId) nextId = id + 1;
               
                if (id_father == -1) {
                    if (root) {
                        cerr << "Advertencia: Múltiples raíces (ID " << id << "), ignorando registro\n";
                        delete newNode;
                    } else {
                        root = newNode;
                    }
                } else {
                    Node* parent = findNodeById(root, id_father);
                    if (parent) {
                        if (!insertNode(parent, newNode)) {
                            cerr << "Advertencia: No se pudo insertar hijo para padre ID " << id_father << endl;
                            delete newNode;
                        }
                    } else {
                        cerr << "Advertencia: Padre no encontrado para ID " << id << ", ignorando registro\n";
                        delete newNode;
                    }
                }
            } catch (const exception& e) {
                cerr << "Error procesando línea '" << line << "': " << e.what() << endl;
            }
        }
       
        file.close();
        return root != nullptr;
    }
};