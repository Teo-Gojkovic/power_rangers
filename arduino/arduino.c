<<<<<<< HEAD
#include <stdio.h>      // Bibliothèque standard pour afficher des messages et gérer les erreurs
#include <stdlib.h>     // Bibliothèque standard pour gérer les fonctions comme exit()
#include <string.h>     // Pour manipuler des chaînes de caractères
#include <fcntl.h>      // Pour ouvrir des fichiers ou des ports (ici, le port série)
#include <termios.h>    // Pour configurer les paramètres du port série
#include <unistd.h>     // Pour des fonctions comme read(), close(), etc.

// Fonction pour ouvrir et configurer un port série
int ouvrirPortSerie(const char *port) {
    // Ouvre le port série en mode lecture/écriture sans le lier au terminal
    int serial_port = open(port, O_RDWR | O_NOCTTY);
    if (serial_port < 0) { // Vérifie si l'ouverture a échoué
        perror("Erreur d'ouverture du port série"); // Affiche un message d'erreur
        return -1; // Retourne -1 pour indiquer une erreur
    }

    struct termios tty; // Structure pour stocker les paramètres du port série
    memset(&tty, 0, sizeof tty); // Initialise la structure à zéro

    // Récupère les paramètres actuels du port série
    if (tcgetattr(serial_port, &tty) != 0) {
        perror("Erreur tcgetattr"); // Affiche une erreur si la récupération échoue
        close(serial_port); // Ferme le port série
        return -1; // Retourne -1 pour indiquer une erreur
    }

    // Définit la vitesse de communication à 9600 bauds (standard pour Arduino)
    cfsetispeed(&tty, B9600);
    cfsetospeed(&tty, B9600);

    // Configure les paramètres du port série :
    tty.c_cflag &= ~PARENB; // Désactive la parité (aucun bit de vérification d'erreur)
    tty.c_cflag &= ~CSTOPB; // Utilise 1 bit de stop (standard)
    tty.c_cflag &= ~CSIZE;  // Efface les bits de taille
    tty.c_cflag |= CS8;     // Définit 8 bits de données (standard)
=======
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
>>>>>>> main

#ifdef CRTSCTS
    tty.c_cflag &= ~CRTSCTS; // Désactive le contrôle de flux matériel (si défini)
#endif
    tty.c_cflag |= CREAD | CLOCAL; // Active la lecture et désactive le contrôle modem

<<<<<<< HEAD
    // Configure le mode d'entrée/sortie :
=======
>>>>>>> main
    tty.c_lflag &= ~(ICANON | ECHO | ECHOE | ISIG); // Mode non canonique (lecture immédiate), pas d'écho
    tty.c_iflag &= ~(IXON | IXOFF | IXANY);         // Désactive le contrôle de flux logiciel
    tty.c_oflag &= ~OPOST;                          // Pas de traitement de sortie

<<<<<<< HEAD
    // Applique les paramètres immédiatement
    tcsetattr(serial_port, TCSANOW, &tty);

    return serial_port; // Retourne le descripteur du port série configuré
}

// Fonction pour lire les données envoyées par l'Arduino
int lireDonneesArduino(int serial_port, float *temperature, float *humidite) {
    char buffer[128]; // Tampon pour stocker la ligne reçue
    memset(buffer, 0, sizeof(buffer)); // Initialise le tampon à zéro

    int index = 0; // Position actuelle dans le tampon
    char c; // Variable pour stocker chaque caractère lu

    // Boucle pour lire les caractères un par un jusqu'à trouver un '\n' (fin de ligne)
    while (read(serial_port, &c, 1) == 1) {
        if (c == '\n') { // Si on trouve un '\n', on termine la chaîne
            buffer[index] = '\0'; // Ajoute un caractère nul pour terminer la chaîne
            break; // Sort de la boucle
        }
        if (index < sizeof(buffer) - 1) { // Si le tampon n'est pas plein
            buffer[index++] = c; // Ajoute le caractère au tampon
        }
    }

    if (index == 0) { // Si aucun caractère n'a été lu
        return 1; // Retourne 1 pour indiquer qu'on attend encore des données
=======
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
>>>>>>> main
    }

    printf("Reçu : %s\n", buffer); // Affiche la ligne reçue

<<<<<<< HEAD
    // Essaie d'extraire deux nombres flottants séparés par un point-virgule
    if (sscanf(buffer, "%f;%f", temperature, humidite) != 2) {
        fprintf(stderr, "Format invalide reçu : %s\n", buffer); // Affiche une erreur si le format est incorrect
        return -1; // Retourne -1 pour indiquer une erreur
    }

    return 0; // Retourne 0 pour indiquer que tout s'est bien passé
}

// Fonction pour lire les données depuis l'Arduino
void lireDepuisArduino(const char *port) {
    // Ouvre et configure le port série
    int serial_port = ouvrirPortSerie(port);
    if (serial_port < 0) { // Si l'ouverture échoue
        printf("Impossible d’ouvrir le port série.\n");
        return; // Quitte la fonction en cas d'erreur
    }

    while (1) { // Boucle infinie pour lire les données en continu
        float temp, hum; // Variables pour stocker la température et l'humidité
        int result = lireDonneesArduino(serial_port, &temp, &hum); // Lit les données

        if (result == 0) { // Si les données sont valides
            printf("Température : %.2f °C\n", temp); // Affiche la température
            printf("Humidité : %.2f %%\n\n", hum); // Affiche l'humidité
        } else if (result == 1) { // Si aucune donnée n'a été reçue
            printf("Waiting...\n"); // Affiche un message d'attente
            sleep(1); // Attend 1 seconde avant de réessayer
        } else { // Si une erreur s'est produite
            printf("Erreur de lecture des données.\n"); // Affiche un message d'erreur
        }
    }

    close(serial_port); // Ferme le port série (jamais atteint ici à cause de la boucle infinie)
}

int main() {
    const char *port = "/dev/ttyACM0"; // Chemin du port série (modifiez si nécessaire)

    lireDepuisArduino(port); // Appelle la fonction pour lire les données depuis l'Arduino

    return 0; // Quitte le programme
}
=======
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
>>>>>>> main
