// Server.c
#include "lib.h"
#include "network.h"
#include "game.h"
int main(int argc, char **argv) {
    char msg[MAX_LINE];
    int sockfd = Socket();

    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    server_addr.sin_port = htons(PORT);

    Bind(sockfd, server_addr);
    srand(time(NULL));

    printf("Server in attesa...\n\n");
    ssize_t n = Ricevi(sockfd, buffer, &client_addr);
    if (n > 0) {
        const char *outcome = "Connessione al server stabilita con successo.\n";
        Spedisci(sockfd, outcome, client_addr);

        for (;;) {
            printf("Numero turno: %d\n", turn);
            for (int i = 0; i < MAX_ASTEROIDS; i++) {
                int debris_x = rand() % GRID;
                int debris_y = rand() % GRID;

                printf("Cordinate asteroide spedite: [%d : %d]\n", debris_x, debris_y);
                sprintf(msg, "%d %d", debris_x, debris_y);
                Spedisci(sockfd, msg, client_addr);
            }
            turn++;
            sleep(TIMER);
            clear();
        }
    }
}