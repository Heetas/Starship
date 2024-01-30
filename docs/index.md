---
title: Starship
author: Lorenzo Mirabella
---

# Introduzione

Starship è un progetto universitario di Reti di Calcolatori che consiste in un videogioco in protocollo UDP.

## Traccia

Una navicella spaziale deve evitare i detriti provenienti da una tempesta di meteoriti.
La navicella (client) entra nel settore spaziale dei meteoriti (server) connettendosi in UDP. La tempesta di meteoriti genera in modo casuale n pacchetti UDP ogni 2 secondi che rappresentano i detriti in una griglia MxM. La navicella riceve un alert nel caso in cui il detrito spaziale si trovi sulla sua stessa posizione e può spostarsi prima dell'impatto.

## Logica

La logica nel progetto consiste in un sistema client/server in struttura UDP, ed è stato sviluppato in C.

Il server, appena riceve notifica della connessione del client, comincia a generare coordinate di asteroidi ogni 2 secondi e li spedisce al client, che ottiene le coordinate dal messaggio ricevuto e le traduce in coordinate all'interno della griglia di gioco.

Il giocatore può muoversi liberamente sulla griglia, ed il suo obiettivo è quello di evitare le coordinate nel quale vi sono gli asteroidi. Se il giocatore dovesse iniziare il turno successivo, cioè arriva un altro pacchetto di asteroidi, mentre è collocato nelle coordinate di un asteroide, allora sarà un Game Over.

Per gestire tutti questi processi contemporaneamente in C sono stati utilizzati 4 thread:
- move_thread: Thread che si è occupato di permettere al giocatore di muoversi a piacimento sulla griglia ed aggiornare le posizioni dopo ogni spostamento.

- update_thread: Thread impostato per aggiornare la griglia degli asteroidi, e controllare se il giocatore finisce il turno mentre è su un asteroide.

- collision_thread: Thread con il ruolo di verificare la posizione della navicella rispetto agli asteroidi presenti sulla griglia. Se la navicella è su un asteroide, il giocatore riceve il messaggio di allerta e vengono salvate le coordinate della collisione. Se il giocatore riesce a spostarsi in tempo, resetta le coordinate di collisione.

- increment_thread: Thread atto ad aggiornare i dati di gioco su tutto il sistema del client, come il turno ed il punteggio.

# Protocollo User Datagram (UDP)

Il protocollo User Datagram (UDP) è uno dei protocolli di comunicazione fondamentali della suite di protocolli Internet utilizzato per inviare messaggi (trasportati come datagrammi in pacchetti) ad altri host su una rete Internet Protocol (IP).

## Caratteristiche

- UDP non richiede una comunicazione preliminare per impostare canali di comunicazione o percorsi di dati.
- UDP utilizza un modello di comunicazione semplice e senza connessione con un minimo di meccanismi di protocollo.
- UDP fornisce checksum per l'integrità dei dati e numeri di porta per indirizzare diverse funzioni alla sorgente e alla destinazione del datagramma.
- Non ha dialoghi di handshaking e quindi espone il programma dell'utente a qualsiasi inaffidabilità della rete sottostante; non c'è garanzia di consegna, ordinamento o protezione dai duplicati.

## Applicazioni

- UDP è adatto per scopi in cui il controllo degli errori e la correzione non sono necessari o vengono eseguiti nell'applicazione.
- Le applicazioni sensibili al tempo spesso utilizzano UDP perché scartare i pacchetti è preferibile all'attesa di pacchetti ritardati a causa della ritrasmissione.
- Il protocollo è stato progettato da David P. Reed nel 1980 e definito formalmente in RFC 768.

## Limitazioni

- Sebbene UDP fornisca la verifica dell'integrità (tramite checksum) dell'intestazione e del payload, non fornisce garanzie al protocollo di livello superiore per la consegna dei messaggi e il livello UDP non conserva lo stato dei messaggi UDP una volta inviati.
- Per questo motivo, UDP a volte viene definito come Unreliable Datagram Protocol.
- Se si desidera la affidabilità della trasmissione, deve essere implementata nell'applicazione dell'utente.

# Dettagli Progetto

## Logica di Connessione

Come menzionato in precedenza, il server prima di cominciare a creare asteroidi e mandarli al client, ma prima un piccolo Handshake con il client per indirizzare correttamente i dati. Ecco com'è stato implementato l'Handshake:

Client.c
```c
// Creazione del socket.
int sockfd = Socket();

// Imposta famiglia e porta dell'indirizzo del server.
server_addr.sin_family = AF_INET;
server_addr.sin_port = htons(PORT);

// Si converte l'indirizzo IP passato a linea di comando.
IP_Conversion(argv[1], &server_addr);

// Invia un messaggio al server e ne verifica il successo.
if (Spedisci(sockfd, "ping", server_addr)) { . . . }
```

Server.c
```c
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
    if (Ricevi(sockfd, &client_addr)) { . . . }
```

