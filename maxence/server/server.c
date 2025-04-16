#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include <unistd.h>

// Définition des constantes
#define DECALAGE_CESAR 5 // Décalage pour le chiffrement de César
#define LENGTH 28  // Taille d'un buffer pour fgets (non utilisé ici)
#define PORT 1618  // Port sur lequel le serveur écoute
#define BUFFER_SIZE 1024  // Taille maximale du buffer pour recevoir des données

// Fonction pour déchiffrer un message avec le chiffrement de César
void dechiffrer_cesar(char *buffer) {
    for (int i = 0; buffer[i] != '\0'; i++) {
        if (buffer[i] >= 32 && buffer[i] <= 126) {
            buffer[i] = ((buffer[i] - 32 - DECALAGE_CESAR + 95) % 95) + 32;
        }
    }
}

// Fonction pour convertir une chaîne de caractères en tableau d'entiers
void convertBufferToIntArray(char *buffer, int *tableau, int *size) {
    int number = 0, index = 0, i = 0;

    while (buffer[index] != '\0') {
        if (buffer[index] >= '0' && buffer[index] <= '9') { // Si le caractère est un chiffre
            number = number * 10 + (buffer[index] - '0'); // Construire le nombre
        } else if (buffer[index] == ',') { // Si c'est une virgule
            tableau[i++] = number; // Ajouter le nombre au tableau
            number = 0; // Réinitialiser le nombre
        }
        index++; // Passer au caractère suivant
    }
    tableau[i++] = number; // Ajouter le dernier nombre
    *size = i; // Mettre à jour la taille du tableau
}

int main() {
    int sockfd; // Descripteur de socket
    struct sockaddr_in server_addr, client_addr; // Structures pour les adresses du serveur et du client
    char buffer[BUFFER_SIZE]; // Buffer pour recevoir les données
    socklen_t addr_len = sizeof(client_addr); // Taille de l'adresse du client

    // Création de la socket
    if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        perror("Socket creation failed"); // Afficher une erreur si la création échoue
        exit(EXIT_FAILURE);
    }

    // Configuration de l'adresse du serveur
    memset(&server_addr, 0, sizeof(server_addr)); // Initialiser la structure à zéro
    server_addr.sin_family = AF_INET; // Utiliser IPv4
    server_addr.sin_port = htons(PORT); // Convertir le port en format réseau
    server_addr.sin_addr.s_addr = INADDR_ANY; // Accepter les connexions de n'importe quelle adresse

    // Lier la socket au port
    if (bind(sockfd, (const struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("Bind failed"); // Afficher une erreur si le bind échoue
        close(sockfd); // Fermer la socket
        exit(EXIT_FAILURE);
    }

    printf("Server is listening on port %d...\n", PORT);
    printf("Listening on IP: 0.0.0.0, Port: %d\n", PORT);

    // Boucle pour écouter en continu les trames entrantes
    while (1) {
        int n = recvfrom(sockfd, buffer, BUFFER_SIZE, 0, (struct sockaddr *)&client_addr, &addr_len);
        if (n < 0) {
            perror("Receive failed"); // Afficher une erreur si la réception échoue
            continue; // Continuer à écouter même en cas d'erreur
        }

        buffer[n] = '\0'; // Terminer la chaîne reçue avec un caractère nul
        dechiffrer_cesar(buffer); // Déchiffrer le message reçu
        printf("Received IP tram: %s\n", buffer);
        /*
        // Convertir le buffer en tableau d'entiers
        int tableau[8]; // Tableau de taille fixe pour stocker les entiers
        int size = 0; // Taille réelle des données dans le tableau
        convertBufferToIntArray(buffer, tableau, &size);

        // Appeler la fonction de traitement avec le tableau d'entiers
        treatment(tableau, size); // Passer la taille réelle du tableau
        */
    }

    // Fermer la socket (ce code est inatteignable dans ce cas)
    close(sockfd);
    return 0;
}