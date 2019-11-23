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
#include "myjson.h"

//api.openweathermap.org//data/2.5/weather?q=Nicosia,cy&APPID=64e92529c453f7621bd77a0948526d55
char *choices_array[18] = { "station_id", "current_time", "current_temp", "current_pressure", "current_humidity", "current_speed", "current_cloudiness", "current_rain", "current_sunrise", "current_sunset", "forecast3_time", "forecast3_temp", "forecast6_time", "forecast6_temp", "forecast9_time", "forecast9_temp", "forecast12_time", "forecast12_temp"};
json_t *weather_json_struct = NULL;

char *API_KEY = "64e92529c453f7621bd77a0948526d55";
char *LOCATION = "Nicosia,cy";
int THREADS = 40;
int PORT = 2000;
int DURATION;
const char *OPENWEATHERMAP_SERVER = "api.openweathermap.org";
int OPENWEATHERMAP_PORT = 80;
const char* OPENWEATHERMAP_GET = "GET /data/2.5/%s?q=%s&units=metric&APPID=%s HTTP/1.1\nHost: api.openweathermap.org\nUser-Agent: myOpenHAB\nAccept: application/json\nConnection: close\n\n";

const char* NOT_IMPLEMENTED = "HTTP/1.1 501 Not Implemented\r\nServer: my_webserver\r\nConnection: close\r\nContent-Type: text/plain\r\nContent-Length: 24\r\n\r\nMethod not implemented!";
const char *REPLY_OK_CLOSE = "HTTP/1.1 200 OK\r\nServer: my_webserver\r\nConnection: close\r\nContent-Type: text/plain\r\nContent-Length: ";
const char *REPLY_OK_ALIVE = "HTTP/1.1 200 OK\r\nServer: my_webserver\r\nConnection: keep-alive\r\nContent-Type: text/plain\r\nContent-Length: ";
const char* NOT_FOUND = "HTTP/1.1 404 NotFound\r\nServer: my_webserver\r\nConnection: close\r\nContent-Type: text/plain\r\nContent-Length: 16\r\n\r\nPath not found!";
const char* ITEM_NOT_FOUND = "HTTP/1.1 404 NotFound\r\nServer: my_webserver\r\nConnection: close\r\nContent-Type: text/plain\r\nContent-Length: 16\r\n\r\nItem not found!";

