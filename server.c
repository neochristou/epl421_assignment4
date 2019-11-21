/* File: int_str_server.c */
#include <sys/types.h> // For sockets
#include <sys/socket.h> // For sockets
#include <netinet/in.h> // For Internet sockets
#include <netdb.h> // For gethostbyaddr
#include <stdio.h> // For I/O
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>

//api.openweathermap.org//data/2.5/weather?q=Nicosia,cy&APPID=64e92529c453f7621bd77a0948526d55

char *API_KEY;
char *LOCATION;
int THREADS;
int PORT;
int DURATION;
const char *OPENWEATHERMAP_SERVER = "api.openweathermap.org";
int OPENWEATHERMAP_PORT = 80;
const char* OPENWEATHERMAP_GET = "GET /data/2.5/%s?q=%s&units=metric&APPID=%s HTTP/1.1\nHost: api.openweathermap.org\nUser-Agent: myOpenHAB\nAccept: application/json\nConnection: close\n\n";

const char* NOT_IMPLEMENTED = "HTTP/1.1 501 Not Implemented\r\nServer: my_webserver\r\nConnection: close\r\nContent-Type: text/plain\r\nContent-Length: 24\r\n\r\nMethod not implemented!";
const char *REPLY_OK_CLOSE = "HTTP/1.1 200 OK\r\nServer: my_webserver\r\nConnection: close\r\nContent-Type: text/plain\r\nContent-Length: ";
const char *REPLY_OK_ALIVE = "HTTP/1.1 200 OK\r\nServer: my_webserver\r\nConnection: keep-alive\r\nContent-Type: text/plain\r\nContent-Length: ";
const char* NOT_FOUND = "HTTP/1.1 404 NotFound\r\nServer: my_webserver\r\nConnection: keep-alive\r\nContent-Type: text/plain\r\nContent-Length: 16\r\n\r\nPath not found!";

int read_config(char *filename){
    FILE *config_file;
    char buf[256];
    if ( (config_file = fopen(filename,"r")) == NULL ){
        perror("fopen");
        return EXIT_FAILURE;
    }

    char *argument, *equals, *value;
    while (fgets(buf, 255, config_file) != NULL){
        if ( (equals = strchr(buf, '=')) != NULL){
            if ((argument = (char *) calloc((equals - buf), sizeof(char))) == NULL){
                perror("malloc argument");
                return EXIT_FAILURE;
            }
            strncpy(argument, buf, equals - buf);
            argument[strlen(argument)] = '\0';
            if ((value = (char *) calloc((strlen(buf) - strlen(argument)), sizeof(char))) == NULL){
                perror("malloc value");
                return EXIT_FAILURE;
            }
            strncpy(value, equals + 1, strlen(buf) - strlen(argument));
            value[strlen(value)] = '\0';
            if (strcmp(argument, "THREADS") == 0){
                THREADS = atoi(value);
            }if (strcmp(argument, "DURATION") == 0){
                DURATION = atoi(value);
            }
            if (strcmp(argument, "PORT") == 0){
                PORT = atoi(value);
            }
            if (strcmp(argument, "LOCATION") == 0){
                if ((LOCATION = (char *) calloc((strlen(value) + 1), sizeof(char))) == NULL){
                    perror("malloc LOCATION");
                    return EXIT_FAILURE;
                }
                strncpy(LOCATION, value, strlen(value));
                LOCATION[strlen(LOCATION)] = '\0';
            }
            if (strcmp(argument, "API_KEY") == 0){
                if ((API_KEY = (char *) calloc((strlen(value) + 1), sizeof(char))) == NULL){
                    perror("malloc API_KEY");
                    return EXIT_FAILURE;
                }
                strncpy(API_KEY, value, strlen(value));
                API_KEY[strlen(API_KEY)] = '\0';
            }
            free(argument);
            free(value);
        }
    }

    return EXIT_SUCCESS;
}

