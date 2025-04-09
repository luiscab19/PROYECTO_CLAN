#include "node.h"
#include <iostream>
#include <cstring>

using namespace std;

Contributor::Contributor(const char* n, int a, int i, const char* d, int g) : 
    age(a), id(i), contributionGrade(g), next(nullptr) { 
    name = new char[strlen(n) + 1];
    strcpy(name, n);
    
    description = new char[strlen(d) + 1];
    strcpy(description, d);
}

Contributor::~Contributor() {
    delete[] name;
    delete[] description;
    delete next;
}

ClanMember::ClanMember(int id, const char* name, const char* last_name, char gender, int age, int id_father, bool is_dead, bool was_chief, bool is_chief) :
    id(id), gender(gender), age(age), id_father(id_father), is_dead(is_dead), was_chief(was_chief), is_chief(is_chief), first_child(nullptr), next_sibling(nullptr), contributors(nullptr) {
    
    this->name = new char[strlen(name) + 1];
    strcpy(this->name, name);
    
    this->last_name = new char[strlen(last_name) + 1];
    strcpy(this->last_name, last_name);
}

ClanMember::~ClanMember() {
    delete[] name;
    delete[] last_name;
    delete first_child;
    delete next_sibling;
    delete contributors;
}

void ClanMember::addContributor(Contributor* new_contributor) {
    if (!contributors) {
        contributors = new_contributor;
    } else {
        Contributor* current = contributors;
        while (current->next) {
            current = current->next;
        }
        current->next = new_contributor;
    }
    sortContributors();
}

void ClanMember::sortContributors() {
    if (!contributors || !contributors->next) return;
    
    bool swapped;
    do {
        swapped = false;
        Contributor** ptr = &contributors;
        
        while ((*ptr)->next) {
            Contributor* first = *ptr;
            Contributor* second = first->next;
            
            bool should_swap = false;
            if (first->contributionGrade < second->contributionGrade) {
                should_swap = true;
            } else if (first->contributionGrade == second->contributionGrade) {
                if (first->age > second->age) {
                    should_swap = true;
                }
            }
            
            if (should_swap) {
                first->next = second->next;
                second->next = first;
                *ptr = second;
                swapped = true;
            }
            ptr = &(*ptr)->next;
        }
    } while (swapped);
}