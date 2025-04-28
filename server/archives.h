#ifndef ARCHIVES_H
#define ARCHIVES_H

typedef struct { // Faire un .h
    char date[20];
    char time[20];
    float temp;
    float humidity;
} DataSensor;

DataSensor extraction(char *data_recorded);

void writing_csv(DataSensor donnees) ;

#endif