int parse(char *request, char **method, char **path, char **connection) {
    const char *start_of_path = strchr(request, ' ') + 1;

    if ((*method  = (char *) calloc((start_of_path - request - 1), sizeof(char))) == NULL) {
        perror("malloc method");
        return EXIT_FAILURE;
    }
    strncpy(*method, request, start_of_path - request - 1);
    (*method)[strlen(*method)] = '\0';

    const char *end_of_path = strchr(start_of_path, ' ');
    if ((*path = (char *) calloc((end_of_path - start_of_path + 1), sizeof(char))) == NULL) {
        perror("malloc path");
        return EXIT_FAILURE;
    }
    strncpy(*path, start_of_path, end_of_path - start_of_path);
    (*path)[strlen(*path)] = '\0';

    char *name = NULL, *value_start = NULL, *value_end = NULL, *value = NULL;
    char *next_line = strchr(end_of_path, '\n') + 1;
    while ((strcmp(next_line, "\r\n") != 0) && next_line != NULL) {
        value_start = strchr(next_line, ':');
        if ((name = (char *) calloc((value_start - next_line), sizeof(char))) == NULL) {
            perror("malloc name");
            return EXIT_FAILURE;
        }
        strncpy(name, next_line, value_start - next_line);
        name[strlen(name)] = '\0';
//        printf("Name: %s\n", name);

        value_start = value_start + 2;
        value_end = strchr(value_start, '\r');
        if ((value = (char *) calloc((value_end - value_start), sizeof(char))) == NULL) {
            perror("malloc value");
            return EXIT_FAILURE;
        }
        strncpy(value, value_start, value_end - value_start);
        value[strlen(value)] = '\0';
//        printf("Value: %s\n", value);

        if (strcmp(name, "Connection") == 0) {
            if (((*connection) = (char *) calloc((value_end - value_start), sizeof(char))) == NULL) {
                perror("malloc connection");
                return EXIT_FAILURE;
            }
            strncpy((*connection), value, strlen(value));
        }

        free(name);
        free(value);
        next_line = value_end + 2;
    }

    return EXIT_SUCCESS;
}