Le funzioni utilizzate sono state create per rendere più leggibile il codice e semplificarlo. Le operazioni riguardanti la connessione sono state implementate nella libreria apposita "network.h".

L'Handshake consiste quindi in:

- Avvio del Server.c, che si posiziona in stato di attesa di ricezione di dati.

- Avvio del Client.c, che si connette all'indirizzo del server e spedisce un messaggio "ping".

- Il Server.c riceve il messaggio, soddisfa le condizioni booleane dell'if e inizia correttamente il ciclo.

- Analogamente, il Client.c verifica il successo del messaggio spedito, e inizia anch'esso il ciclo.

## Logica di Gioco

Come detto prima, il Server.c crea asteroidi e li manda al client ogni due secondi. Il Client.c invece riceve gli asteroidi e avvia il gioco.

Ecco come è stato implementato con relativi commenti per facilitare la comprensione.

Server.c
```c
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
```

Client.c
```c
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
```

Server.c è sufficientemente autoesplicativo, ma per comprendere meglio cosa sta succedendo in Client.c bisogna verificare i dettagli della libreria apposita "game.h", che stavolta riporto per intero con consueti commenti.

```c
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
```
```c
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
```
```c
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
```
```c
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
```
```c
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
```
```c
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
```
```c
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
```

I thread utilizzati in Client.c non sono quindi altro che funzioni asincrone implementati nell'header game.h.

# Conclusione

Il risultato finale è il seguente:

``` bash
BENVENUTO NEL GIOCO STARSHIP!
EVITA GLI ASTEROIDI CON W-A-S-D.
TURNO: 1   -   SCORE: 0
NUMERO DI ASTEROIDI CORRENTI: 25
. X X . . X X . .
X X . . . . . . X
. . . X X . . X .
. X . . . X . . .
. X . . A X . . .
X . X . . . . . X
. . . X X . X . .
X X . . . . . X .
. . . . X . . . .
```
``` bash
BENVENUTO NEL GIOCO STARSHIP!
EVITA GLI ASTEROIDI CON W-A-S-D.
TURNO: 2   -   SCORE: 10000
NUMERO DI ASTEROIDI CORRENTI: 26
. . . . X . . . .
. X . X X X . . X
X . . X . . . . .
. . . A . . . . .
X . . X . . X . X
X . X . . X . X X
. . . X X X . . .
X . X X . X X . .
. . . . . X . . .
```
``` bash
BENVENUTO NEL GIOCO STARSHIP!
EVITA GLI ASTEROIDI CON W-A-S-D.
TURNO: 3   -   SCORE: 30000
NUMERO DI ASTEROIDI CORRENTI: 27
. X X . X . . . X
. . X . A . . X X
. X X . . . . . X
X . X . . X X X X
X . . . X . X . .
X . X . . . . . .
X . . X . . . . .
. . . . . . . . X
X . . X . . . . X
``` 
``` bash
BENVENUTO NEL GIOCO STARSHIP!
EVITA GLI ASTEROIDI CON W-A-S-D.
TURNO: 4   -   SCORE: 60000
NUMERO DI ASTEROIDI CORRENTI: 28
X . X X X . . . .
X . . . A X X X .
X X . X X . . . .
. . X X . X . . .
. . X . X . . . .
. X X . . . . . .
. . . X . . . . .
. . X . . . X X X
X X X X . . . . .
``` 
``` bash
BENVENUTO NEL GIOCO STARSHIP!
EVITA GLI ASTEROIDI CON W-A-S-D.
TURNO: 5   -   SCORE: 100000
NUMERO DI ASTEROIDI CORRENTI: 29
. X . . . X . . .
. X X . X . X X .
X X . X . A . . .
X . X . . . . . .
X . . . X . . X .
. . X X . . X . X
. . X X . . X . X
. . . . . . . X .
X . . . X . X . X

!!! ALLERTA: ASTEROIDE NELLE TUE COORDINATE !!!
```
``` bash
!!! GAME OVER - COLLISIONE CON ASTEROIDE !!!

RIPROVA!
TURNO: 5   -   PUNTEGGIO: 100000
```

Il gioco permette libero movimento all'interno della griglia, e per aumentare la difficoltà col passare dei turni ho implementato una funzione che dopo ogni turno aumentano gli asteroidi di uno, per un massimo di 60 asteroidi totali sulla griglia 9x9.

# Manuale Utente

Per giocare al gioco Starship bisogna buildare con CMake gli eseguibili "Server" e "Client", e poi eseguire in ordine cronologico i due seguenti codici in un terminale UNIX.

Posizione build del Server partendo dalla cartella progetto di Starship:
``` bash
./build/cmake-build-debug/Server 127.0.0.1
```

Posizione build del Client partendo dalla cartella progetto di Starship:
``` bash
./build/cmake-build-debug/Client 127.0.0.1
``` 

# Crediti

Progetto sviluppato da Lorenzo Mirabella [0124002515] per l'esame di Reti di Calcolatori dell'anno accademico 2023/2024 dell'università Parthenope (NA) sostenuto con il professor Emanuel Di Nardo.