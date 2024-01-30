---
title: Starship
author: Lorenzo Mirabella
---

# Starship [2023/2024]

Starship è un progetto universitario di Reti di Calcolatori costruito nell'anno accademico 2023/2024 dallo studente Lorenzo Mirabella [0124002515] per il professore Emanuel Di Nardo.

## Traccia

Una navicella spaziale deve evitare i detriti provenienti da una tempesta di meteoriti.
La navicella (client) entra nel settore spaziale dei meteoriti (server) connettendosi in UDP. La tempesta di meteoriti genera in modo casuale n pacchetti UDP ogni 2 secondi che rappresentano i detriti in una griglia MxM. La navicella riceve un alert nel caso in cui il detrito spaziale si trovi sulla sua stessa posizione e può spostarsi prima dell'impatto.

## Funzionamento e Logica

La logica nel progetto consiste nel server e del client in struttura UDP, ed è stato sviluppato in C.

Il server appena riceve notifica della connessione del client, comincia a generare coordinate di asteroidi ogni 2 secondi e li spedisce al client, che ottiene le coordinate dal messaggio ricevuto e le traduce in coordinate all'interno della griglia di gioco.

Il giocatore può muoversi liberamente sulla griglia, ed il suo obiettivo è quello di evitare le coordinate nel quale vi sono gli asteroidi. Se il giocatore dovesse iniziare il turno successivo, cioè arriva un altro pacchetto di asteroidi, mentre è collocato nelle coordinate di un asteroide, allora sarà un Game Over.

Per gestire tutti questi processi contemporaneamente in C, sono stati utilizzati 4 thread:
- move_thread: Thread che si è occupato di permettere al giocatore di muoversi a piacimento sulla griglia ed aggiornare le posizioni dopo ogni spostamento.

- update_thread: Thread impostato per aggiornare la griglia degli asteroidi, e controllare se il giocatore finisce il turno mentre è su un asteroide.

- collision_thread: Thread con il ruolo di verificare la posizione della navicella rispetto agli asteroidi presenti sulla griglia. Se la navicella è su un asteroide, il giocatore riceve il messaggio di allerta e vengono salvate le coordinate della collisione. Se il giocatore riesce a spostarsi in tempo, resetta le coordinate di collisione.

- increment_thread: Thread atto ad aggiornare i dati di gioco su tutto il sistema del client, come il turno ed il punteggio.