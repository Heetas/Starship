// Server.c
#include "header.h"

int main(int argc, char **argv)
{
    int sockfd;
    struct sockaddr_in servaddr, cliaddr;
    ssize_t n;
    socklen_t len;
    char mesg[MAXLINE], line[MAXLINE];

    // Creazione del socket
    if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        perror("socket error");
        exit(1);
    }

    // Inizializzazione della struttura dati per l'indirizzo del server
    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servaddr.sin_port = htons(1024);

    // Associazione dell'indirizzo al socket
    if (bind(sockfd, (struct sockaddr *) &servaddr, sizeof(servaddr)) < 0) {
        perror("bind error");
        exit(1);
    }

    // Ciclo infinito di ricezione dei messaggi dai client e invio delle risposte
    for (;;) {
        len = sizeof(cliaddr);

        // Ricezione del messaggio dal client
        n = recvfrom(sockfd, mesg, MAXLINE, 0, (struct sockaddr *) &cliaddr, &len);
        n--;

        // Stampa dell'indirizzo del client e del messaggio ricevuto
        printf("Received request from %s\n", inet_ntop(AF_INET, &cliaddr.sin_addr, line, sizeof(line)));
        mesg[n] = '\0';
        printf("Received %zd bytes: %s\n", n, mesg);

        // Creazione della risposta
        char str[100];
        sprintf(str, "Received %zd characters\n", n);

        // Invio della risposta al client
        sendto(sockfd, str, strlen(str), 0, (struct sockaddr *) &cliaddr, sizeof(cliaddr));
    }
}