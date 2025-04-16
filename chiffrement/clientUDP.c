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
    struct sockaddr_in adresse_serveur;
    char message_envoye[TAILLE_MESSAGE];
    char message_recu[TAILLE_MESSAGE];
    socklen_t taille_adresse = sizeof(struct sockaddr_in);

    // Création du socket UDP
    socket_desc = socket(AF_INET, SOCK_DGRAM, 0);
    if (socket_desc < 0) {
        perror("Erreur lors de la création du socket");
        exit(EXIT_FAILURE);
    }

    // Configuration de l'adresse du serveur
    adresse_serveur.sin_family = AF_INET;
    adresse_serveur.sin_port = htons(1618); // Port du serveur
    adresse_serveur.sin_addr.s_addr = inet_addr("127.0.0.1"); // Adresse IP du serveur

    while (1) {
        // Envoi du message au serveur
        printf("Entrez votre message : ");
        fgets(message_envoye, TAILLE_MESSAGE, stdin);
        message_envoye[strcspn(message_envoye, "\n")] = '\0'; // Suppression du saut de ligne
        chiffrer_cesar(message_envoye);
        sendto(socket_desc, message_envoye, strlen(message_envoye), 0,
               (struct sockaddr *)&adresse_serveur, taille_adresse);

        // Réception de la réponse du serveur
        ssize_t longueur = recvfrom(socket_desc, message_recu, TAILLE_MESSAGE - 1, 0,
                                    NULL, NULL);
        if (longueur > 0) {
            message_recu[longueur] = '\0';
            dechiffrer_cesar(message_recu);
            printf("Réponse du serveur : %s\n", message_recu);
        }
    }

    close(socket_desc);
    return 0;
}
