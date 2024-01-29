// game.h

#include "lib.h"
#include "network.h"
#ifndef STARSHIP_GAME_H
#define STARSHIP_GAME_H

// Funzione utilizzata per stampare la griglia di gioco, tra cui la posizione della nave, i detriti,
// il numero di turni e il punteggio attuale, quest'ultimi ottenuti dall'aggiornamento della funzione *IncrementaDati.
void printGrid(int nave_x, int nave_y, int debris_x[], int debris_y[], int n, int turno, int punteggio, int asteroidi) {

    // Pulizia della console precedente e annuncio di gioco
    clear();
    printf("BENVENUTO NEL GIOCO STARSHIP!\nEVITA GLI ASTEROIDI CON W-A-S-D.\nTURNO: %d   -   SCORE: %d\nNUMERO DI ASTEROIDI CORRENTI: %d\n", turno,
           punteggio, asteroidi);
    fflush(stdout);

    // Ciclo per la stampa della griglia.
    // Se nella casella vi è la navicella, viene printato "A "
    // Se nella casella vi è un asteroide, viene printato "X "
    // Se nella casella non c'è niente, viene printato ". "
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
    fflush(stdout);
}

// Funzione eseguita perpetuamente in un thread separato.
// Nel caso in cui ci sia una collisione, la funzione salva la posizione coincidente tra navicella e detrito,
// per poi stampare un avviso sullo schermo.
void *CheckCollisione(void *arg) {
    game_info *navicella = (game_info *) arg;
    for (;;) {
        for (int i = 0; i < asteroids; i++) {

            // Se la posizione della navicella coincide con quella di un detrito, salvo le coordinate,
            // effettuo un PrintGrid per pulire e riscrivere la griglia ed annetto il messaggio d'errore.
            // L'utilizzo del PrintGrid qui è per fare in modo di impedire lo spam di messaggi di allerta tramite la pulizia della griglia.
            if (navicella->ship_x == navicella->debris_x[i] && navicella->ship_y == navicella->debris_y[i]) {
                navicella->prev_ship_x = navicella->debris_x[i];
                navicella->prev_ship_y = navicella->debris_y[i];
                printGrid(navicella->ship_x, navicella->ship_y, navicella->debris_x, navicella->debris_y, asteroids,
                          turn, score, asteroids);
                printf("\n!!! ALLERTA: ASTEROIDE NELLE TUE COORDINATE !!!\n");
                fflush(stdout);
            }
        }
        // Cooldown di rilevamento di collisione di 2s
        usleep(200000);
    }
}

// Funzione eseguita perpetuamente in un thread separato.
// Responsabile di ricevere l'input dell'utente per muovere la navicella ed aggiornare la griglia con la nuova posizione.
void *MuoviNavicella(void *arg) {
    game_info *navicella = (game_info *) arg;
    char input;
    for (;;) {
        scanf(" %c", &input);
        switch (input) {

            case 'w':
            case 'W':
                // Sposta verso l'alto
                if (navicella->ship_y > 0)
                    navicella->ship_y--;
                break;

            case 'a':
            case 'A':
                // Sposta verso sinistra
                if (navicella->ship_x > 0)
                    navicella->ship_x--;
                break;

            case 's':
            case 'S':
                // Sposta verso il basso
                if (navicella->ship_y < GRID - 1)
                    navicella->ship_y++;
                break;

            case 'd':
            case 'D':
                // Sposta verso destra
                if (navicella->ship_x < GRID - 1)
                    navicella->ship_x++;
                break;
            default:
                break;
        }

        // Per effettuare un aggiornamento corretto e non invasivo della griglia utilizzo un cooldown di 1s.
        usleep(100000);
        printGrid(navicella->ship_x, navicella->ship_y, navicella->debris_x, navicella->debris_y, asteroids, turn,
                  score, asteroids);
    }
}

// Funzione eseguita all'interno della funzione GameUpdate.
// Ha lo scopo di ricevere i dati degli asteroidi dal server.
void RicezioneAsteroidi(void *arg) {
    game_info *navicella = (game_info *) arg;

    for (int i = 0; i < asteroids; i++) {
        ssize_t n = Ricevi(navicella->sockfd, &(navicella->server_addr));
        if (n > 0) {

            // Utilizzo il sistema token di strtok per ottenere in modo preciso e completo
            // i dati mandati dal server in formato "%d %d".
            buffer[n] = 0;
            char *token;
            token = strtok(buffer, " ");
            navicella->debris_x[i] = atoi(token);
            token = strtok(NULL, " ");
            navicella->debris_y[i] = atoi(token);
        }
    }
}

// Funzione eseguita all'interno della funzione GameUpdate.
// Il suo scopo è impedire al giocatore di continuare al giocare se questo
// termina un turno mentre la navicella condivide le coordinate con un detrito.
// Al suo richiamo pulisce la console, printa un messaggio di Game Over e restituisce
// il turno ed il punteggio raggiunto.
void GameOver() {
    clear();
    printf("\n\n!!! GAME OVER - COLLISIONE CON ASTEROIDE !!!\n\nRIPROVA!\nTURNO: %d   -   PUNTEGGIO: %d\n\n", turn,
           score);
    exit(0);
}

// Funzione eseguita perpetuamente in un thread separato.
// Consiste nell'aggiornamento dei valori del turno e del punteggio del giocatore e il numero di asteroidi correnti.
// Per farlo si è sincronizzata al TIMER utilizzato dagli altri componenti del gioco.
void *IncrementaDati(void *arg) {
    for (;;) {
        sleep(TIMER);
        turn++;
        score += score_increment;
        score_increment += 10000;

        // Incrementa il numero di asteroidi per ogni turno fino a un massimo di MAX_ASTEROIDS (50).
        if (asteroids < MAX_ASTEROIDS) {
            asteroids++;
        }
    }
}

// Funzione eseguita perpetuamente in un thread separato.
// Si occupa di organizzare e far convergere tutto il sistema di gioco.
void *GameUpdate(void *arg) {
    game_info *navicella = (game_info *) arg;
    for (;;) {

        // Riceve gli asteroidi dal server.
        RicezioneAsteroidi(arg);

        // Aggiorna la griglia con un nuovo turno di asteroidi.
        printGrid(navicella->ship_x, navicella->ship_y, navicella->debris_x, navicella->debris_y, asteroids, turn,
                  score, asteroids);

        // Effettua una check alla fine del turno per verificare se la navicella si trova su un asteroide.
        // Se non è il caso, resetta i valori di collisione della navicella.
        if (navicella->ship_x == navicella->prev_ship_x && navicella->ship_y == navicella->prev_ship_y)
            GameOver();
        else {
            navicella->prev_ship_x = -1;
            navicella->prev_ship_y = -1;
        }
    }
}

#endif //STARSHIP_GAME_H