int connect_socket(const char *server_name, int port, int *sock) {
    int serverlen;
    struct sockaddr_in server;
    struct sockaddr *serverptr;
    struct hostent *rem;
    if ((*sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {  /* Create socket */
        perror("socket");
        return EXIT_FAILURE;
    }
    if ((rem = gethostbyname(server_name)) == NULL) {
        perror("gethostbyname");
        return EXIT_FAILURE;
    }

    bcopy((char *) rem -> h_addr, (char *) &server.sin_addr, rem -> h_length);
    server.sin_family = AF_INET; /* Internet domain */
    server.sin_port = htons(port);
    serverptr = (struct sockaddr *) &server;
    serverlen = sizeof server;

    if (connect(*sock, serverptr, serverlen) < 0) {/* Request Connect */
        perror("connect");
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}

int get_weather_data() {

    char buf[256];
    int res;
    int sock1, sock2;

    if ( (connect_socket(OPENWEATHERMAP_SERVER, OPENWEATHERMAP_PORT, &sock1)) == EXIT_FAILURE) {
        printf("Error in connect socket\n");
        return  EXIT_FAILURE;
    }

    /*Get current weather data and save to json file*/
    int file_current = open("../current.json", O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (file_current < 0) {
        perror("opening file");
        return EXIT_FAILURE;
    }
    bzero(buf, sizeof buf); /* Initialize buffer */
    sprintf(buf , OPENWEATHERMAP_GET, "weather", LOCATION, API_KEY);
    if (write(sock1, buf, sizeof buf) < 0) {  /* Send message */
        perror("write");
        return EXIT_FAILURE;
    }
    bzero(buf, sizeof buf); /* Initialize buffer */
    while ((res = read(sock1, buf, sizeof buf)) > 0) {
        if (res < 0) {
            perror("read");
            return EXIT_FAILURE;
        }
        if ( write(file_current, buf, strlen(buf)) != strlen(buf) ) {
            perror ("write error");
            return EXIT_FAILURE;
        }
        bzero(buf, sizeof buf); /* Initialize buffer */
    }
    if (close(file_current) < 0) {
        perror("Closing file:");
        return EXIT_FAILURE;
    }
    close(sock1);

    if ( (connect_socket(OPENWEATHERMAP_SERVER, OPENWEATHERMAP_PORT, &sock2)) == EXIT_FAILURE) {
        printf("Error in connect socket\n");
        return  EXIT_FAILURE;
    }
    /*Get forecast weather data and save to json file*/
    int file_forecast = open("../forecast.json", O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (file_forecast < 0) {
        perror("opening file");
        return EXIT_FAILURE;
    }
    bzero(buf, sizeof buf); /* Initialize buffer */
    sprintf(buf, OPENWEATHERMAP_GET, "forecast", LOCATION, API_KEY, "close");

    if (write(sock2, buf, sizeof buf) < 0) {  /* Send message */
        perror("write");
        return EXIT_FAILURE;
    }
    bzero(buf, sizeof buf); /* Initialize buffer */
    while ((res = read(sock2, buf, sizeof buf)) > 0) {
        if (res < 0) {
            perror("read");
            return EXIT_FAILURE;
        }
        if ( write(file_forecast, buf, strlen(buf)) != strlen(buf) ) {
            perror ("write error");
            return EXIT_FAILURE;
        }
        bzero(buf, sizeof buf); /* Initialize buffer */
    }
    if (close(file_forecast) < 0) {
        perror("Closing file:");
        return EXIT_FAILURE;
    }
    close(sock2);

    return EXIT_SUCCESS;
}

int header_reply(int newsock, char *connection, int content_length) {
    char buf[256];
    char content_header[10];
    sprintf(content_header,"%d\r\n\r\n",content_length);
    bzero(buf, sizeof buf);
    if(!strcmp(connection,"close"))
        strcpy(buf,REPLY_OK_CLOSE);
    else
        strcpy(buf,REPLY_OK_ALIVE);
    strcat(buf,content_header);
    if (write(newsock, buf, sizeof buf) < 0) { 
        perror("write");
        exit(1);
    }
    return EXIT_SUCCESS;
}

int item_exists(char *item) {
    if (!strcmp(item,"c") || !strcmp(item, "station_id") || !strcmp(item, "current_time") || !strcmp(item, "current_temp") || !strcmp(item, "current_pressure") || !strcmp(item, "current_humidity") || !strcmp(item, "current_speed") || !strcmp(item, "current_cloudiness") || !strcmp(item, "current_rain") || !strcmp(item, "current_sunrise") || !strcmp(item, "current_sunset") || !strcmp(item, "forecast3_time") || !strcmp(item, "forecast3_temp") || !strcmp(item, "forecast6_time") || !strcmp(item, "forecast6_temp") || !strcmp(item, "forecast9_time") || !strcmp(item, "forecast9_temp") || !strcmp(item, "forecast12_time") || !strcmp(item, "forecast12_temp")) {
        return EXIT_SUCCESS;
    }
    return EXIT_FAILURE;
}

int path_exist(char *path) {
    if (!strcmp(path, "/items")) {
        return EXIT_SUCCESS;
    }
    else if (!strncmp("/items/", path, 7)) {
        char *item_name = &path[7];
        if (item_exists(item_name) == EXIT_SUCCESS) {
            return EXIT_SUCCESS;
        }
    }
    return EXIT_FAILURE;
}

int method_exist(char *method) {
    if (!strcmp(method, "GET") || !strcmp(method, "PUT") || !strcmp(method, "DELETE") || !strcmp(method, "HEAD")) {
        return EXIT_SUCCESS;
    }
    return EXIT_FAILURE;
}

int get_request(int newsock, char *path, char *connection) {
    int content_length=0;
    if (!strcmp(path, "/items")) {
        // Prepare items to send
        //printf("Yes /items\n");
    }
    else if (!strncmp("/items/", path, 7)) {
        // Prepare items to send
    }
    // First find content length
    header_reply(newsock,connection,content_length);
    //Send data
    return EXIT_SUCCESS;
}

int main(int argc, char *argv[]) { /* Server with Internet stream sockets */
    if (get_weather_data() == EXIT_FAILURE) {
        printf("Error in get_weather_data\n");
    }

    if (read_config("./config.txt") == EXIT_FAILURE){
        printf("Error in read_config\n");
    }

    char *a, *b, *c;
    parse("GET /items/current_time HTTP/1.1\r\nHost: nicolas\r\nConnection: close\r\n\r\n", &a, &b, &c );

     printf("a=%s\nb=%s\nc=%s\n",a,b,c);
    // sleep(3000)

    int sock, newsock, serverlen, yes = 1; // clientlen;
    socklen_t clientlen;
    char buf[256];
    struct sockaddr_in server, client;
    struct sockaddr *serverptr, *clientptr;
    struct hostent *rem;

    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {  /* Create socket */
        perror("socket");
        exit(1);
    }
    if (setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) == -1) {
        perror("setsockopt");
        exit(1);
    }
    server.sin_family = AF_INET; /* Internet domain */
    server.sin_addr.s_addr = htonl(INADDR_ANY); /* My Internet address */
    server.sin_port = htons(PORT); /* The given port */
    serverptr = (struct sockaddr *) &server;
    serverlen = sizeof server;

    if (bind(sock, serverptr, serverlen) < 0) {  /* Bind socket to an address */
        perror("bind");
        exit(1);
    }
    if (listen(sock, 5) < 0) {  /* Listen for connections */
        perror("listen");
        exit(1);
    }

    printf("Listening for connections to port %d\n", PORT);

    while (1) {
        clientptr = (struct sockaddr *) &client;
        clientlen = sizeof client;
        if ((newsock = accept(sock, clientptr, &clientlen)) < 0) {
            perror("accept");
            exit(1);
        } /* Accept connection */

        char *method = NULL;
        char *path = NULL;
        char *connection = NULL;
        switch (fork()) {  /* Create child for serving the client */
        case -1: {
            perror("fork");
            exit(1);
        }
        case 0: { /* Child process */
            do {
                bzero(buf, sizeof buf); /* Initialize buffer */
                if (read(newsock, buf, sizeof buf) < 0) {  /* Receive message */
                    perror("read");
                    exit(1);
                }
//                printf("BUF: %s\n\n", buf);
//                parse(buf, &method, &path, &connection);
//                printf("Method: %s\n", method);
//                printf("Path: %s\n", path);
//                printf("Connection: %s\n", connection);
                bzero(buf, sizeof buf);
                if (method_exist(method) == EXIT_FAILURE) {
                    strcpy(buf, NOT_IMPLEMENTED);
                }
                else if (path_exist(path) == EXIT_SUCCESS) {
                    if (!strcmp(method, "GET")) {
                        get_request(newsock, path, connection);
                    }
                    else if (!strcmp(method, "HEAD")) {
                        int length =0;
                        //Find header length function
                        header_reply(newsock,connection,length);
                    }
                    else if (!strcmp(method, "PUT")) {
                        header_reply(newsock,connection,0);
                        // Call function to do action
                    }
                    else if (!strcmp(method, "DELETE")) {
                        header_reply(newsock,connection,0);
                        // Call function to do action
                    }
                    //sprintf(buf, "Reply\n");
                }
                else {
                    strcpy(buf, NOT_FOUND);
                }
                if (write(newsock, buf, sizeof buf) < 0) { /* Send message */
                    perror("write");
                    exit(1);
                }
            } while (strcmp(connection, "close") != 0); /* Finish on "end" */
        }
            close(newsock); /* Close socket */
        printf("Connection from %s is closed\n", rem -> h_name);
        exit(0);
        }
    }
}
