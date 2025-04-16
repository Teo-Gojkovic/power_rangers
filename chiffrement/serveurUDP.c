#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <arpa/inet.h>

#define TAILLE_MESSAGE 1024
#define DECALAGE_CESAR 3

// Fonction pour chiffrer un message avec le chiffrement de César
void chiffrer_cesar(char *message) {
    for (int i = 0; message[i] != '\0'; i++) {
        if (message[i] >= 32 && message[i] <= 126) {
            message[i] = ((message[i] - 32 + DECALAGE_CESAR) % 95) + 32;
        }
    }
}

// Fonction pour déchiffrer un message avec le chiffrement de César
void dechiffrer_cesar(char *message) {
    for (int i = 0; message[i] != '\0'; i++) {
        if (message[i] >= 32 && message[i] <= 126) {
            message[i] = ((message[i] - 32 - DECALAGE_CESAR + 95) % 95) + 32;
        }
    }
}

int main() {
    int socket_desc;
    struct sockaddr_in adresse_serveur, adresse_client;
    char message_recu[TAILLE_MESSAGE];
    char message_envoye[TAILLE_MESSAGE];
    socklen_t taille_adresse = sizeof(struct sockaddr_in);

    // Création du socket UDP
    socket_desc = socket(AF_INET, SOCK_DGRAM, 0);
    if (socket_desc < 0) {
        perror("Erreur lors de la création du socket");
        exit(EXIT_FAILURE);
    }

    // Configuration de l'adresse du serveur
    adresse_serveur.sin_family = AF_INET;
    adresse_serveur.sin_addr.s_addr = INADDR_ANY;
    adresse_serveur.sin_port = htons(1618); // Port du serveur

    // Liaison du socket à l'adresse du serveur
    if (bind(socket_desc, (struct sockaddr *)&adresse_serveur, sizeof(adresse_serveur)) < 0) {
        perror("Erreur lors du bind");
        close(socket_desc);
        exit(EXIT_FAILURE);
    }

    printf("Serveur en attente de messages...\n");

    while (1) {
        // Réception du message du client
        ssize_t longueur = recvfrom(socket_desc, message_recu, TAILLE_MESSAGE - 1, 0,
                                    (struct sockaddr *)&adresse_client, &taille_adresse);
        if (longueur > 0) {
            message_recu[longueur] = '\0';
            dechiffrer_cesar(message_recu);
            printf("Message reçu : %s\n", message_recu);

            // Réponse au client
            printf("Entrez votre réponse : ");
            fgets(message_envoye, TAILLE_MESSAGE, stdin);
            message_envoye[strcspn(message_envoye, "\n")] = '\0'; // Suppression du saut de ligne
            chiffrer_cesar(message_envoye);
            sendto(socket_desc, message_envoye, strlen(message_envoye), 0,
                   (struct sockaddr *)&adresse_client, taille_adresse);
        }
    }

    close(socket_desc);
    return 0;
}
