//
// Created by neo on 11/15/19.
//

#ifndef SERVER_H
#define SERVER_H

#include <pthread.h>
#include "requests.h"

char *API_KEY;
char *LOCATION;
int THREADS;
int PORT;
int DURATION;
const char *OPENWEATHERMAP_SERVER = "api.openweathermap.org";
int OPENWEATHERMAP_PORT = 80;
const char *OPENWEATHERMAP_GET = "GET /data/2.5/%s?q=%s&units=metric&APPID=%s HTTP/1.1\nHost: api.openweathermap.org\nUser-Agent: myOpenHAB\nAccept: application/json\nConnection: close\n\n";
pthread_mutex_t change_work;
pthread_mutex_t thread_lock;
pthread_cond_t client_ready;
int new_socket;
int available_work;

int read_config(char *filename);

int parse(char *request, char **method, char **path, char **connection, char **body);

int connect_socket(const char *server_name, int port, int *sock);

int get_weather_data();

#endif //EPL421_AS4_SERVER_H