int read_config(char *filename) {
    FILE *config_file;
    char buf[256];
    if ( (config_file = fopen(filename, "r")) == NULL ) {
        perror("fopen");
        return EXIT_FAILURE;
    }

    char *argument, *equals, *value;
    while (fgets(buf, 255, config_file) != NULL) {
        if ( (equals = strchr(buf, '=')) != NULL) {
            if ((argument = (char *) calloc((equals - buf), sizeof(char))) == NULL) {
                perror("malloc argument");
                return EXIT_FAILURE;
            }
            strncpy(argument, buf, equals - buf);
            argument[strlen(argument)] = '\0';
            if ((value = (char *) calloc((strlen(buf) - strlen(argument)), sizeof(char))) == NULL) {
                perror("malloc value");
                return EXIT_FAILURE;
            }
            strncpy(value, equals + 1, strlen(buf) - strlen(argument));
            value[strlen(value)] = '\0';
            if (strcmp(argument, "THREADS") == 0) {
                THREADS = atoi(value);
            } if (strcmp(argument, "DURATION") == 0) {
                DURATION = atoi(value);
            }
            if (strcmp(argument, "PORT") == 0) {
                PORT = atoi(value);
            }
            if (strcmp(argument, "LOCATION") == 0) {
                if ((LOCATION = (char *) calloc((strlen(value) + 1), sizeof(char))) == NULL) {
                    perror("malloc LOCATION");
                    return EXIT_FAILURE;
                }
                strncpy(LOCATION, value, strlen(value));
                LOCATION[strlen(LOCATION)] = '\0';
            }
            if (strcmp(argument, "API_KEY") == 0) {
                if ((API_KEY = (char *) calloc((strlen(value) + 1), sizeof(char))) == NULL) {
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

int parse(char *request, char **method, char **path, char **connection, char **body) {
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

    char *last_bytes = malloc (3 * sizeof(char));
    strncpy(last_bytes, next_line, 2);
    last_bytes[3] = '\0';

    while ((strcmp(last_bytes, "\r\n") != 0) && next_line != NULL) {
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

        strncpy(last_bytes, next_line, 2);
        last_bytes[3] = '\0';
    }
    next_line += 2;
    (*body) = next_line;

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

    bzero(buf, sizeof buf); /* Initialize buffer */
    sprintf(buf , OPENWEATHERMAP_GET, "weather", LOCATION, API_KEY);
    if (write(sock1, buf, sizeof buf) < 0) {  /* Send message */
        perror("write");
        return EXIT_FAILURE;
    }
    char current_json_string[1000];
    bzero(current_json_string, sizeof(current_json_string));
    bzero(buf, sizeof buf); /* Initialize buffer */
    while ((res = read(sock1, buf, sizeof(buf))) > 0) {
        if (res < 0) {
            perror("read");
            return EXIT_FAILURE;
        }
        strncat(current_json_string, buf, sizeof(buf));
        bzero(buf, sizeof buf); /* Initialize buffer */
    }

    //printf("%s\n", current_json_string);
    close(sock1);

    if ( (connect_socket(OPENWEATHERMAP_SERVER, OPENWEATHERMAP_PORT, &sock2)) == EXIT_FAILURE) {
        printf("Error in connect socket\n");
        return  EXIT_FAILURE;
    }
    bzero(buf, sizeof buf); /* Initialize buffer */
    sprintf(buf, OPENWEATHERMAP_GET, "forecast", LOCATION, API_KEY, "close");

    if (write(sock2, buf, sizeof buf) < 0) {  /* Send message */
        perror("write");
        return EXIT_FAILURE;
    }

    char forecast_json_string[50000];
    bzero(forecast_json_string, sizeof(forecast_json_string));

    bzero(buf, sizeof buf); /* Initialize buffer */
    while ((res = read(sock2, buf, sizeof buf)) > 0) {
        if (res < 0) {
            perror("read");
            return EXIT_FAILURE;
        }
        strncat(forecast_json_string, buf, sizeof(buf));
        bzero(buf, sizeof buf); /* Initialize buffer */
    }

    close(sock2);
    //printf("%s\n",forecast_json_string);
    //json_t *createJsonStruct(char *, char *);
    char *current_json_string_noheader = JsonHeaderRemover(current_json_string);
    char *forecast_json_string_noheader = JsonHeaderRemover(forecast_json_string);

    weather_json_struct = createJsonStruct(current_json_string_noheader, forecast_json_string_noheader);

    //printf("JSON:\n%s\n", json_dumps(json_struct,JSON_ENSURE_ASCII));
    //printf("%s\n",forecast_json_string_noheader);
    return EXIT_SUCCESS;
}

int item_not_found_reply(int newsock) {
    char buf[256];
    strcpy(buf, ITEM_NOT_FOUND);
    if (write(newsock, buf, sizeof buf) < 0) {
        perror("write");
        exit(1);
    }
    return EXIT_SUCCESS;
}

int header_reply(int newsock, char *connection, int content_length, char *temp_buf, int body_reply) {
    char buf[25600];
    char content_header[10];
    sprintf(content_header, "%d\r\n\r\n", content_length);
    bzero(buf, sizeof buf);
    if (!strcmp(connection, "close"))
        strcpy(buf, REPLY_OK_CLOSE);
    else
        strcpy(buf, REPLY_OK_ALIVE);
    strcat(buf, content_header);
    if (body_reply)
        strcat(buf, temp_buf);
    if (write(newsock, buf, sizeof buf) < 0) {
        perror("write");
        exit(1);
    }
    return EXIT_SUCCESS;
}

/*int body_reply(int newsock, char *body) {
    if(write(newsock, body, sizeof(body)) < 0) {
        perror("read");
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}*/

int item_exists(char *item) {
    if (!strcmp(item, "station_id") || !strcmp(item, "current_time") || !strcmp(item, "current_temp") || !strcmp(item, "current_pressure") || !strcmp(item, "current_humidity") || !strcmp(item, "current_speed") || !strcmp(item, "current_cloudiness") || !strcmp(item, "current_rain") || !strcmp(item, "current_sunrise") || !strcmp(item, "current_sunset") || !strcmp(item, "forecast3_time") || !strcmp(item, "forecast3_temp") || !strcmp(item, "forecast6_time") || !strcmp(item, "forecast6_temp") || !strcmp(item, "forecast9_time") || !strcmp(item, "forecast9_temp") || !strcmp(item, "forecast12_time") || !strcmp(item, "forecast12_temp")) {
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

int get_request(int newsock, char *path, char *connection, int find_length) {
    char *temp_buf = malloc(sizeof(char) * 100000);
    bzero(temp_buf, 100000);
    int i, count = 0;
    if (!strcmp(path, "/items")) {
        for (i = 0; i < 18; i++) {
            char *temp_buf2 = json_dumps(json_array_get(weather_json_struct, i), JSON_ENSURE_ASCII);
            if (strcmp(temp_buf2, "{}")) {
                strcat(temp_buf, temp_buf2);
                count++;
            }
        }
        if (count == 0) {
            item_not_found_reply(newsock);
            free(temp_buf);
            return EXIT_FAILURE;
        }
    }
    else if (!strncmp("/items/", path, 7)) {
        char *item_name = &path[7];
        for (i = 0; i < 18; i++) {
            if (strcmp(item_name, choices_array[i]) == 0) {
                break;
            }
        }
        //json_object_clear(json_array_get(weather_json_struct, i));
        temp_buf = json_dumps(json_array_get(weather_json_struct, i), JSON_ENSURE_ASCII);
        if (!strcmp(temp_buf, "{}")/*strlen(temp_buf)==2*/) {
            item_not_found_reply(newsock);
            free(temp_buf);
            return EXIT_FAILURE;
        }
    }
    if (find_length) {
        free(temp_buf);
        return strlen(temp_buf);
    }
    header_reply(newsock, connection, strlen(temp_buf), temp_buf, 1);
    free(temp_buf);
    return EXIT_SUCCESS;
}

int delete_request(int newsock, char *path, char *connection) {
    int i;
    if (!strcmp(path, "/items")) {
        for (i = 0; i < 18; i++) {
            json_object_clear(json_array_get(weather_json_struct, i));
        }
    }
    else if (!strncmp("/items/", path, 7)) {
        char *item_name = &path[7];
        int i;
        for (i = 0; i < 18; i++) {
            if (strcmp(item_name, choices_array[i]) == 0) {
                break;
            }
        }
        json_object_clear(json_array_get(weather_json_struct, i));
    }
    header_reply(newsock, connection, 0, NULL, 0);
    return EXIT_SUCCESS;
}

int put_request(int newsock, char *path, char *connection, char *body ) {
    int i, j,flag_not_found=0;
    json_error_t error;
    json_t *new_obj = json_loads(body, 0, &error);
    if (!new_obj) {
        fprintf(stderr, "error on line %d %s\n", error.line, error.text);
        exit(0);
    }
    if (!strcmp(path, "/items")) {
        if (!json_is_array(new_obj)) {
            fprintf(stderr, "json file is not a list\n");
            exit(0);
        }
        if (json_array_size(new_obj) > 18 || json_array_size(new_obj) <= 0) {
            fprintf(stderr, "json has not enough or more than 18 items\n");
            exit(0);
        }
        for (i = 0; i < json_array_size(new_obj); i++) {
            const char *tok = json_string_value(json_object_get(json_array_get(new_obj, i), "link"));
            char *last = strrchr(tok, '/');
            last++;
            for (j = 0; j < 18; j++) {
                if (strcmp(last, choices_array[j]) == 0) {
                    flag_not_found++;
                    json_object_clear(json_array_get(weather_json_struct, j));
                    json_object_update(json_array_get(weather_json_struct, j), json_array_get(new_obj,i));
                    break;
                }
            }
        }
        printf("%s\n",json_dumps(weather_json_struct, JSON_ENSURE_ASCII) );
    }
    else if (!strncmp("/items/", path, 7)) {
        char *item_name = &path[7];
        for (i = 0; i < 18; i++) {
            if (strcmp(item_name, choices_array[i]) == 0) {
                break;
            }
        }
        json_object_clear(json_array_get(weather_json_struct, i));
        json_object_update(json_array_get(weather_json_struct, i), new_obj);
        //printf("%s\n",json_dumps(weather_json_struct, JSON_ENSURE_ASCII) );
    }
    header_reply(newsock, connection, 0, NULL, 0);
    return EXIT_SUCCESS;
}

int main(int argc, char *argv[]) { /* Server with Internet stream sockets */
    if (get_weather_data() == EXIT_FAILURE) {
        printf("Error in get_weather_data\n");
    }

    /*if (read_config("./config.txt") == EXIT_FAILURE){
        printf("Error in read_config\n");
    }*/

//    char *a, *b, *c, *d = NULL;
//    parse("GET path HTTP/1.1\r\nHost: www.example.com\r\nTest: value\r\nConnection: close\r\n\r\nThis is the body", &a, &b, &c, &d );
//
//     printf("a=%s\nb=%s\nc=%s\nd=%s\n",a,b,c,d);
    // sleep(3000)

    int sock, newsock, serverlen, yes = 1; // clientlen;
    socklen_t clientlen;
    char buf[256000];
    struct sockaddr_in server, client;
    struct sockaddr *serverptr, *clientptr;
    //struct hostent *rem;

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
        char *body = NULL;
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
                printf("BUF: %s\n\n", buf);
                parse(buf, &method, &path, &connection, &body);
                printf("Method: %s\n", method);
                printf("Path: %s\n", path);
                printf("Connection: %s\n", connection);
                printf("Body: %s\n", body);
                bzero(buf, sizeof buf);
                int send = 0;
                if (method_exist(method) == EXIT_FAILURE) {
                    strcpy(buf, NOT_IMPLEMENTED);
                    send = 1;
                }
                else if (path_exist(path) == EXIT_SUCCESS) {
                    if (!strcmp(method, "GET")) {
                        get_request(newsock, path, connection, 0);
                    }
                    else if (!strcmp(method, "HEAD")) {
                        int length = 0;
                        length = get_request(newsock, path, connection, 1);
                        header_reply(newsock, connection, length, NULL, 0);
                    }
                    else if (!strcmp(method, "PUT")) {
                        //char *body = "[{\"link\": \"http://myserver.org:8080/items/station_id\", \"state\": \"15000\", \"stateDescription\": {\"pattern\": \"%s\", \"readonly\": true, \"options\": []}, \"editable\": true, \"type\": \"String\", \"name\": \"WeatherAndForecast_Station_StationId\", \"label\": \"Station Id\", \"tags\": [], \"groupNames\": []}]";
                        put_request(newsock, path, connection, body);
                    }
                    else if (!strcmp(method, "DELETE")) {
                        delete_request(newsock, path, connection);
                    }
                }
                else {
                    strcpy(buf, NOT_FOUND);
                    send = 1;
                }
                if (send && write(newsock, buf, sizeof buf) < 0) { /* Send message */
                    perror("write");
                    exit(1);
                }
            } while (strcmp(connection, "close") != 0); /* Finish on "end" */
        }
        close(newsock);
        printf("Connection from somwone is closed\n" ); //rem -> h_name
        exit(0);
        }
    }
}
