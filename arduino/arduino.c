#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <termios.h>
#include <unistd.h>

int ouvrirPortSerie(const char *port) {
    int serial_port = open(port, O_RDWR | O_NOCTTY);
    if (serial_port < 0) {
        perror("Erreur d'ouverture du port série");
        return -1;
    }

    struct termios tty;
    memset(&tty, 0, sizeof tty);

    if (tcgetattr(serial_port, &tty) != 0) {
        perror("Erreur tcgetattr");
        close(serial_port);
        return -1;
    }

    cfsetispeed(&tty, B9600);
    cfsetospeed(&tty, B9600);

    tty.c_cflag &= ~PARENB;
    tty.c_cflag &= ~CSTOPB;
    tty.c_cflag &= ~CSIZE;
    tty.c_cflag |= CS8;

#ifdef CRTSCTS
    tty.c_cflag &= ~CRTSCTS;
#endif
    tty.c_cflag |= CREAD | CLOCAL;

    tty.c_lflag &= ~(ICANON | ECHO | ECHOE | ISIG);
    tty.c_iflag &= ~(IXON | IXOFF | IXANY);
    tty.c_oflag &= ~OPOST;

    tcsetattr(serial_port, TCSANOW, &tty);

    return serial_port;
}

int lireDonneesArduino(int serial_port, float *temperature, float *humidite) {
    char buffer[128];
    memset(buffer, 0, sizeof(buffer));

    int index = 0;
    char c;
    int timeout = 1000000; // 1 million = 1 seconde approx en µs
    int elapsed = 0;

    // Lecture caractère par caractère avec timeout
    while (read(serial_port, &c, 1) == 1) {
        if (c == '\n') {
            buffer[index] = '\0';
            break;
        }
        if (index < sizeof(buffer) - 1) {
            buffer[index++] = c;
        }
    }

    if (index == 0) {
        // Aucun caractère reçu
        return 1; // code spécial pour "waiting"
    }

    printf("Reçu : %s\n", buffer);

    if (sscanf(buffer, "%f;%f", temperature, humidite) != 2) {
        fprintf(stderr, "Format invalide reçu : %s\n", buffer);
        return -1;
    }

    return 0;
}

int main() {
    const char *port = "/dev/ttyACM0";

    int serial_port = ouvrirPortSerie(port);
    if (serial_port < 0) {
        printf("Impossible d’ouvrir le port série.\n");
        return 1;
    }

    while (1) {
        float temp, hum;
        int result = lireDonneesArduino(serial_port, &temp, &hum);

        if (result == 0) {
            // Données valides
            printf("Température : %.2f °C\n", temp);
            printf("Humidité : %.2f %%\n\n", hum);
        } else if (result == 1) {
            // Rien reçu : attente normale
            printf("Waiting...\n");
            sleep(1); // Affiche "Waiting..." toutes les secondes
        } else {
            // Erreur dans les données
            printf("Erreur de lecture des données.\n");
        }
    }

    close(serial_port);
    return 0;
}