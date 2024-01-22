// Server.c
#include "header.h"

int main(int argc, char **argv) {

    // Dichiarazione delle strutture dati per gli indirizzi del server e del client
    struct sockaddr_in server_addr, client_addr;

    // Buffer per la ricezione e messaggio da inviare
    char buffer[MAX_LINE + 1];
    char msg[MAX_LINE];

    // Creazione del socket
    int sockfd = Socket();
    printf("Socket creato con successo.\n");

    // Modifica delle impostazioni del socket per riutilizzare l'indirizzo
    Reusaddr(sockfd);
    printf("Impostazioni socket modificate con successo.\n");

    // Inizializzazione della struttura dati per l'indirizzo del server
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    server_addr.sin_port = htons(PORT);

    // Associazione dell'indirizzo al socket
    Bind(sockfd, server_addr);
    printf("Bind alla porta avvenuto con successo.\n\n");

    // Inizializzazione del generatore di numeri casuali
    srand(time(NULL));

    // Ciclo infinito di generazione dei detriti e invio degli alert
    for (;;) {
        printf("Server in attesa...\n");

        // Ricezione dei dati dal client
        ssize_t n = Ricevi(sockfd, buffer, &client_addr);
        if (n > 0)
            printf("Ricezione avvenuta con successo:\n%zd", n);

        // Generazione casuale della posizione dei detriti
        int debris_x = rand() % GRID;
        int debris_y = rand() % GRID;

        // Creazione dell'alert con la posizione dei detriti
        sprintf(msg, "ASTEROIDE IN ARRIVO IN %dx %dy", debris_x, debris_y);

        // Invio dell'alert al client
        Spedisci(sockfd, msg, client_addr);

        // Attesa di 2 secondi prima di generare nuovi detriti
        sleep(2);
    }
}