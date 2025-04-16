#include <stdio.h> // Pour printf
#include <stdlib.h> // Pour exit et rand
#include <string.h> // Pour memset
#include <time.h> // Pour srand et rand
#include <fcntl.h>      // Pour ouvrir des fichiers ou des ports (ici, le port série)
#include <termios.h>    // Pour configurer les paramètres du port série
#include <arpa/inet.h> // Pour inet_addr et les structures réseau
#include <unistd.h> // Pour close

// Définition des constantes
#define DECALAGE_CESAR 5 // Décalage pour le chiffrement de César
#define PORT 1618 // Numéro de port utilisé pour la communication
#define SERVER_IP "127.0.0.1" // Adresse IP du serveur (localhost)

// Fonction pour chiffrer un message avec le chiffrement de César
void chiffrer_cesar(char *buffer) {
    for (int i = 0; buffer[i] != '\0'; i++) {
        if (buffer[i] >= 32 && buffer[i] <= 126) {
            buffer[i] = ((buffer[i] - 32 + DECALAGE_CESAR) % 95) + 32;
        }
    }
}

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

// Modifier la fonction lireDepuisArduino pour accepter un port série déjà ouvert
void lireDepuisArduino(int serial_port, char *formatted_data) {
    float temp, hum;
    int result = lireDonneesArduino(serial_port, &temp, &hum);

    if (result == 0) {
        time_t now = time(NULL);
        struct tm *t = localtime(&now);

        snprintf(formatted_data, 1024, "%02d-%02d-%04d,%02d-%02d-%02d,%.2f,%.2f",
                 t->tm_mday, t->tm_mon + 1, t->tm_year + 1900,
                 t->tm_hour, t->tm_min, t->tm_sec,
                 temp, hum);
        
        return; // Succès
    }

    // En cas d'erreur, mettre une chaîne vide
    formatted_data[0] = '\0';
}

// Modifier le main pour gérer correctement la boucle et le port série
int main() {
    int sockfd; // Descripteur de socket
    struct sockaddr_in server_addr; // Structure pour l'adresse du serveur
    char buffer[1024]; // Buffer pour envoyer les données au serveur
    const char *port = "/dev/ttyACM0"; // Chemin du port série (modifiez si nécessaire)

    // Création de la socket
    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("Échec de la création de la socket"); // Afficher une erreur si la création échoue
        exit(EXIT_FAILURE); // Quitter le programme en cas d'échec
    }

    // Configuration de l'adresse du serveur
    memset(&server_addr, 0, sizeof(server_addr)); // Initialiser la structure à zéro
    server_addr.sin_family = AF_INET; // Utiliser IPv4
    server_addr.sin_port = htons(PORT); // Convertir le port en format réseau
    server_addr.sin_addr.s_addr = inet_addr(SERVER_IP); // Convertir l'adresse IP en format réseau

    // Établir une connexion avec le serveur
    if (connect(sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("Échec de la connexion au serveur"); // Afficher une erreur si la connexion échoue
        close(sockfd); // Fermer la socket
        exit(EXIT_FAILURE); // Quitter le programme en cas d'échec
    }

    printf("Connexion au serveur réussie.\n");

    // Ouvrir le port série une seule fois
    int serial_port = ouvrirPortSerie(port);
    if (serial_port < 0) {
        printf("Impossible d'ouvrir le port série.\n");
        close(sockfd);
        exit(EXIT_FAILURE);
    }

    printf("Port série ouvert avec succès.\n");

    // Boucle principale
    while (1) {
        lireDepuisArduino(serial_port, buffer);
        
        if (strlen(buffer) > 0) {
            // Chiffrer et envoyer seulement si on a des données valides
            chiffrer_cesar(buffer);
            printf("Message à envoyer : %s\n", buffer);

            
            if (send(sockfd, buffer, strlen(buffer), 0) < 0) {
                perror("Échec de l'envoi du message");
                close(sockfd);
                close(serial_port);
                exit(EXIT_FAILURE);
            }

            printf("Message envoyé : %s\n", buffer);
        }

        // Délai entre chaque lecture
        sleep(1);
    }

    // Fermeture des connexions
    close(serial_port);
    close(sockfd);
    return 0;
}
