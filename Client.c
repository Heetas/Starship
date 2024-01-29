// Client.c

#include "lib.h"
#include "network.h"
#include "game.h"
int main(int argc, char **argv) {

    // Creazione del socket.
    int sockfd = Socket();

    // Imposta famiglia e porta dell'indirizzo del server.
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);

    // Si converte l'indirizzo IP passato a linea di comando.
    IP_Conversion(argv[1], &server_addr);

    // Invia un messaggio al server e ne verifica il successo.
    if (Spedisci(sockfd, "ping", server_addr)) {

        // Inizia il loop infinito nel quale avviene il gioco.
        for (;;) {

            // Dichiarazione dei thread.
            pthread_t move_thread, update_thread, collision_thread, increment_thread;

            // Assegna memoria per i dati del gioco.
            game_info *navicella = malloc(sizeof(game_info));

            // Inizializza i dati del gioco.
            *navicella = (game_info) {sockfd, server_addr, ship_x, ship_y};

            // Crea i thread.
            pthread_create(&move_thread, NULL, MuoviNavicella, navicella);
            pthread_create(&update_thread, NULL, GameUpdate, navicella);
            pthread_create(&collision_thread, NULL, CheckCollisione, navicella);
            pthread_create(&increment_thread, NULL, IncrementaDati, navicella);

            // Attendi il completamento dei thread.
            pthread_join(move_thread, NULL);
            pthread_join(update_thread, NULL);
            pthread_join(collision_thread, NULL);
            pthread_join(increment_thread, NULL);
        }
    }
}