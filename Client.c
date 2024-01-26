// Client.c
#include "lib.h"
#include "network.h"
#include "game.h"
int main(int argc, char **argv) {

    struct sockaddr_in server_addr;
    char buffer[MAX_LINE + 1];
    int ship_x = 4;
    int ship_y = 4;

    int sockfd = Socket();

    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);

    IP_Conversion(argv[1], &server_addr);

    const char *msg = "Connessione al client avvenuta con successo.\n";
    Spedisci(sockfd, msg, server_addr);

    ssize_t outcome = Ricevi(sockfd, buffer, &server_addr);
    if (outcome > 0) {

        pthread_t input_tid, receive_tid;
        grid_info thread_data = {sockfd, server_addr, ship_x, ship_y, {0}, {0}};
        pthread_create(&input_tid, NULL, MuoviNavicella, &thread_data);
        pthread_create(&receive_tid, NULL, RicezioneAsteroidi, &thread_data);

        pthread_join(input_tid, NULL);
        pthread_join(receive_tid, NULL);
    }
}