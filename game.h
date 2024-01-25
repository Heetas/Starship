// game.h
#include "lib.h"
#include "network.h"
#ifndef STARSHIP_GAME_H
#define STARSHIP_GAME_H

// Funzione per la stampa della griglia
void printGrid(int ship_x, int ship_y, int debris_x[], int debris_y[], int n) {
    for (int i = 0; i < GRID; i++) {
        for (int j = 0; j < GRID; j++) {
            if (j == ship_x && i == ship_y) {
                printf("A ");
            } else {
                int is_asteroid = 0;
                for (int k = 0; k < n; k++) {
                    if (j == debris_x[k] && i == debris_y[k]) {
                        is_asteroid = 1;
                        break;
                    }
                }
                if (is_asteroid) {
                    printf("X ");
                } else {
                    printf(". ");
                }
            }
        }
        printf("\n");
    }
}

// Funzione eseguita dal thread di input dell'utente
void *MuoviNavicella(void *arg) {
    thread_data_t *navicella = (thread_data_t *) arg;

    while (1) {
        char input;
        scanf(" %c", &input);
        switch (input) {
            case 'w':
            case 'W':
                if (navicella->ship_y > 0)
                    navicella->ship_y--; // Muovi su
                break;
            case 'a':
            case 'A':
                if (navicella->ship_x > 0)
                    navicella->ship_x--; // Muovi a sinistra
                break;
            case 's':
            case 'S':
                if (navicella->ship_y < GRID - 1)
                    navicella->ship_y++; // Muovi giù
                break;
            case 'd':
            case 'D':
                if (navicella->ship_x < GRID - 1)
                    navicella->ship_x++; // Muovi a destra
                break;
            default:
                printf("INPUT INVALIDO\n");
                break;
        }
    }
}

// Funzione eseguita dal thread di ricezione dei dati
void *RicezioneAsteroidi(void *arg) {
    thread_data_t *dati = (thread_data_t *) arg;
    char buffer[MAX_LINE + 1];
    int collision_detected;

    while (1) {
        for (int i = 0; i < MAX_ASTEROIDS; i++) {
            ssize_t n = Ricevi(dati->sockfd, buffer, &(dati->server_addr));
            if (n > 0) {
                buffer[n] = 0;
                char *token;
                token = strtok(buffer, " ");
                dati->debris_x[i] = atoi(token);
                token = strtok(NULL, " ");
                dati->debris_y[i] = atoi(token);
            }
        }

        clear();
        printf("TURNO N.%d\nEVITA GLI ASTEROIDI CON W-A-S-D!\n", turn);
        printGrid(dati->ship_x, dati->ship_y, dati->debris_x, dati->debris_y, MAX_ASTEROIDS);

        for (int i = 0; i < MAX_ASTEROIDS; i++) {
            printf("COORDINATE ASTEROIDI: [%d : %d]\n", dati->debris_x[i], dati->debris_y[i]);

            // Se è stato precedentemente rilevato una collisione e la navicella non si è mossa, allora è game over
            if (collision_detected && dati->debris_x[i] == dati->ship_x && dati->debris_y[i] == dati->ship_y) {
                printf("!!! GAME OVER - ASTEROID HIT !!!\n");
                exit(1);
            }

            // Controlliamo per un possibile impatto al prossimo ciclo
            if (dati->debris_x[i] == dati->ship_x && dati->debris_y[i] == dati->ship_y) {
                printf("!!! ALERT ASTEROID AT YOUR LOCATION - MOVE NOW !!!\n");
                collision_detected = 1;
            } else {
                collision_detected = 0;
            }
        }
        turn++;
        printf("\n");
        sleep(TIMER);
    }
}

#endif //STARSHIP_GAME_H
