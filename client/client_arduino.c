#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <netinet/tcp.h> // Pour TCP_NODELAY
#include <time.h>
#include <fcntl.h>
#include <termios.h>

// Définition des constantes
#define PORT 1618
#define SERVER_IP "127.0.0.1"
#define DECALAGE_CESAR 5

// Fonction pour chiffrer un message avec le chiffrement de César
void chiffrer_cesar(char *buffer) {
    for (int i = 0; buffer[i] != '\0'; i++) {
        if (buffer[i] >= 32 && buffer[i] <= 126) {
            buffer[i] = ((buffer[i] - 32 + DECALAGE_CESAR) % 95) + 32;
        }
    }
}

// Fonction pour ouvrir et configurer le port série
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
    tty.c_cflag |= CREAD | CLOCAL;

    tty.c_lflag &= ~(ICANON | ECHO | ECHOE | ISIG);
    tty.c_iflag &= ~(IXON | IXOFF | IXANY);
    tty.c_oflag &= ~OPOST;

    tcsetattr(serial_port, TCSANOW, &tty);
    return serial_port;
}

// Fonction pour lire les données envoyées par l'Arduino
int lireDepuisArduino(int serial_port, char *formatted_data) {
    char buffer[128] = {0};
    int index = 0;
    char c;

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
        return 1; // Pas encore de données complètes
    }

    printf("Reçu : %s\n", buffer);

    // Formater les données avec un timestamp
    time_t now = time(NULL);
    struct tm *t = localtime(&now);
    snprintf(formatted_data, 1024, "%02d-%02d-%04d,%02d-%02d-%02d,%s",
             t->tm_mday, t->tm_mon + 1, t->tm_year + 1900,
             t->tm_hour, t->tm_min, t->tm_sec,
             buffer);

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

// Fonction principale
int main() {
    int sockfd;
    struct sockaddr_in server_addr;
    char buffer[1024];
    const char *port = "/dev/ttyACM0";

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

    // Ouverture du port série
    int serial_port = ouvrirPortSerie(port);
    if (serial_port < 0) {
        close(sockfd);
        exit(EXIT_FAILURE);
    }

    printf("Port série ouvert avec succès.\n");

    // Boucle principale pour lire et envoyer les données
    while (1) {
        if (lireDepuisArduino(serial_port, buffer) == 0) {
            chiffrer_cesar(buffer);
            printf("Message à envoyer : %s\n", buffer);

            // Tentative d'envoi des données
            if (send(sockfd, buffer, strlen(buffer), 0) < 0) {
                perror("Échec de l'envoi du message");

                // Fermeture de la socket avant de tenter une reconnexion
                close(sockfd);

                // Tentative de reconnexion au serveur
                printf("Tentative de reconnexion au serveur...\n");
                int reconnexion_reussie = 0;
                for (int i = 0; i < 5; i++) { // Limite à 5 tentatives
                    sockfd = connecter_au_serveur(&server_addr);
                    if (sockfd >= 0) {
                        printf("Reconnexion réussie.\n");
                        reconnexion_reussie = 1;
                        break;
                    }
                    printf("Nouvelle tentative dans 2 secondes...\n");
                    sleep(2);
                }

                // Si la reconnexion échoue après 5 tentatives, quitter le programme
                if (!reconnexion_reussie) {
                    fprintf(stderr, "Impossible de se reconnecter au serveur après plusieurs tentatives. Arrêt du programme.\n");
                    close(serial_port);
                    exit(EXIT_FAILURE);
                }

                // Recommencer la boucle après une reconnexion réussie
                continue;
            }

            // Si l'envoi réussit
            printf("Message envoyé : %s\n", buffer);
        } else {
            printf("Aucune donnée valide reçue depuis l'Arduino.\n");
        }

        printf("\n--------------------------------------\n");
        sleep(2); // Délai entre chaque envoi
    }

    // Fermeture des connexions
    close(serial_port);
    close(sockfd);
    return 0;
}