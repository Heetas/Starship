// Client.c
#include "lib.h"
#include "network.h"
#include "game.h"

int main(int argc, char **argv) {

    int sockfd = Socket();
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);

    IP_Conversion(argv[1], &server_addr);

    const char *msg = "Connessione al client avvenuta con successo.\n";
    Spedisci(sockfd, msg, server_addr);

    ssize_t outcome = Ricevi(sockfd, buffer, &server_addr);
    if (outcome > 0) {

        pthread_t move_thread, update_thread, collision_thread;
        game_info *navicella = malloc(sizeof(game_info));
        *navicella = (game_info) {sockfd, server_addr, ship_x, ship_y};

        pthread_create(&move_thread, NULL, MuoviNavicella, navicella);
        pthread_create(&update_thread, NULL, UpdateGrid, navicella);
        pthread_create(&collision_thread, NULL, CheckCollisione, navicella);

        pthread_join(move_thread, NULL);
        pthread_join(update_thread, NULL);
        pthread_join(collision_thread, NULL);

        free(navicella);
    }
    return 0;
}