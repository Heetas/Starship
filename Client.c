// Client.c
#include "header.h"

int main(int argc, char **argv) {
    int sockfd;
    struct sockaddr_in servaddr;
    char sendline[MAXLINE], recvline[MAXLINE + 1];

    // Controllo del numero di argomenti passati al programma
    if (argc != 2) {
        fprintf(stderr, "usage: %s <IPaddress>\n", argv[0]);
        exit(1);
    }

    // Creazione del socket
    if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        perror("socket error");
        exit(1);
    }

    // Inizializzazione della struttura dati per l'indirizzo del server
    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(1024);

    if (inet_pton(AF_INET, argv[1], &servaddr.sin_addr) <= 0) {
        fprintf(stderr, "inet_pton error for %s\n", argv[1]);
        exit(1);
    }

    // Ciclo di lettura da standard input, invio al server e ricezione della risposta
    while (fgets(sendline, MAXLINE, stdin) != NULL) {

        // Invio del messaggio al server
        sendto(sockfd, sendline, strlen(sendline), 0, (struct sockaddr *) &servaddr, sizeof(servaddr));

        // Ricezione della risposta dal server
        int n = recvfrom(sockfd, recvline, MAXLINE, 0, NULL, NULL);

        // Aggiunta del carattere di terminazione alla fine della stringa
        recvline[n] = 0;

        // Stampa della risposta a video
        fputs(recvline, stdout);
        printf("\n");
    }

    exit(0);
}