#ifndef REQUESTS_H
#define REQUESTS_H

#include <sys/types.h> // For sockets
#include <sys/socket.h> // For sockets
#include <netinet/in.h> // For Internet sockets
#include <netdb.h> // For gethostbyaddr
#include <stdio.h> // For I/O
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <time.h>    // time_t, time
#include <signal.h>  // signal, alarm
#include "myjson.h"

const char *NOT_IMPLEMENTED;
const char *REPLY_OK_CLOSE;
const char *REPLY_OK_ALIVE;
const char *NOT_FOUND;
const char *ITEM_NOT_FOUND;

int item_not_found_reply(int newsock);

int item_exists(char *item);

int path_exist(char *path);

int method_exist(char *method);

int header_reply(int newsock, char *connection, int content_length, char *temp_buf, int body_reply);

int get_request(int newsock, char *path, char *connection, int find_length);

int delete_request(int newsock, char *path, char *connection);

int put_request(int newsock, char *path, char *connection, char *body);

#endif
