#include <stdio.h>
#include <stdlib.h>

typedef struct { 
    char date[20];
    char time[20];
    float temp;
    float humidity;
} DataSensor; // Création de la structure et d'un nouveau type de variable 

DataSensor donnees; // Déclaration d'une nouvelle variable

// --- FONCTIONS ---

DataSensor extraction(char *data_recorded) { // ADRESSE où est stockée data_recorded
    DataSensor d;
    // [^,] : permet de lire jusqu'à la prochaine virgule pour les dates et
    sscanf(data_recorded, "%[^,], %[^,] , %f, %f",
           donnees.date, donnees.time, &donnees.temp, &donnees.humidity);

    printf("Date : %s \n", donnees.date);
    printf("Time : %s \n", donnees.time);
    printf("Temperature : %.1f \n", donnees.temp);
    printf("Humidity : %.1f \n", donnees.humidity);

    return donnees;
}

void writing_csv(DataSensor donnees) {

    // Ouverture du fichier CSV en mode ajout
    FILE *le_gros_paf_de_maxime = fopen("donnees_temp.csv", "a"); 
    /* Déclaration de variable
       "a" = append --> ajoute les données à la fin du fichier */

    if (le_gros_paf_de_maxime == NULL) { // SI fopen a échoué : interdiction d'écrire dans le fichier pour X raisons
        printf("Erreur lors de l'ouverture du fichier. \n");
        return;
    }

    // Ecriture des données dans le fichier .csv
    fprintf(le_gros_paf_de_maxime, "%s ; %s ; %.1f ; %.1f.\n", 
            donnees.date, donnees.time, donnees.temp, donnees.humidity);

    // Fermeture du fichier
    fclose(le_gros_paf_de_maxime);
}

int main() {

    // Exemple de chaîne envoyée par Maxence
    // char data_recorded[] = "10-10-2024, 10:00:30, 24.4, 54.3";
    char *datas_recorded[] = { // Déclaration d'une variable : chaîne de caractères
        "10-10-2024, 11:30:05, 24.4, 55",
        "10-10-2024, 12:35:05, 26.4, 53",
        "10-10-2024, 13:40:05, 25, 55"
    };

    for (int i = 0; i < 3; i++) {
        extraction(datas_recorded[i]);
        writing_csv(donnees); 
        printf("---------\n");
    }

    extraction(*datas_recorded);

    return EXIT_SUCCESS;
}