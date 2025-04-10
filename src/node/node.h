#ifndef NODE_H
#define NODE_H

#include <string>
using namespace std;

class Contributor {
public:
    string name;
    int age;
    int id;
    string contribution;
    int contribution_level;

    Contributor() : name(""), age(0), id(0), contribution(""), contribution_level(0) {}
    Contributor(string n, int a, int i, string c, int cl) : 
        name(n), age(a), id(i), contribution(c), contribution_level(cl) {}

    bool operator>(const Contributor& other) const;
};

class Node {
public:
    int id;
    string name;
    string last_name;
    char gender;
    int age;
    int id_father;
    bool is_dead;
    bool was_chief;
    bool is_chief;
    Contributor* contributors;
    int contributorCount;
    int contributorCapacity;
   
    Node* left;
    Node* right;
   
    Node(int id, string name, string last_name, char gender, int age, int id_father, bool is_dead, bool was_chief, bool is_chief);
    ~Node();
   
    bool hasChildren();
    bool hasBrothers();
    int countChildren();
    Node* findChild();
    Node* findBrother();
    Node* findUncle(Node* father);
    Node* findAncestorWithTwoChildren();
    void print();
    void printContributors();
    string toCSV();
    void addContributor(string name, int age, int id, string contribution, int level);
    void sortContributors();
    void loadContributors(const string& filename, int memberId);
};

#endif