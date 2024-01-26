// network.h
#include "lib.h"
#ifndef STARSHIP_NETWORK_H
#define STARSHIP_NETWORK_H

// Funzione per gestire gli errori e terminare il programma
void error(const char *msg) {
    perror(msg);
    exit(1);
}

// Funzione per la creazione del socket
int Socket() {
    int sockfd;
    if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        error("Errore durante la creazione del socket");
    }
    return sockfd;
}

// Funzione per eseguire il bind dell'indirizzo al socket
void Bind(int sockfd, struct sockaddr_in servaddr) {
    if (bind(sockfd, (struct sockaddr *) &servaddr, sizeof(servaddr)) < 0) {
        error("Errore durante il bind");
    }
}

// Funzione per la conversione dell'indirizzo IP
void IP_Conversion(const char *ip, struct sockaddr_in *addr) {
    if (inet_pton(AF_INET, ip, &(addr->sin_addr)) <= 0) {
        error("Errore durante la conversione dell'IP");
    }
}

// Funzione per la spedizione dei dati tramite socket
ssize_t Spedisci(int sockfd, const char *data, struct sockaddr_in addr) {
    ssize_t n = sendto(sockfd, data, strlen(data), 0, (struct sockaddr *) &addr, sizeof(addr));
    return n;
}

// Funzione per la ricezione dei dati tramite socket
ssize_t Ricevi(int sockfd, char *buf, struct sockaddr_in *addr) {
    socklen_t len = sizeof(*addr);
    ssize_t n = recvfrom(sockfd, buf, MAX_LINE, 0, (struct sockaddr *) addr, &len);
    if (n > 0) buf[n] = '\0';
    return n;
}

#endif //STARSHIP_NETWORK_H
