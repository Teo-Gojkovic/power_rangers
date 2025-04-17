#include <stdio.h>
#include <stdlib.h>

void extraction(char *data_recorded) { // ADRESSE où est stockée data_recorded
    // Variables pour stocker les données extraites
    char date[20];
    char time[20];
    float temp;
    float humidity;

    // [^,] : permet de lire jusqu'à la prochaine virgule pour les dates et
    sscanf(data_recorded, "%[^,], %[^,] , %f, %f", date, time, &temp, &humidity);

    printf("Date : %s \n", date);
    printf("Time : %s \n", time);
    printf("Temperature : %.1f \n", temp);
    printf("Humidity : %.1f \n", humidity);

    // Ouverture du fichier CSV en mode ajout
    FILE *le_gros_paf_de_maxime = fopen("donnees_temp.csv", "a"); 
    /* Déclaration de variable
       "a" = append --> ajoute les données à la fin du fichier */

    if (le_gros_paf_de_maxime == NULL) { // SI fopen a échoué : interdiction d'écrire dans le fichier pour X raisons
        printf("Erreur lors de l'ouverture du fichier. \n");
        return;
    }

    // Ecriture des données dans le fichier .csv
    fprintf(le_gros_paf_de_maxime, "%s ; %s ; %.1f ; %.1f.\n", date, time, temp, humidity);

    // Fermeture du fichier
    fclose(le_gros_paf_de_maxime);
}

int main() {


    // Exemple de chaîne envoyée par Maxence
    // char data_recorded[] = "10-10-2024, 10:00:30, 24.4, 54.3";
    char *datas_recorded[] = {
        "10-10-2024, 10:30:05, 24.4, 55",
        "10-10-2024, 10:35:05, 26.4, 53",
        "10-10-2024, 10:40:05, 25, 55"
    };

    for (int i = 0; i < 3; i++) {
        extraction(datas_recorded[i]);
        printf("%s\n", datas_recorded[i]); 
        printf("---------\n");
    }

    extraction(*datas_recorded);

    return EXIT_SUCCESS;
}