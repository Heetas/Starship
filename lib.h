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
#define MAX_ASTEROIDS 30
#define MAX_LINE 4096
#define PORT 1024
#define TIMER 2
#define clear() system("clear")

struct sockaddr_in server_addr, client_addr;
char buffer[MAX_LINE + 1];
int turn = 1;
int ship_x = 4;
int ship_y = 4;
int score = 0;
int score_increment = 10000;

typedef struct {
    int sockfd;
    struct sockaddr_in server_addr;
    int ship_x;
    int ship_y;
    int debris_x[MAX_ASTEROIDS];
    int debris_y[MAX_ASTEROIDS];
    int prev_ship_x;
    int prev_ship_y;
    char direction;
    int game_over;
} game_info;

#endif //STARSHIP_HEADER