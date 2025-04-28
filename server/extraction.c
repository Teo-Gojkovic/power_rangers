#include "archives.h"
#include <stdio.h>
#include <stdlib.h>

int main() { // fichier à part pour le main

    DataSensor donnees; // Déclaration d'une nouvelle variable

    // Exemple de chaîne envoyée par Maxence
    // char data_recorded[] = "10-10-2024, 10:00:30, 24.4, 54.3";
    char *datas_recorded[] = { // Déclaration d'une variable : chaîne de caractères
        "10-10-2024, 11:30:05, 24.4, 55",
        "10-10-2024, 12:35:05, 26.4, 53",
        "10-10-2024, 13:40:05, 25, 55"
    };

    for (int i = 0; i < 3; i++) {
        donnees = extraction(datas_recorded[i]);
        
        printf("Date : %s \n", donnees.date);
        printf("Time : %s \n", donnees.time);
        printf("Temperature : %.1f \n", donnees.temp);
        printf("Humidity : %.1f \n", donnees.humidity);

        writing_csv(donnees); 
        printf("---------\n");
    }
    

    extraction(*datas_recorded);

    return EXIT_SUCCESS;
}