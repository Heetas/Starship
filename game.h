// game.h
#include "lib.h"
#include "network.h"

#ifndef STARSHIP_GAME_H
#define STARSHIP_GAME_H

// Funzione per la stampa della griglia
void printGrid(int nave_x, int nave_y, int debris_x[], int debris_y[], int n, int turno, int punteggio) {
    clear();
    printf("BENVENUTO NEL GIOCO STARSHIP!\nEVITA GLI ASTEROIDI CON W-A-S-D.\nTURNO: %d   -   SCORE: %d\n", turno,
           punteggio);
    for (int i = 0; i < GRID; i++) {
        for (int j = 0; j < GRID; j++) {
            if (j == nave_x && i == nave_y) {
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

void *CheckCollisione(void *arg) {
    game_info *navicella = (game_info *) arg;
    while (1) {
        for (int i = 0; i < MAX_ASTEROIDS; i++) {
            if (navicella->ship_x == navicella->debris_x[i] && navicella->ship_y == navicella->debris_y[i]) {

                navicella->prev_ship_x = navicella->debris_x[i];
                navicella->prev_ship_y = navicella->debris_y[i];
                printf("\n!!! ALLERTA: ASTEROIDE NELLE TUE COORDINATE !!!");

            } else {
                navicella->prev_ship_x = -1;
                navicella->prev_ship_y = -1;
            }
        }
        usleep(100000);
    }
}

// Funzione eseguita dal thread di input dell'utente
void *MuoviNavicella(void *arg) {
    game_info *navicella = (game_info *) arg;
    char input;
    while (!navicella->game_over) {
        if (scanf(" %c", &input) == 1) {
            if (input == 'w' || input == 'W' || input == 'a' || input == 'A' ||
                input == 's' || input == 'S' || input == 'd' || input == 'D') {
                navicella->direction = input;
            }
        }
        switch (navicella->direction) {
            case 'w':
            case 'W':
                if (navicella->ship_y > 0)
                    navicella->ship_y--; // Move up
                break;
            case 'a':
            case 'A':
                if (navicella->ship_x > 0)
                    navicella->ship_x--; // Move left
                break;
            case 's':
            case 'S':
                if (navicella->ship_y < GRID - 1)
                    navicella->ship_y++; // Move down
                break;
            case 'd':
            case 'D':
                if (navicella->ship_x < GRID - 1)
                    navicella->ship_x++; // Move right
                break;
            default:
                break;
        }
        navicella->direction = '\0';
        usleep(100000);
    }
    return NULL;
}

// Funzione eseguita dal thread di ricezione dei dati
void RicezioneAsteroidi(void *arg) {
    game_info *navicella = (game_info *) arg;

    for (int i = 0; i < MAX_ASTEROIDS; i++) {
        ssize_t n = Ricevi(navicella->sockfd, buffer, &(navicella->server_addr));
        if (n > 0) {
            buffer[n] = 0;
            char *token;
            token = strtok(buffer, " ");
            navicella->debris_x[i] = atoi(token);
            token = strtok(NULL, " ");
            navicella->debris_y[i] = atoi(token);
        }
    }
}

void GameOver(game_info *navicella) {
    clear();
    navicella->game_over = 1;
    printf("!!! GAME OVER - COLLISIONE CON ASTEROIDE !!!\n\nRIPROVA!\nTURNO: %d   -   PUNTEGGIO: %d\n\n", turn, score);
    exit(0);
}

void *UpdateGrid(void *arg) {
    game_info *navicella = (game_info *) arg;
    while (!navicella->game_over) {
        RicezioneAsteroidi(arg);
        printGrid(navicella->ship_x, navicella->ship_y, navicella->debris_x, navicella->debris_y, MAX_ASTEROIDS, turn,
                  score);
        if (navicella->ship_x == navicella->prev_ship_x && navicella->ship_y == navicella->prev_ship_y)
            GameOver(navicella);
        turn++;

        score += score_increment;
        score_increment += 10000;
        sleep(TIMER);
    }
    return NULL;
}

#endif //STARSHIP_GAME_H