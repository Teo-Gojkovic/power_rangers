#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include <unistd.h>

// Définition des constantes
#define LENGTH 28  // Taille d'un buffer pour fgets (non utilisé ici)
#define PORT 1618  // Port sur lequel le serveur écoute
#define BUFFER_SIZE 1024  // Taille maximale du buffer pour recevoir des données

// Fonction pour trouver l'indice du minimum dans un tableau
int min(int *tableau, int size) {
    int index = 0;

    for (int i = 1; i < size; i++) {
        if (tableau[i] < tableau[index] && tableau[i] != -1) { // Ignorer les valeurs marquées comme -1
            index = i;
        }
    }

    return index; // Retourne l'indice de la plus petite valeur
}

// Fonction pour trouver l'indice du maximum dans un tableau
int max(int *tableau, int size) {
    int index = 0;

    for (int i = 1; i < size; i++) {
        if (tableau[i] > tableau[index]) { // Comparer les valeurs pour trouver le maximum
            index = i;
        }
    }

    return index; // Retourne l'indice de la plus grande valeur
}

// Fonction pour calculer la moyenne des éléments d'un tableau
int calcul_moyenne(int *tableau, int size) {
    int somme = 0;

    for (int i = 0; i < size; i++) {
        somme += tableau[i]; // Ajouter chaque élément à la somme
    }

    return somme / size; // Retourner la moyenne (division entière)
}

// Fonction pour isoler les nombres d'une chaîne de caractères et les stocker dans un tableau
void IsolatioNumber(char *chaine, int *tableau) {
    int number = 0, index = 0, i = 0;

    printf("Chaîne à traiter : ");
    for (int j = 0; chaine[j] != '\0'; j++) {
        printf("%c", chaine[j]); // Afficher chaque caractère de la chaîne
    }
    printf("\n");

    while (chaine[index] != '\0') {
        if (chaine[index] >= '0' && chaine[index] <= '9') { // Si le caractère est un chiffre
            number = number * 10 + (chaine[index] - '0'); // Construire le nombre
        } else if (chaine[index] == ',') { // Si c'est une virgule
            tableau[i++] = number; // Ajouter le nombre au tableau
            number = 0; // Réinitialiser le nombre
        }
        index++; // Passer au caractère suivant
    }

    tableau[i] = number; // Ajouter le dernier nombre après la dernière virgule

    // Afficher les valeurs extraites pour débogage
    printf("Valeurs extraites : ");
    for (int j = 0; j <= i; j++) {
        printf("%d ", tableau[j]);
    }
    printf("\n");
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

// Fonction pour traiter un tableau d'entiers
void treatment(int *tableau, int size) {
    // Afficher le tableau initial
    printf("Tableau initial : ");
    for (int i = 0; i < size; i++) {
        printf("%d ", tableau[i]);
    }
    printf("\n");

    // Calculer et afficher la moyenne de tous les éléments
    printf("Moyenne : %d\n", calcul_moyenne(tableau, size));

    // Trouver et afficher la valeur minimale
    int minIndex = min(tableau, size);
    printf("Valeur minimale : %d (à l'indice %d)\n", tableau[minIndex], minIndex);

    // Trouver et afficher la valeur maximale
    int maxIndex = max(tableau, size);
    printf("Valeur maximale : %d (à l'indice %d)\n", tableau[maxIndex], maxIndex);
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
        printf("Received IP tram: %s\n", buffer);

        // Convertir le buffer en tableau d'entiers
        int tableau[8]; // Tableau de taille fixe pour stocker les entiers
        int size = 0; // Taille réelle des données dans le tableau
        convertBufferToIntArray(buffer, tableau, &size);

        // Appeler la fonction de traitement avec le tableau d'entiers
        treatment(tableau, size); // Passer la taille réelle du tableau
    }

    // Fermer la socket (ce code est inatteignable dans ce cas)
    close(sockfd);
    return 0;
}

