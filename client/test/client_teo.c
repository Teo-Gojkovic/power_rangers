#include <stdio.h>    // Pour printf
#include <stdlib.h>   // Pour exit et rand
#include <string.h>   // Pour memset
#include <time.h>     // Pour srand et rand
#include <fcntl.h>    // Pour ouvrir des fichiers ou des ports (ici, le port série)
#include <termios.h>  // Pour configurer les paramètres du port série
#include <arpa/inet.h>// Pour inet_addr et les structures réseau
#include <unistd.h>   // Pour close
#include <netinet/tcp.h> // Pour TCP_NODELAY

// Définition des constantes
#define DECALAGE_CESAR 5 // Décalage pour le chiffrement de César
#define PORT 1618        // Numéro de port utilisé pour la communication
#define SERVER_IP "127.0.0.1" // Adresse IP du serveur (localhost)

// Fonction pour chiffrer un message avec le chiffrement de César
// Cette fonction applique un décalage sur les caractères imprimables (ASCII 32 à 126)
void chiffrer_cesar(char *buffer) {
    for (int i = 0; buffer[i] != '\0'; i++) {
        if (buffer[i] >= 32 && buffer[i] <= 126) {
            buffer[i] = ((buffer[i] - 32 + DECALAGE_CESAR) % 95) + 32;
        }
    }
    printf("Chiffrement César effectué.\n");
}

// Fonction pour ouvrir et configurer un port série
// Retourne le descripteur du port série ou -1 en cas d'erreur
int ouvrirPortSerie(const char *port) {
    int serial_port = open(port, O_RDWR | O_NOCTTY); // Ouvre le port série
    if (serial_port < 0) {
        perror("Erreur d'ouverture du port série");
        return -1;
    }

    struct termios tty;
    memset(&tty, 0, sizeof tty);

    if (tcgetattr(serial_port, &tty) != 0) { // Récupère les paramètres actuels
        perror("Erreur tcgetattr");
        close(serial_port);
        return -1;
    }

    // Configuration des paramètres du port série
    cfsetispeed(&tty, B9600);
    cfsetospeed(&tty, B9600);
    tty.c_cflag &= ~PARENB; // Pas de parité
    tty.c_cflag &= ~CSTOPB; // 1 bit de stop
    tty.c_cflag &= ~CSIZE;  // Efface les bits de taille
    tty.c_cflag |= CS8;     // 8 bits de données
#ifdef CRTSCTS
    tty.c_cflag &= ~CRTSCTS; // Désactive le contrôle de flux matériel
#endif
    tty.c_cflag |= CREAD | CLOCAL; // Active la lecture et désactive le contrôle modem
    tty.c_lflag &= ~(ICANON | ECHO | ECHOE | ISIG); // Mode non canonique
    tty.c_iflag &= ~(IXON | IXOFF | IXANY);         // Désactive le contrôle de flux logiciel
    tty.c_oflag &= ~OPOST;                          // Pas de traitement de sortie

    tcsetattr(serial_port, TCSANOW, &tty); // Applique les paramètres
    return serial_port;
}

// Fonction pour lire les données envoyées par l'Arduino
// Retourne 0 si les données sont valides, -1 en cas d'erreur de format, 1 si aucune donnée
int lireDonneesArduino(int serial_port, float *temperature, float *humidite) {
    char buffer[128];
    memset(buffer, 0, sizeof(buffer));

    int index = 0;
    char c;

    // Lecture caractère par caractère jusqu'à '\n'
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
        return 1; // Pas de données reçues
    }

    printf("Reçu : %s\n", buffer);

    // Extraction des données (température et humidité)
    if (sscanf(buffer, "%f;%f", temperature, humidite) != 2) {
        fprintf(stderr, "Format invalide reçu : %s\n", buffer);
        return -1;
    }

    return 0; // Données valides
}

// Fonction pour lire les données depuis l'Arduino et les formater
// En cas de succès, les données formatées sont stockées dans formatted_data
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
    } else {
        formatted_data[0] = '\0'; // Chaîne vide en cas d'erreur
    }
}

// Fonction principale
int main() {
    int sockfd;
    struct sockaddr_in server_addr;
    char buffer[1024];
    const char *port = "/dev/ttyACM0";

    // Création de la socket
    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("Échec de la création de la socket");
        exit(EXIT_FAILURE);
    }

    // Désactiver le Nagle's Algorithm pour envoyer les données immédiatement
    int flag = 1;
    if (setsockopt(sockfd, IPPROTO_TCP, TCP_NODELAY, (void *)&flag, sizeof(flag)) < 0) {
        perror("Erreur lors de la désactivation de Nagle's Algorithm");
        close(sockfd);
        exit(EXIT_FAILURE);
    }

    // Configuration de l'adresse du serveur
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    server_addr.sin_addr.s_addr = inet_addr(SERVER_IP);

    // Connexion au serveur
    if (connect(sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("Échec de la connexion au serveur");
        close(sockfd);
        exit(EXIT_FAILURE);
    }

    printf("Connexion au serveur réussie.\n");

    // Ouverture du port série
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
            chiffrer_cesar(buffer);
            printf("Message à envoyer : %s\n", buffer);

            if (send(sockfd, buffer, strlen(buffer), 0) < 0) {
                perror("Échec de l'envoi du message");

                // Réessayer plusieurs fois avant de tenter une reconnexion
                int retry_count = 0;
                while (retry_count < 3) {
                    printf("Nouvelle tentative d'envoi (%d/3)...\n", retry_count + 1);
                    if (send(sockfd, buffer, strlen(buffer), 0) >= 0) {
                        printf("Message envoyé après tentative.\n");
                        break;
                    }
                    retry_count++;
                    sleep(1); // Attendre avant de réessayer
                }

                // Si toutes les tentatives échouent, tenter une reconnexion
                if (retry_count == 3) {
                    printf("Tentative de reconnexion au serveur...\n");
                    close(sockfd);

                    while (1) {
                        if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
                            perror("Échec de la recréation de la socket");
                            sleep(1);
                            continue;
                        }

                        if (connect(sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
                            perror("Échec de la reconnexion au serveur");
                            close(sockfd);
                            sleep(1);
                            continue;
                        }

                        printf("Reconnexion au serveur réussie.\n");
                        break;
                    }
                }
            } else {
                printf("Message envoyé : %s\n", buffer);
            }
        } else {
            printf("Aucune donnée valide reçue du port série.\n");
        }

        // Vérification du port série
        if (serial_port < 0) {
            perror("Port série invalide, tentative de réouverture...");
            serial_port = ouvrirPortSerie(port);
            if (serial_port < 0) {
                printf("Impossible de rouvrir le port série. Arrêt du programme.\n");
                break;
            }
        }

        printf("\n -------------------------------------- \n");
        sleep(2);
    }

    printf("Sortie de la boucle principale.\n");

    // Fermeture des connexions
    close(serial_port);
    close(sockfd);
    return 0;
}
