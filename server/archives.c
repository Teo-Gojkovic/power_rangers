#include "archives.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

DataSensor extraction(char *data_recorded) {
    DataSensor d;

    // Initialisation à zéro pour éviter des valeurs indéfinies
    memset(&d, 0, sizeof(DataSensor));

    // Lecture des champs séparés par des points-virgules
    // %19[^;] pour éviter le dépassement de tampon
    sscanf(data_recorded, "%19[^;];%19[^;];%f;%f", d.date, d.time, &d.temp, &d.humidity);

    return d;
}

void writing_csv(DataSensor donnees) {
    FILE *f = fopen("donnees_temp.csv", "a");
    if (f == NULL) {
        printf("Erreur lors de l'ouverture du fichier.\n");
        return;
    }
    // Format CSV propre, sans espaces inutiles ni point final
    fprintf(f, "%s;%s;%.2f;%.2f\n", donnees.date, donnees.time, donnees.temp, donnees.humidity);
    fclose(f);
}
