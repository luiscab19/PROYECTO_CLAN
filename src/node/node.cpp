#include "node.h"
#include <iostream>
#include <fstream>
#include <sstream>

using namespace std;

bool Contributor::operator>(const Contributor& other) const {
    if (contribution_level != other.contribution_level) {
        return contribution_level > other.contribution_level;
    }
    return age > other.age;
}

Node::Node(int id, string name, string last_name, char gender, int age, 
     int id_father, bool is_dead, bool was_chief, bool is_chief) :
    id(id), name(name), last_name(last_name), gender(toupper(gender)), age(age),
    id_father(id_father), is_dead(is_dead), was_chief(was_chief),
    is_chief(is_chief), left(nullptr), right(nullptr),
    contributors(nullptr), contributorCount(0), contributorCapacity(0) {}

Node::~Node() {
    delete left;
    delete right;
    delete[] contributors;
}

bool Node::hasChildren() { return left != nullptr; }
bool Node::hasBrothers() { return right != nullptr; }

int Node::countChildren() {
    int count = 0;
    Node* child = left;
    while (child != nullptr) {
        count++;
        child = child->right;
    }
    return count;
}

Node* Node::findChild() {
    if (!hasChildren()) return nullptr;
    Node* current = left;
    while (current != nullptr) {
        if (!current->is_dead) return current;
        current = current->right;
    }
    return nullptr;
}

Node* Node::findBrother() {
    if (!hasBrothers()) return nullptr;
    Node* current = right;
    while (current != nullptr) {
        if (!current->is_dead) return current;
        current = current->right;
    }
    return nullptr;
}

Node* Node::findUncle(Node* father) {
    if (father == nullptr) return nullptr;
    return father->findBrother();
}

Node* Node::findAncestorWithTwoChildren() {
    Node* current = this;
    while (current != nullptr) {
        if (current->left != nullptr && current->left->right != nullptr) {
            return current;
        }
        current = current->left;
    }
    return nullptr;
}

void Node::print() {
    cout << "ID: " << id << "\nNombre: " << name << " " << last_name
         << "\nGénero: " << gender << "\nEdad: " << age
         << "\nID Padre: " << (id_father == -1 ? "Ninguno" : to_string(id_father))
         << "\nEstá muerto: " << (is_dead ? "Sí" : "No")
         << "\nFue jefe: " << (was_chief ? "Sí" : "No")
         << "\nEs jefe: " << (is_chief ? "Sí" : "No")
         << "\n----------------------------\n";
}

void Node::printContributors() {
    if (contributorCount == 0) {
        cout << "No hay contribuidores registrados para este miembro.\n";
        return;
    }
    
    cout << "\n=== Contribuidores de " << name << " " << last_name << " ===\n";
    for (int i = 0; i < contributorCount; i++) {
        cout << "Nombre: " << contributors[i].name << "\nEdad: " << contributors[i].age 
             << "\nID: " << contributors[i].id << "\nContribución: " << contributors[i].contribution
             << "\nNivel: " << contributors[i].contribution_level << "/10"
             << "\n----------------------------\n";
    }
}

string Node::toCSV() {
    stringstream ss;
    ss << id << ";" << name << ";" << last_name << ";" << gender << ";"
       << age << ";" << (id_father == -1 ? "" : to_string(id_father)) << ";"
       << (is_dead ? "1" : "0") << ";" << (was_chief ? "1" : "0") << ";"
       << (is_chief ? "1" : "0");
    return ss.str();
}

void Node::addContributor(string name, int age, int id, string contribution, int level) {
    if (contributorCount == contributorCapacity) {
        int newCapacity = contributorCapacity == 0 ? 2 : contributorCapacity * 2;
        Contributor* newContributors = new Contributor[newCapacity];
        
        for (int i = 0; i < contributorCount; i++) {
            newContributors[i] = contributors[i];
        }
        
        delete[] contributors;
        contributors = newContributors;
        contributorCapacity = newCapacity;
    }
    
    contributors[contributorCount++] = Contributor(name, age, id, contribution, level);
    sortContributors();
}

void Node::sortContributors() {
    for (int i = 0; i < contributorCount - 1; i++) {
        for (int j = 0; j < contributorCount - i - 1; j++) {
            if (contributors[j] > contributors[j + 1]) {
                Contributor temp = contributors[j];
                contributors[j] = contributors[j + 1];
                contributors[j + 1] = temp;
            }
        }
    }
}

void Node::loadContributors(const string& filename, int memberId) {
    ifstream file(filename);
    if (!file.is_open()) return;
    
    string line;
    getline(file, line); // Header
    
    while (getline(file, line)) {
        if (line.empty()) continue;
        
        stringstream ss(line);
        string token;
        string tokens[6];
        int tokenCount = 0;
        
        while (getline(ss, token, ';') && tokenCount < 6) {
            tokens[tokenCount++] = token;
        }
        
        if (tokenCount < 6) continue;
        
        int currentMemberId = stoi(tokens[0]);
        if (currentMemberId != memberId) continue;
        
        string name = tokens[1];
        int age = stoi(tokens[2]);
        int id = stoi(tokens[3]);
        string contribution = tokens[4];
        int level = stoi(tokens[5]);
        
        addContributor(name, age, id, contribution, level);
    }
    
    file.close();
}