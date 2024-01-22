// Client.c
#include "header.h"

int main(int argc, char **argv) {

    // Dichiarazione delle variabili del client
    int sockfd;
    struct sockaddr_in servaddr;
    char buffer[MAX_LINE + 1];

    // Posizione iniziale della navicella
    int ship_x = 0;
    int ship_y = 0;

    // Creazione del socket
    sockfd = Socket();
    printf("Socket creato con successo.\n");

    // Inizializzazione della struttura dati per l'indirizzo del server
    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(PORT);

    // Conversione dell'indirizzo IP passato come argomento
    IP_Conversion(argv[1], &servaddr);
    printf("IP convertito con successo.\n\n");

    // Ciclo di ricezione degli alert dal server
    for (;;) {
        printf("Client in attesa...\n");

        // Ricezione dell'alert dal server
        ssize_t n = Ricevi(sockfd, buffer, &servaddr);
        if (n > 0)
            printf("Ricezione avvenuta con successo:\n%zd", n);

        // Aggiunta del carattere di terminazione alla fine della stringa
        buffer[n] = 0;

        // Estrazione delle coordinate dei detriti dall'alert
        char *token = strtok(buffer, " ");
        token = strtok(NULL, " ");
        int debris_x = atoi(token);
        token = strtok(NULL, " ");
        int debris_y = atoi(token);

        // Controllo se la navicella è nella stessa posizione dei detriti
        if (ship_x == debris_x && ship_y == debris_y) {
            printf("Allerta! Detrito in arrivo. Spostamento in corso...\n");

            // Spostamento della navicella (esempio: incremento delle coordinate modulo GRID)
            ship_x = (ship_x + 1) % GRID;
            ship_y = (ship_y + 1) % GRID;
        }
    }
}