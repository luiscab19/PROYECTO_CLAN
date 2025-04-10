#include ".\tree\tree.h"
#include <iostream>

using namespace std;

void displayMenu() {
    cout << "\n=== Sistema del Clan ===\n"
        << "1. Ingresar nuevo integrante\n"
        << "2. Registrar defunción\n"
        << "3. Mostrar línea de sucesión corta\n"
        << "4. Mostrar línea de sucesión completa\n"
        << "5. Modificar datos de integrante\n"
        << "6. Mostrar miembros fallecidos\n"
        << "7. Mostrar miembros vivos\n"
        << "8. Mostrar estado actual\n"
        << "9. Agregar contribuidor (solo al jefe actual)\n"
        << "10. Mostrar contribuidores del jefe actual\n"
        << "11. Salir\n"
        << "Opción: ";
}

int main() {
    Tree clan;
    cout << "Cargando datos de CLAN.csv...\n";
    if (!clan.loadFromCSV()) {
        cout << "El sistema continuará sin datos iniciales\n";
    }

    int option;
    do {
        displayMenu();
        if (!(cin >> option)) {
            clearInputBuffer();
            option = 0;
        }
        clearInputBuffer();
       
        switch (option) {
            case 1: clan.addMember(); break; //listo
            case 2: clan.markAsDeceased(); break;
            //case 3: clan.showShortSuccessionLine(); break;
            //case 4: clan.showFullSuccessionLine(); break;
            case 5: clan.updateMemberData(); break;
            case 6: clan.showLivingMembers(); break; // Listo
            case 7: clan.showDeceasedMembers(); break; //listo
            //case 8: clan.displayCurrentState(); break;
            //case 9: clan.addContributor(); break; //lista
            //case 10: clan.showContributors(); break;
            //case 11: cout << "Saliendo...\n"; break;
            default: cout << "Opción inválida\n";
        }
    } while (option != 11);

    return 0;
}