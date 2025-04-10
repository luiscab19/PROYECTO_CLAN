#include "node.h"
#include <cstring>
#include <cstdlib>

// Función auxiliar para duplicar strings
char* duplicarString(const char* s) {
    if (!s) return nullptr;
    char* copia = (char*)malloc(strlen(s) + 1);
    if (copia) strcpy(copia, s);
    return copia;
}

// Implementación de Contributor

Contributor::Contributor(const char* n, int a, int i, const char* d, int g) :
    age(a), id(i), contributionGrade(g), next(nullptr) {
    name = duplicarString(n);
    description = duplicarString(d);
}

Contributor::~Contributor() {
    free(name);
    free(description);
    delete next; 
}

// Implementación de ClanMember

ClanMember::ClanMember(int id, const char* name, const char* last_name, char gender, int age, int id_father, bool is_dead, bool was_chief, bool is_chief) :
    id(id), gender(gender), age(age), id_father(id_father),
    is_dead(is_dead), was_chief(was_chief), is_chief(is_chief),
    first_child(nullptr), second_child(nullptr), contributors(nullptr) {
    this->name = duplicarString(name);
    this->last_name = duplicarString(last_name);
}

ClanMember::~ClanMember() {
    free(name);
    free(last_name);
    delete first_child;  // Elimina árbol recursivamente
    delete second_child;
    delete contributors; // Elimina lista de colaboradores
}

void ClanMember::addContributor(Contributor* nuevo) {
    if (!nuevo) return;

    // Caso: lista vacía o nuevo tiene mayor prioridad
    if (!contributors || nuevo->contributionGrade > contributors->contributionGrade) {
        nuevo->next = contributors;
        contributors = nuevo;
        return;
    }

    // Buscar posición de inserción
    Contributor* actual = contributors;
    while (actual->next && (nuevo->contributionGrade < actual->next->contributionGrade || 
        (nuevo->contributionGrade == actual->next->contributionGrade &&  nuevo->age >= actual->next->age))) {
        actual = actual->next;
    }

    // Insertar el nuevo colaborador
    nuevo->next = actual->next;
    actual->next = nuevo;
}

void ClanMember::sortContributors() {
    if (!contributors || !contributors->next) return;

    bool ordenado;
    do {
        ordenado = true;
        Contributor** ptr = &contributors;
        
        while (*ptr && (*ptr)->next) {
            Contributor* actual = *ptr;
            Contributor* siguiente = actual->next;
            
            // Ordenar por grado (desc) y edad (asc)
            if (actual->contributionGrade < siguiente->contributionGrade || (actual->contributionGrade == siguiente->contributionGrade && actual->age > siguiente->age)) {
                actual->next = siguiente->next;
                siguiente->next = actual;
                *ptr = siguiente;
                ordenado = false;
            }
            ptr = &(*ptr)->next;
        }
    } while (!ordenado);
}

void ClanMember::transferContributors(ClanMember* nuevo_jefe) {
    if (!nuevo_jefe) return;
    
    Contributor* actual = contributors;
    while (actual) {
        Contributor* temp = actual->next;
        nuevo_jefe->addContributor(actual);
        actual = temp;
    }
    contributors = nullptr;
}

bool ClanMember::canAddChild() const {
    return !first_child || !second_child;
}

ClanMember* ClanMember::getOtherChild(const ClanMember* hijo) const {
    if (first_child == hijo) return second_child;
    if (second_child == hijo) return first_child;
    return nullptr;
}