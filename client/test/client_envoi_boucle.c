#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <netinet/tcp.h> // Pour TCP_NODELAY
#include <time.h>        // Pour time()

// Définition des constantes
#define PORT 1618        // Numéro de port utilisé pour la communication
#define SERVER_IP "127.0.0.1" // Adresse IP du serveur (localhost)

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

    // Boucle principale pour envoyer des trames
    while (1) {
        // Exemple de données à envoyer
        snprintf(buffer, sizeof(buffer), "Trame exemple : %ld", (long int)time(NULL));
        printf("Message à envoyer : %s\n", buffer);

        if (send(sockfd, buffer, strlen(buffer), 0) < 0) {
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
            printf("Message envoyé : %s\n", buffer);
        }

        printf("\n -------------------------------------- \n");
        sleep(2); // Délai entre chaque envoi
    }

    printf("Sortie de la boucle principale.\n");

    // Fermeture de la connexion
    close(sockfd);
    return 0;
}
