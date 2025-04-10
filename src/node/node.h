#ifndef NODE_H
#define NODE_H

// Estructura que representa a un colaborador del clan
struct Contributor {
    char* name;               
    int age;                  
    int id;                   
    char* description;        
    int contributionGrade;    
    Contributor* next;        
    
    Contributor(const char* n, int a, int i, const char* d, int g);
    ~Contributor();
};

// Estructura que representa un miembro del clan
struct ClanMember {
    int id;                  
    char* name;              
    char* last_name;         
    char gender;             
    int age;                 
    int id_father;          
    bool is_dead;            
    bool was_chief;          
    bool is_chief;           
    ClanMember* first_child;  // Primer hijo 
    ClanMember* second_child; // Segundo hijo
    Contributor* contributors; // Lista de colaboradores
    
    ClanMember(int id, const char* name, const char* last_name, char gender, int age, int id_father, bool is_dead, bool was_chief, bool is_chief);
    ~ClanMember();
    
    void addContributor(Contributor* contributor);
    void sortContributors();
    void transferContributors(ClanMember* new_chief);
    bool canAddChild() const;
    ClanMember* getOtherChild(const ClanMember* child) const;
};

#endif // NODE_H