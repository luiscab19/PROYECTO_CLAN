#ifndef TREE_H
#define TREE_H

#include "..\node\node.cpp"
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>

using namespace std;

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
        if (parent == nullptr) return false;
       
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

    void findChiefHelper(Node* node, Node** chief) {
        if (node == nullptr || *chief != nullptr) return;
        if (node->is_chief) {
            *chief = node;
            return;
        }
        findChiefHelper(node->left, chief);
        findChiefHelper(node->right, chief);
    }

    Node* findFirstLivingChild(Node* node, char gender) {
        if (!node || !node->hasChildren()) return nullptr;
        
        Node* child = node->left;
        while (child != nullptr) {
            if (!child->is_dead && child->gender == gender) {
                return child;
            }
            child = child->right;
        }
        return nullptr;
    }

    Node* findFirstLivingMember(Node* node) {
        if (!node) return nullptr;
        if (!node->is_dead) return node;
        
        Node* found = findFirstLivingMember(node->left);
        if (found) return found;
        
        return findFirstLivingMember(node->right);
    }

    void printLiving(Node* node, bool living) {
        if (node == nullptr) return;
        if (node->is_dead != living) {
            node->print();
        }
        printLiving(node->left, living);
        printLiving(node->right, living);
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

    void saveContributorsHelper(Node* node, ofstream& file) {
        if (!node) return;
        
        for (int i = 0; i < node->contributorCount; i++) {
            file << node->id << ";" << node->contributors[i].name << ";" << node->contributors[i].age << ";"
                 << node->contributors[i].id << ";" << node->contributors[i].contribution << ";" 
                 << node->contributors[i].contribution_level << "\n";
        }
        
        saveContributorsHelper(node->left, file);
        saveContributorsHelper(node->right, file);
    }

    bool isNodeInPath(Node* node, Node* path[], int pathLength) {
        for (int i = 0; i < pathLength; i++) {
            if (path[i] == node) {
                return true;
            }
        }
        return false;
    }

public:
    Tree() : root(nullptr), currentChief(nullptr), nextId(1), 
             filename("../bin/clan_data.csv"), contributorsFile("../bin/Contributors.csv") {}
    ~Tree() { delete root; }
   
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
       
        // Cargar contribuidores
        ifstream contribFile(contributorsFile);
        if (contribFile.is_open()) {
            string line;
            getline(contribFile, line); // Header
            
            while (getline(contribFile, line)) {
                if (line.empty()) continue;
                
                stringstream ss(line);
                string token;
                string tokens[6];
                int tokenCount = 0;
                
                while (getline(ss, token, ';') && tokenCount < 6) {
                    tokens[tokenCount++] = token;
                }
                
                if (tokenCount < 6) continue;
                
                int memberId = stoi(tokens[0]);
                Node* member = findNodeById(root, memberId);
                if (!member) continue;
                
                string name = tokens[1];
                int age = stoi(tokens[2]);
                int id = stoi(tokens[3]);
                string contribution = tokens[4];
                int level = stoi(tokens[5]);
                
                member->addContributor(name, age, id, contribution, level);
            }
            
            contribFile.close();
        }
       
        currentChief = nullptr;
        findChiefHelper(root, &currentChief);
       
        if (!currentChief && root) {
            cerr << "Advertencia: No se encontró jefe en el árbol\n";
        }
       
        return root != nullptr;
    }
    
    void saveContributorsToCSV() {
        ofstream file(contributorsFile);
        if (!file.is_open()) {
            cerr << "Error: No se pudo abrir " << contributorsFile << " para escritura\n";
            return;
        }
        
        file << "member_id;name;age;id;contribution;level\n";
        saveContributorsHelper(root, file);
        file.close();
    }
   
    void showShortSuccessionLine() {
        if (!currentChief) {
            cout << "No hay jefe actual\n";
            return;
        }
       
        cout << "\n=== Línea de sucesión corta ===\n";
        cout << "Jefe actual:\n";
        currentChief->print();
       
        Node* heir1 = findNewChief(currentChief);
        if (heir1) {
            cout << "\nHeredero 1:\n";
            heir1->print();
           
            Node* heir2 = findNewChief(heir1);
            if (heir2) {
                cout << "\nHeredero 2:\n";
                heir2->print();
            }
        }
    }
   
    Node* findNewChief(Node* node) {
        if (!node) return nullptr;

        // 1. Buscar primer hijo varón vivo
        Node* child = node->left;
        while (child) {
            if (!child->is_dead && child->gender == 'M') {
                return child;
            }
            child = child->right;
        }

        // 2. Buscar nietos varones vivos a través de hijas
        child = node->left;
        while (child) {
            if (!child->is_dead && child->gender == 'F') {
                Node* grandson = child->left;
                while (grandson) {
                    if (!grandson->is_dead && grandson->gender == 'M') {
                        return grandson;
                    }
                    grandson = grandson->right;
                }
            }
            child = child->right;
        }

        // 3. Buscar hermanos varones vivos
        Node* father = findNodeById(root, node->id_father);
        if (father) {
            Node* uncle = father->left;
            while (uncle) {
                if (uncle != node && !uncle->is_dead && uncle->gender == 'M') {
                    // 3a. Si tiene hijos, buscar el primero varón vivo
                    Node* nephew = uncle->left;
                    while (nephew) {
                        if (!nephew->is_dead && nephew->gender == 'M') {
                            return nephew;
                        }
                        nephew = nephew->right;
                    }
                    // 3b. Si no tiene hijos, el hermano mismo
                    return uncle;
                }
                uncle = uncle->right;
            }
        }

        // 4. Buscar tíos varones vivos (hermanos del padre)
        if (father) {
            Node* grandpa = findNodeById(root, father->id_father);
            if (grandpa) {
                Node* greatUncle = grandpa->left;
                while (greatUncle) {
                    if (greatUncle != father && !greatUncle->is_dead && greatUncle->gender == 'M') {
                        // 4a. Si tiene hijos, buscar el primero varón vivo
                        Node* cousin = greatUncle->left;
                        while (cousin) {
                            if (!cousin->is_dead && cousin->gender == 'M') {
                                return cousin;
                            }
                            cousin = cousin->right;
                        }
                        // 4b. Si no tiene hijos, el tío mismo
                        return greatUncle;
                    }
                    greatUncle = greatUncle->right;
                }
            }
        }

        // 5. Buscar en ancestros con múltiples hijos
        Node* ancestor = node;
        while (ancestor) {
            ancestor = findNodeById(root, ancestor->id_father);
            if (ancestor && ancestor->countChildren() >= 2) {
                Node* uncle = ancestor->left;
                while (uncle) {
                    if (uncle != node && !uncle->is_dead && uncle->gender == 'M') {
                        return uncle;
                    }
                    uncle = uncle->right;
                }
            }
        }

        // 6. Aplicar mismas reglas para mujeres
        child = node->left;
        while (child) {
            if (!child->is_dead && child->gender == 'F') {
                return child;
            }
            child = child->right;
        }

        // 7. Último recurso: buscar cualquier miembro vivo
        return findFirstLivingMember(root);
    }

    void showFullSuccessionLine() {
        if (!root) {
            cout << "No hay datos en el árbol\n";
            return;
        }
        
        cout << "\n=== Línea de sucesión completa (solo miembros vivos) ===\n";
        
        if (!currentChief) {
            cout << "No hay jefe actual en el clan\n";
            return;
        }

        cout << "1. " << currentChief->name << " (ID " << currentChief->id << ")";
        if (currentChief->is_chief) cout << "\tLíder actual";
        cout << endl;

        Node* successor = findNewChief(currentChief);
        int position = 2;
        Node* visited[100] = {nullptr};
        int pathLength = 0;
        visited[pathLength++] = currentChief;

        while (successor && successor != currentChief && pathLength < 100) {
            // Evitar ciclos
            bool alreadyVisited = false;
            for (int i = 0; i < pathLength; i++) {
                if (visited[i] == successor) {
                    alreadyVisited = true;
                    break;
                }
            }
            
            if (alreadyVisited) break;

            visited[pathLength++] = successor;
            
            cout << position++ << ". " << successor->name << " (ID " << successor->id << ")" << endl;
            
            successor = findNewChief(successor);
        }

        // Mostrar al patriarca si es diferente y está vivo
        if (root && !root->is_dead && root != currentChief) {
            bool rootVisited = false;
            for (int i = 0; i < pathLength; i++) {
                if (visited[i] == root) {
                    rootVisited = true;
                    break;
                }
            }
            
            if (!rootVisited) {
                cout << position << ". " << root->name << " (ID " << root->id << ")";
                if (root->age > 70) cout << "\tVivo (pero tiene " << root->age << " años, y no es líder)";
                cout << endl;
            }
        }
    }
   
    void updateChiefAutomatically() {
        if (!currentChief) {
            cout << "No hay jefe actual\n";
            return;
        }

        bool needsNewChief = currentChief->is_dead || currentChief->age > 70;
        
        if (needsNewChief) {
            cout << "El jefe actual " << (currentChief->is_dead ? "ha fallecido" : 
                 "tiene más de 70 años (" + to_string(currentChief->age) + ")") << ". Buscando sucesor...\n";

            Node* newChief = findNewChief(currentChief);

            if (newChief) {
                if (newChief == currentChief) {
                    cout << "El jefe actual sigue siendo válido\n";
                    return;
                }

                currentChief->is_chief = false;
                currentChief->was_chief = true;
                
                newChief->is_chief = true;
                newChief->was_chief = false;
                currentChief = newChief;

                cout << "\n=== NUEVO JEFE ===\n";
                currentChief->print();
                
                updateCSV();
                saveContributorsToCSV();
            } else {
                cout << "No se encontró un sucesor adecuado. El clan queda sin jefe.\n";
                currentChief->is_chief = false;
                currentChief->was_chief = true;
                currentChief = nullptr;
                updateCSV();
            }
        } else {
            cout << "El jefe actual sigue siendo válido (edad: " << currentChief->age << ")\n";
        }
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
    
    void addContributor() {
        if (!currentChief) {
            cout << "No hay jefe actual. No se pueden agregar contribuidores.\n";
            return;
        }
        
        string name, contribution;
        int age, id, level;
        
        cout << "Nombre del contribuidor: ";
        getline(cin, name);
        cout << "Edad: ";
        cin >> age;
        cout << "ID: ";
        cin >> id;
        clearInputBuffer();
        cout << "Descripción de la contribución: ";
        getline(cin, contribution);
        cout << "Nivel de contribución (1-10): ";
        cin >> level;
        clearInputBuffer();
        
        if (level < 1 || level > 10) {
            cout << "Nivel de contribución debe estar entre 1 y 10\n";
            return;
        }
        
        currentChief->addContributor(name, age, id, contribution, level);
        cout << "Contribuidor agregado exitosamente al jefe actual\n";
        saveContributorsToCSV();
    }
    
    void showContributors() {
        if (!currentChief) {
            cout << "No hay jefe actual\n";
            return;
        }
        
        currentChief->printContributors();
    }
   
    void showLivingMembers() {
        cout << "\n=== MIEMBROS VIVOS ===\n";
        printLiving(root, true);
    }
   
    void showDeceasedMembers() {
        cout << "\n=== MIEMBROS FALLECIDOS ===\n";
        printLiving(root, false);
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
};

#endif