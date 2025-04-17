#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include <unistd.h>

// Définition des constantes
#define DECALAGE_CESAR 5 // Décalage pour le chiffrement de César
#define PORT 1618        // Port sur lequel le serveur écoute
#define BUFFER_SIZE 1024 // Taille maximale du buffer pour recevoir des données

// Fonction pour déchiffrer un message avec le chiffrement de César
void dechiffrer_cesar(char *buffer) {
    for (int i = 0; buffer[i] != '\0'; i++) {
        if (buffer[i] >= 32 && buffer[i] <= 126) {
            buffer[i] = ((buffer[i] - 32 - DECALAGE_CESAR + 95) % 95) + 32;
        }
    }
}

int main() {
    int sockfd, new_socket;
    struct sockaddr_in server_addr, client_addr;
    char buffer[BUFFER_SIZE];
    socklen_t addr_len = sizeof(client_addr);

    // Création de la socket
    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("Échec de la création de la socket");
        exit(EXIT_FAILURE);
    }

    // Configuration de l'adresse du serveur
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    server_addr.sin_addr.s_addr = INADDR_ANY;

    // Lier la socket au port
    if (bind(sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("Échec du bind");
        close(sockfd);
        exit(EXIT_FAILURE);
    }

    // Mettre la socket en mode écoute
    if (listen(sockfd, 3) < 0) {
        perror("Échec de l'écoute");
        close(sockfd);
        exit(EXIT_FAILURE);
    }

    printf("Le serveur écoute sur le port %d...\n", PORT);

    // Boucle infinie pour accepter et traiter les connexions entrantes
    while (1) {
        // Accepter une connexion entrante
        new_socket = accept(sockfd, (struct sockaddr *)&client_addr, &addr_len);
        if (new_socket < 0) {
            perror("Échec de l'acceptation de la connexion");
            continue;
        }

        // Recevoir des données du client dans une boucle
        while (1) {
            int n = recv(new_socket, buffer, BUFFER_SIZE, 0);
            if (n <= 0) {
                if (n < 0) perror("Échec de la réception");
                break; // Sortir de la boucle si la connexion est fermée ou en cas d'erreur
            }

            buffer[n] = '\0'; // Terminer la chaîne reçue avec un caractère nul
            dechiffrer_cesar(buffer); // Déchiffrer le message reçu
            printf("Message reçu : %s\n", buffer);
        }

        // Fermer la socket pour cette connexion
        close(new_socket);
    }

    // Fermer la socket principale (jamais atteint ici à cause de la boucle infinie)
    close(sockfd);
    return 0;
}
