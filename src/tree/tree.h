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

    void addMember() {
        if (root == nullptr) {
            cout << "Error: No se puede agregar miembros sin un árbol cargado\n";
            return;
        }
       
        string name, last_name;
        char gender;
        int age, id_father;
        bool is_dead, was_chief;
       
        cout << "Nombre: ";
        getline(cin, name);
        cout << "Apellido: ";
        getline(cin, last_name);
        cout << "Género (F/M): ";
        cin >> gender;
        gender = toupper(gender);
        if (gender != 'F' && gender != 'M') {
            cout << "Error: Género debe ser F o M\n";
            clearInputBuffer();
            return;
        }
        cout << "Edad: ";
        cin >> age;
        cout << "ID del padre (debe existir en el árbol): ";
        cin >> id_father;
        cout << "¿Está muerto? (1=Sí/0=No): ";
        cin >> is_dead;
        cout << "¿Fue jefe? (1=Sí/0=No): ";
        cin >> was_chief;
        clearInputBuffer();
       
        if (id_father == -1) {
            cout << "Error: No se puede agregar otro root al árbol\n";
            return;
        }
       
        Node* parent = findNodeById(root, id_father);
        if (!parent) {
            cout << "Error: Padre no encontrado en el árbol\n";
            return;
        }
       
        Node* newNode = new Node(nextId++, name, last_name, gender, age, id_father, is_dead, was_chief, false);
       
        if (!insertNode(parent, newNode)) {
            delete newNode;
            return;
        }
       
        cout << "Miembro agregado con ID " << newNode->id << endl;
        updateCSV();
    }

    void showLivingMembers() {
        cout << "\n=== MIEMBROS FALLECIDOS ===\n";
        printLiving(root, false);
    }
   
    void showDeceasedMembers() {
        cout << "\n=== MIEMBROS VIVOS ===\n";
        printLiving(root, true);
    }

    void printLiving(Node* node, bool living) {
        if (node == nullptr) return;
        if (node->is_dead != living) {
            node->print();
        }
        printLiving(node->left, living);
        printLiving(node->right, living);
    }

    void updateChiefAutomatically() {
        if (!currentChief) {
            cout << "No hay jefe actual\n";
            return;
        }
       
        if (currentChief->is_dead || currentChief->age > 70) {
            cout << "El jefe actual " << (currentChief->is_dead ? "ha fallecido" : "tiene más de 70 años (" + to_string(currentChief->age) + ")") << ". Buscando sucesor...\n";
           
            Node* newChief = findNewChief(currentChief);
           
            if (newChief) {
                currentChief->is_chief = false;
                currentChief->was_chief = true;
                newChief->is_chief = true;
                newChief->was_chief = false;
               
                cout << "\n=== Nuevo jefe ===\n";
                newChief->print();
                currentChief = newChief;
                updateCSV();
            } else {
                cout << "No se encontró sucesor adecuado\n";
            }
        } else {
            cout << "El jefe sigue siendo válido (edad: " << currentChief->age << ")\n";
        }
    }
   

    void updateMemberData() {
        int id;
        cout << "ID del miembro a actualizar: ";
        cin >> id;
        clearInputBuffer();
       
        Node* node = findNodeById(root, id);
        if (!node) {
            cout << "ID no encontrado\n";
            return;
        }
       
        string name, last_name;
        char gender;
        int age;
        bool is_dead, was_chief;
       
        cout << "Nuevo nombre (" << node->name << "): ";
        getline(cin, name);
        cout << "Nuevo apellido (" << node->last_name << "): ";
        getline(cin, last_name);
        cout << "Género (F/M) (" << node->gender << "): ";
        cin >> gender;
        gender = toupper(gender);
        if (gender != 'F' && gender != 'M') {
            cout << "Error: Género debe ser F o M\n";
            clearInputBuffer();
            return;
        }
        cout << "Edad (" << node->age << "): ";
        cin >> age;
        cout << "¿Está muerto? (1=Sí/0=No) (" << node->is_dead << "): ";
        cin >> is_dead;
        cout << "¿Fue jefe? (1=Sí/0=No) (" << node->was_chief << "): ";
        cin >> was_chief;
        clearInputBuffer();
       
        cout << "\n=== Actualizando ===\nANTES:\n";
        node->print();
       
        node->name = name.empty() ? node->name : name;
        node->last_name = last_name.empty() ? node->last_name : last_name;
        node->gender = gender;
        node->age = age;
        node->is_dead = is_dead;
        node->was_chief = was_chief;
       
        cout << "DESPUÉS:\n";
        node->print();
       
        if (node->is_chief && (is_dead || age > 70)) {
            updateChiefAutomatically();
        }
       
        updateCSV();
    }

    Node* findNewChief(Node* node) {
        // 1. Find first living son
        Node* candidate = findFirstLivingSon(node);
        if (candidate) return candidate;
       
        // 2. Find first grandson through first living daughter
        Node* firstDaughter = findFirstLivingDaughter(node);
        if (firstDaughter && (candidate = findFirstLivingSon(firstDaughter))) {
            return candidate;
        }
       
        // 3. Check brothers
        Node* brother = node->findBrother();
        if (brother) {
            if ((candidate = findFirstLivingSon(brother))) {
                return candidate;
            }
            if (!brother->hasChildren()) {
                return brother;
            }
        }
       
        // 4. Check uncles
        Node* uncle = node->findUncle(findNodeById(root, node->id_father));
        if (uncle) {
            if ((candidate = findFirstLivingSon(uncle))) {
                return candidate;
            }
            if (!uncle->hasChildren()) {
                return uncle;
            }
        }
       
        // 5. Find ancestor with two children
        Node* ancestor = node->findAncestorWithTwoChildren();
        if (ancestor && (candidate = findFirstLivingSon(ancestor))) {
            return candidate;
        }
       
        // 6. If no males, apply same rules for females
        if ((candidate = findFirstLivingDaughter(node))) return candidate;
        if (firstDaughter && (candidate = findFirstLivingDaughter(firstDaughter))) return candidate;
        if (brother && (candidate = findFirstLivingDaughter(brother))) return candidate;
        if (uncle && (candidate = findFirstLivingDaughter(uncle))) return candidate;
        if (ancestor && (candidate = findFirstLivingDaughter(ancestor))) return candidate;
       
        return nullptr;
    }

    Node* findFirstLivingSon(Node* node) {
        if (node == nullptr || !node->hasChildren()) return nullptr;
        Node* child = node->left;
        while (child != nullptr) {
            if (!child->is_dead && child->gender == 'M') return child;
            child = child->right;
        }
        return nullptr;
    }
   
    Node* findFirstLivingDaughter(Node* node) {
        if (node == nullptr || !node->hasChildren()) return nullptr;
        Node* child = node->left;
        while (child != nullptr) {
            if (!child->is_dead && child->gender == 'F') return child;
            child = child->right;
        }
        return nullptr;
    } 

    void markAsDeceased() {
        int id;
        cout << "ID del miembro fallecido: ";
        cin >> id;
        clearInputBuffer();
       
        Node* node = findNodeById(root, id);
        if (!node) {
            cout << "ID no encontrado\n";
            return;
        }
       
        if (node->is_dead) {
            cout << "Este miembro ya estaba marcado como fallecido\n";
            return;
        }
       
        node->is_dead = true;
        cout << "Se ha registrado el fallecimiento de " << node->name << " " << node->last_name << endl;
       
        if (node->is_chief) {
            cout << "Era el jefe actual. Buscando sucesor...\n";
            updateChiefAutomatically();
        }
       
        updateCSV();
    }
   
};