// Server.c

#include "lib.h"
#include "network.h"
#include "game.h"
int main(int argc, char **argv) {

    // Buffer per i dati degli asteroidi.
    char debris_data[MAX_LINE];

    // Inizializza la griglia del gioco.
    int grid[GRID][GRID] = {0};

    // Inizializza il generatore di numeri casuali.
    srand(time(NULL));

    // Crea il socket.
    int sockfd = Socket();

    // Imposta famiglia, indirizzo e porta del server.
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    server_addr.sin_port = htons(PORT);

    // Associa il socket a un indirizzo e a una porta.
    Bind(sockfd, server_addr);

    // Stampa un messaggio per indicare che il server è in attesa di una connessione.
    printf("Server in attesa...\n\n");

    // Controlla se il server riceve una richiesta dal client.
    if (Ricevi(sockfd, &client_addr)) {

        // Crea un loop infinito nel quale verranno spediti i dati degli asteroidi.
        for (;;) {

            // Genera le coordinate di detriti casuali e le invia al client.
            for (int i = 0; i < asteroids; i++) {
                int debris_x, debris_y;

                // Genera coordinate casuali che non sono ancora occupate nella griglia.
                do {
                    debris_x = rand() % GRID;
                    debris_y = rand() % GRID;
                } while (grid[debris_x][debris_y]);

                // Occupa le coordinate della griglia impostandole ad 1.
                grid[debris_x][debris_y] = 1;

                // Carica i dati degli asteroidi.
                printf("Cordinate asteroide spedite: [%d : %d]\n", debris_x, debris_y);
                sprintf(debris_data, "%d %d", debris_x, debris_y);

                // Spedisci le nuove coordinate dei detriti al client.
                Spedisci(sockfd, debris_data, client_addr);
            }

            // Reset della griglia di gioco.
            for (int i = 0; i < GRID; i++) {
                for (int j = 0; j < GRID; j++) {
                    grid[i][j] = 0;
                }
            }

            // Incrementa il numero di asteroidi per ogni turno fino a un massimo di MAX_ASTEROIDS (50).
            if (asteroids < MAX_ASTEROIDS) {
                asteroids++;
            }

            // Fermo il timer di gioco per 2s, equivalente quindi alla durata di un turno.
            sleep(TIMER);
            clear();
        }
    }
}