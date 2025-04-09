#ifndef NODE_H
#define NODE_H

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
    ClanMember* first_child;
    ClanMember* next_sibling;
    Contributor* contributors;
    
    ClanMember(int id, const char* name, const char* last_name, char gender, int age, int id_father, bool is_dead, bool was_chief, bool is_chief);
    ~ClanMember();
    
    void addContributor(Contributor* contributor);
    void sortContributors();
};

#endif // NODE_H