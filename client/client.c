#include <stdio.h> // Pour printf
#include <stdlib.h> // Pour exit et rand
#include <string.h> // Pour memset
#include <time.h> // Pour srand et rand
#include <arpa/inet.h> // Pour inet_addr et les structures réseau
#include <unistd.h> // Pour close

// Définition des constantes
#define PORT 1618 // Numéro de port utilisé pour la communication
#define NUM_COUNT 8 // Nombre de nombres aléatoires à générer
#define MAX_VALUE 2023 // Valeur maximale pour les nombres aléatoires
//#define SERVER_IP "172.16.20.11" // Adresse IP du serveur (exemple pour un autre PC)
#define SERVER_IP "127.0.0.1" // Adresse IP du serveur (localhost)

int main() {
    int sockfd; // Descripteur de socket
    struct sockaddr_in server_addr; // Structure pour l'adresse du serveur
    int numbers[NUM_COUNT]; // Tableau pour stocker les nombres aléatoires
    char buffer[1024]; // Buffer pour envoyer les données au serveur

    // Création de la socket
    if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        perror("Socket creation failed"); // Afficher une erreur si la création échoue
        exit(EXIT_FAILURE); // Quitter le programme en cas d'échec
    }

    // Configuration de l'adresse du serveur
    memset(&server_addr, 0, sizeof(server_addr)); // Initialiser la structure à zéro
    server_addr.sin_family = AF_INET; // Utiliser IPv4
    server_addr.sin_port = htons(PORT); // Convertir le port en format réseau
    server_addr.sin_addr.s_addr = inet_addr(SERVER_IP); // Convertir l'adresse IP en format réseau

    // Génération de nombres aléatoires
    srand(time(NULL)); // Initialiser le générateur de nombres aléatoires avec l'heure actuelle
    for (int i = 0; i < NUM_COUNT; i++) {
        numbers[i] = rand() % (MAX_VALUE + 1); // Générer un nombre aléatoire entre 0 et MAX_VALUE
    }

    // Préparer le message à envoyer
    snprintf(buffer, sizeof(buffer), "%d,%d,%d,%d,%d,%d,%d,%d",
             numbers[0], numbers[1], numbers[2], numbers[3],
             numbers[4], numbers[5], numbers[6], numbers[7]); // Formater les nombres en une chaîne séparée par des virgules

    printf("Sending to IP: %s, Port: %d\n", SERVER_IP, PORT); // Afficher l'adresse IP et le port du serveur

    // Envoyer le message au serveur
    if (sendto(sockfd, buffer, strlen(buffer), 0, (const struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("Send failed"); // Afficher une erreur si l'envoi échoue
        close(sockfd); // Fermer la socket
        exit(EXIT_FAILURE); // Quitter le programme en cas d'échec
    }

    printf("IP tram sent: %s\n", buffer); // Afficher le message envoyé

    // Fermer la socket
    close(sockfd); // Libérer les ressources associées à la socket
    return 0; // Terminer le programme avec succès
}
