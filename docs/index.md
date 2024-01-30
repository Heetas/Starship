---
title: Starship
author: Lorenzo Mirabella
---

# Introduzione

Starship è un progetto universitario di Reti di Calcolatori costruito nell'anno accademico 2023/2024 dallo studente Lorenzo Mirabella [0124002515] per il professore Emanuel Di Nardo.

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

Il protocollo User Datagram (UDP) è uno dei protocolli di comunicazione fondamentali della suite di protocolli Internet utilizzato per inviare messaggi (trasportati come datagrammi in pacchetti) ad altri host su una rete Internet Protocol (IP)[^1^][1].

## Caratteristiche

- UDP non richiede una comunicazione preliminare per impostare canali di comunicazione o percorsi di dati[^1^][1].
- UDP utilizza un modello di comunicazione semplice e senza connessione con un minimo di meccanismi di protocollo[^1^][1].
- UDP fornisce checksum per l'integrità dei dati e numeri di porta per indirizzare diverse funzioni alla sorgente e alla destinazione del datagramma[^1^][1].
- Non ha dialoghi di handshaking e quindi espone il programma dell'utente a qualsiasi inaffidabilità della rete sottostante; non c'è garanzia di consegna, ordinamento o protezione dai duplicati[^1^][1].

## Applicazioni

- UDP è adatto per scopi in cui il controllo degli errori e la correzione non sono necessari o vengono eseguiti nell'applicazione[^1^][1].
- Le applicazioni sensibili al tempo spesso utilizzano UDP perché scartare i pacchetti è preferibile all'attesa di pacchetti ritardati a causa della ritrasmissione[^1^][1].
- Il protocollo è stato progettato da David P. Reed nel 1980 e definito formalmente in RFC 768[^1^][1].

## Limitazioni

- Sebbene UDP fornisca la verifica dell'integrità (tramite checksum) dell'intestazione e del payload, non fornisce garanzie al protocollo di livello superiore per la consegna dei messaggi e il livello UDP non conserva lo stato dei messaggi UDP una volta inviati[^1^][1].
- Per questo motivo, UDP a volte viene definito come Unreliable Datagram Protocol[^1^][1].
- Se si desidera la affidabilità della trasmissione, deve essere implementata nell'applicazione dell'utente[^1^][1].

