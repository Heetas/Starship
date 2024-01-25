// lib.h
#ifndef STARSHIP_HEADER
#define STARSHIP_HEADER

// Inclusione delle librerie necessarie
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <errno.h>
#include <time.h>
#include <pthread.h>
#include <unistd.h>

#define GRID 9
#define MAX_ASTEROIDS 3
#define MAX_LINE 4096
#define PORT 1024
#define TIMER 2
#define clear() system("clear")

int turn = 1;

typedef struct {
    int sockfd;
    struct sockaddr_in server_addr;
    int ship_x;
    int ship_y;
    int debris_x[MAX_ASTEROIDS];
    int debris_y[MAX_ASTEROIDS];
} thread_data_t;

#endif //STARSHIP_HEADER