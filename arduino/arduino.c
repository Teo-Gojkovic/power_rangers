#include <stdio.h>      // Pour printf(), fprintf(), perror()
#include <stdlib.h>     // Pour exit(), etc.
#include <string.h>     // Pour memset(), manipulation de chaînes
#include <fcntl.h>      // Pour open(), O_RDWR, etc.
#include <termios.h>    // Pour la configuration du port série
#include <unistd.h>     // Pour read(), close(), sleep(), etc.

int ouvrirPortSerie(const char *port) { // Fonction qui ouvre et configure le port série
    int serial_port = open(port, O_RDWR | O_NOCTTY); // Ouvre le port en lecture/écriture sans le lier au terminal
    if (serial_port < 0) { // Vérifie si l'ouverture a échoué
        perror("Erreur d'ouverture du port série"); // Affiche l'erreur système
        return -1; // Retourne -1 en cas d'erreur
    }

    struct termios tty; // Structure de configuration du port série
    memset(&tty, 0, sizeof tty); // Initialise la structure à zéro

    if (tcgetattr(serial_port, &tty) != 0) { // Récupère les paramètres actuels du port
        perror("Erreur tcgetattr"); // Affiche l'erreur si échec
        close(serial_port); // Ferme le port si erreur
        return -1; // Retourne -1
    }

    cfsetispeed(&tty, B9600); // Définit la vitesse d'entrée à 9600 bauds
    cfsetospeed(&tty, B9600); // Définit la vitesse de sortie à 9600 bauds

    tty.c_cflag &= ~PARENB; // Désactive la parité
    tty.c_cflag &= ~CSTOPB; // Utilise 1 bit de stop
    tty.c_cflag &= ~CSIZE;  // Efface les bits de taille
    tty.c_cflag |= CS8;     // Définit 8 bits de données

#ifdef CRTSCTS
    tty.c_cflag &= ~CRTSCTS; // Désactive le contrôle de flux matériel (si défini)
#endif
    tty.c_cflag |= CREAD | CLOCAL; // Active la lecture et désactive le contrôle modem

    tty.c_lflag &= ~(ICANON | ECHO | ECHOE | ISIG); // Mode non canonique (lecture immédiate), pas d'écho
    tty.c_iflag &= ~(IXON | IXOFF | IXANY);         // Désactive le contrôle de flux logiciel
    tty.c_oflag &= ~OPOST;                          // Pas de traitement de sortie

    tcsetattr(serial_port, TCSANOW, &tty); // Applique la configuration immédiatement

    return serial_port; // Retourne le port configuré
}

int lireDonneesArduino(int serial_port, float *temperature, float *humidite) { // Lit une ligne venant de l'Arduino et extrait temp + humidité
    char buffer[128]; // Tampon pour stocker la ligne reçue
    memset(buffer, 0, sizeof(buffer)); // Initialise à zéro

    int index = 0; // Position actuelle dans le buffer
    char c; // Caractère lu
    int timeout = 1000000; // Non utilisé ici, on pourrait l'utiliser pour une lecture avec délai
    int elapsed = 0; // Non utilisé ici non plus

    // Lecture caractère par caractère jusqu'à '\n'
    while (read(serial_port, &c, 1) == 1) {
        if (c == '\n') { // Si fin de ligne, on termine la chaîne
            buffer[index] = '\0';
            break;
        }
        if (index < sizeof(buffer) - 1) { // Ajoute le caractère au buffer
            buffer[index++] = c;
        }
    }

    if (index == 0) { // Si rien n’a été reçu
        return 1; // Retourne 1 → "waiting"
    }

    printf("Reçu : %s\n", buffer); // Affiche la ligne reçue

    if (sscanf(buffer, "%f;%f", temperature, humidite) != 2) { // Essaie d'extraire 2 float séparés par un ';'
        fprintf(stderr, "Format invalide reçu : %s\n", buffer); // Affiche une erreur si mauvais format
        return -1; // Retourne -1 en cas d'erreur
    }

    return 0; // Succès
}

int main() {
    const char *port = "/dev/ttyACM0"; // Chemin du port série à ouvrir

    int serial_port = ouvrirPortSerie(port); // Ouvre et configure le port
    if (serial_port < 0) { // Si échec
        printf("Impossible d’ouvrir le port série.\n");
        return 1; // Quitte le programme avec erreur
    }

    while (1) { // Boucle infinie
        float temp, hum; // Variables pour température et humidité
        int result = lireDonneesArduino(serial_port, &temp, &hum); // Lit les données

        if (result == 0) {
            // Données valides reçues
            printf("Température : %.2f °C\n", temp);
            printf("Humidité : %.2f %%\n\n", hum);
        } else if (result == 1) {
            // Aucun caractère reçu pour l'instant → attente
            printf("Waiting...\n");
            sleep(1); // Pause de 1 seconde
        } else {
            // Format incorrect
            printf("Erreur de lecture des données.\n");
        }
    }

    close(serial_port); // Ferme le port série (jamais atteint ici car boucle infinie)
    return 0; // Fin du programme
}
