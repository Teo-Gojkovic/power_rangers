#include "archives.h"
#include <stdio.h>
#include <stdlib.h>

DataSensor extraction(char *data_recorded) { // ADRESSE où est stockée data_recorded
    DataSensor d;
    // [^,] : permet de lire jusqu'à la prochaine virgule pour les dates et
    sscanf(data_recorded, "%[^,], %[^,] , %f, %f", d.date, d.time, &d.temp, &d.humidity);

    return d;
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