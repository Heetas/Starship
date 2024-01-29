// network.h

#include "lib.h"
#ifndef STARSHIP_NETWORK_H
#define STARSHIP_NETWORK_H

// La funzione "error" viene utilizzata per gestire gli errori. Accetta una stringa come parametro, la quale descrive l'errore riscontrato.
// Dopodiché stampa l'errore ricevuto come parametro sullo stream di errore standard e termina l'esecuzione del programma.
void error(const char *msg) {
    perror(msg);
    exit(1);
}

// La funzione "Socket" crea un nuovo file descriptor di socket.
// Se la creazione fallisce, la funzione chiama la funzione "error" coi dettagli dell'errore.
int Socket() {
    int sockfd;
    if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        error("Errore durante la creazione del socket");
    }
    return sockfd;
}

// La funzione "Bind" associa l'indirizzo del server al socket.
// Se l'operazione di bind fallisce, chiama la funzione "error" coi dettagli dell'errore.
void Bind(int sockfd, struct sockaddr_in servaddr) {
    if (bind(sockfd, (struct sockaddr *) &servaddr, sizeof(servaddr)) < 0) {
        error("Errore durante il bind");
    }
}

// La funzione "IP_Conversion" converte un indirizzo IP da una forma presentabile in una forma numerica binaria utilizzabile nella struttura sockaddr_in.
// Se la conversione fallisce, chiama la funzione "error" coi dettagli dell'errore.
void IP_Conversion(const char *ip, struct sockaddr_in *addr) {
    if (inet_pton(AF_INET, ip, &(addr->sin_addr)) <= 0) {
        error("Errore durante la conversione dell'IP");
    }
}

// La funzione "Spedisci" spedisce un pacchetto di dati.
// Restituisce il numero di caratteri spediti, oppure un valore negativo in caso di errore.
ssize_t Spedisci(int sockfd, const char *data, struct sockaddr_in addr) {
    ssize_t n = sendto(sockfd, data, strlen(data), 0, (struct sockaddr *) &addr, sizeof(addr));
    return n;
}

// La funzione "Ricevi" riceve un pacchetto di dati.
// Il pacchetto viene salvato nel buffer specificato.
// Restituisce il numero di caratteri ricevuti.
ssize_t Ricevi(int sockfd, struct sockaddr_in *addr) {
    socklen_t len = sizeof(*addr);
    ssize_t n = recvfrom(sockfd, buffer, MAX_LINE, 0, (struct sockaddr *) addr, &len);
    if (n > 0) buffer[n] = '\0';
    return n;
}

#endif //STARSHIP_NETWORK_H