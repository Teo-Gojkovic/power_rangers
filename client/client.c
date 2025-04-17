#include <stdio.h>       // Bibliothèque standard pour afficher des messages et gérer les erreurs
#include <stdlib.h>      // Bibliothèque standard pour gérer les fonctions comme exit()
#include <string.h>      // Pour manipuler des chaînes de caractères
#include <arpa/inet.h>   // Pour les fonctions de réseau (comme socket, connect, etc.)
#include <unistd.h>      // Pour des fonctions comme read(), close(), etc.
#include <netinet/tcp.h> // Pour TCP_NODELAY
#include <time.h>        // Pour time()
   
#include <fcntl.h>      // Pour ouvrir des fichiers ou des ports (ici, le port série)
#include <termios.h>    // Pour configurer les paramètres du port série
#include <unistd.h>     // Pour des fonctions comme read(), close(), etc.

// Définition des constantes
#define PORT 1618        // Numéro de port utilisé pour la communication
#define SERVER_IP "127.0.0.1" // Adresse IP du serveur (localhost)
#define DECALAGE_CESAR 5 // Décalage pour le chiffrement de César

// Fonction pour le serveur
int connecter_au_serveur(struct sockaddr_in *server_addr);

// Fonction pour l'arduino
int ouvrirPortSerie(const char *port);
int lireDonneesArduino(int serial_port, char *buffer, size_t buffer_size);
void lireDepuisArduino(const char *port);

// Fonction pour chiffrer un message avec le chiffrement de César
void chiffrer_cesar(char *buffer);

// Fonction principale
int main() {
    const char *port = "/dev/ttyACM0"; // Chemin du port série (modifiez si nécessaire)

    int sockfd;
    struct sockaddr_in server_addr;
    char buffer[1024];

    // Configuration de l'adresse du serveur
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    server_addr.sin_addr.s_addr = inet_addr(SERVER_IP);

    // Établir une première connexion
    sockfd = connecter_au_serveur(&server_addr);
    if (sockfd < 0) {
        exit(EXIT_FAILURE);
    }

    // Ouvrir le port série pour lire les données de l'Arduino
    int serial_port = ouvrirPortSerie(port);
    if (serial_port < 0) {
        printf("Impossible d’ouvrir le port série.\n");
        close(sockfd);
        exit(EXIT_FAILURE);
    }

    // Boucle principale pour lire les données de l'Arduino et les envoyer au serveur
    while (1) {
        char raw_data[128]; // Tampon pour stocker les données brutes reçues de l'Arduino
        char formatted_data[1024]; // Tampon pour stocker les données formatées avec horodatage
        int result = lireDonneesArduino(serial_port, raw_data, sizeof(raw_data));

        if (result == 0) { // Si les données sont valides
            printf("Reçu : %s\n", raw_data); // Affiche les données brutes reçues

            // Ajouter un horodatage et formater les données
            time_t now = time(NULL);
            struct tm *t = localtime(&now);

            snprintf(formatted_data, sizeof(formatted_data), "%02d-%02d-%04d;%02d-%02d-%02d;%s",
                     t->tm_mday, t->tm_mon + 1, t->tm_year + 1900,
                     t->tm_hour, t->tm_min, t->tm_sec,
                     raw_data);

            printf("Données formatées : %s\n", formatted_data);

            // Chiffrer les données avant l'envoi
            chiffrer_cesar(formatted_data);
            printf("Données chiffrées : %s\n", formatted_data);

            if (send(sockfd, formatted_data, strlen(formatted_data), 0) < 0) {
                perror("Échec de l'envoi du message");

                // Tentative de reconnexion en boucle infinie
                close(sockfd);
                printf("Tentative de reconnexion au serveur...\n");
                while ((sockfd = connecter_au_serveur(&server_addr)) < 0) {
                    printf("Nouvelle tentative dans 2 secondes...\n");
                    sleep(2);
                }
                continue; // Réessayer d'envoyer après reconnexion
            } else {
                printf("Message envoyé : %s\n", formatted_data);
            }
        } else if (result == 1) { // Si aucune donnée n'a été reçue
            printf("En attente des données de l'Arduino...\n");
        } else { // Si une erreur s'est produite
            printf("Erreur de lecture des données de l'Arduino.\n");
        }

        printf("\n -------------------------------------- \n");
        sleep(2); // Délai entre chaque envoi
    }

    // Fermeture des connexions
    close(serial_port);
    close(sockfd);
    return 0;
}

// Fonction pour établir une connexion au serveur
int connecter_au_serveur(struct sockaddr_in *server_addr) {
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
        perror("Échec de la création de la socket");
        return -1;
    }

    // Désactiver le Nagle's Algorithm pour envoyer les données immédiatement
    int flag = 1;
    if (setsockopt(sockfd, IPPROTO_TCP, TCP_NODELAY, (void *)&flag, sizeof(flag)) < 0) {
        perror("Erreur lors de la désactivation de Nagle's Algorithm");
        close(sockfd);
        return -1;
    }

    // Connexion au serveur
    if (connect(sockfd, (struct sockaddr *)server_addr, sizeof(*server_addr)) < 0) {
        perror("Échec de la connexion au serveur");
        close(sockfd);
        return -1;
    }

    printf("Connexion au serveur réussie.\n");
    return sockfd;
}

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

// Nouvelle signature de la fonction pour lire les données brutes depuis l'Arduino
int lireDonneesArduino(int serial_port, char *buffer, size_t buffer_size) {
    memset(buffer, 0, buffer_size); // Initialise le tampon à zéro

    int index = 0; // Position actuelle dans le tampon
    char c; // Variable pour stocker chaque caractère lu

    // Boucle pour lire les caractères un par un jusqu'à trouver un '\n' (fin de ligne)
    while (read(serial_port, &c, 1) == 1) {
        if (c == '\n') { // Si on trouve un '\n', on termine la chaîne
            buffer[index] = '\0'; // Ajoute un caractère nul pour terminer la chaîne
            break; // Sort de la boucle
        }
        if (index < buffer_size - 1) { // Si le tampon n'est pas plein
            buffer[index++] = c; // Ajoute le caractère au tampon
        }
    }

    if (index == 0) { // Si aucun caractère n'a été lu
        return 1; // Retourne 1 pour indiquer qu'on attend encore des données
    }

    printf("Reçu : %s\n", buffer); // Affiche la ligne reçue
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
        char raw_data[128]; // Tampon pour stocker les données brutes reçues
        int result = lireDonneesArduino(serial_port, raw_data, sizeof(raw_data)); // Lit les données

        if (result == 0) { // Si les données sont valides
            printf("Données brutes reçues : %s\n", raw_data); // Affiche les données brutes
        } else if (result == 1) { // Si aucune donnée n'a été reçue
            printf("En attente des données de l'Arduino...\n");
            sleep(1); // Attend 1 seconde avant de réessayer
        } else { // Si une erreur s'est produite
            printf("Erreur de lecture des données de l'Arduino.\n");
        }
    }

    close(serial_port); // Ferme le port série (jamais atteint ici à cause de la boucle infinie)
}

void chiffrer_cesar(char *buffer) {
    for (int i = 0; buffer[i] != '\0'; i++) {
        if (buffer[i] >= 32 && buffer[i] <= 126) {
            buffer[i] = ((buffer[i] - 32 + DECALAGE_CESAR) % 95) + 32;
        }
    }
}