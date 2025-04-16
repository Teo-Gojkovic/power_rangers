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

#ifdef CRTSCTS
    tty.c_cflag &= ~CRTSCTS; // Désactive le contrôle de flux matériel (si défini)
#endif
    tty.c_cflag |= CREAD | CLOCAL; // Active la lecture et désactive le contrôle modem

    // Configure le mode d'entrée/sortie :
    tty.c_lflag &= ~(ICANON | ECHO | ECHOE | ISIG); // Mode non canonique (lecture immédiate), pas d'écho
    tty.c_iflag &= ~(IXON | IXOFF | IXANY);         // Désactive le contrôle de flux logiciel
    tty.c_oflag &= ~OPOST;                          // Pas de traitement de sortie

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
    }

    printf("Reçu : %s\n", buffer); // Affiche la ligne reçue

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