/* File: int_str_server.c */
#include "server.h"

//api.openweathermap.org//data/2.5/weather?q=Nicosia,cy&APPID=64e92529c453f7621bd77a0948526d55

int read_config(char *filename) { /* Parse the configuration file */
    FILE *config_file;
    char buf[256];
    if ((config_file = fopen(filename, "r")) == NULL) {
        perror("fopen");
        return EXIT_FAILURE;
    }

    char *argument, *equals, *value;
    while (fgets(buf, 255, config_file) != NULL) {
        if ((equals = strchr(buf, '=')) != NULL) {
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
            }
            if (strcmp(argument, "DURATION") == 0) {
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
                LOCATION[strlen(LOCATION) - 1] = '\0';
            }
            if (strcmp(argument, "API_KEY") == 0) {
                if ((API_KEY = (char *) calloc((strlen(value) + 1), sizeof(char))) == NULL) {
                    perror("malloc API_KEY");
                    return EXIT_FAILURE;
                }
                strncpy(API_KEY, value, strlen(value));
                API_KEY[strlen(API_KEY) - 1] = '\0';
            }
            free(argument);
            free(value);
        }
    }

    return EXIT_SUCCESS;
}

int parse(char *request, char **method, char **path, char **connection, char **body) { /* Parse the request */
    if (strcmp(request, "") == 0) {
        printf("ERROR: Empty request\n");
        return EXIT_FAILURE;
    }

    //find the beggining of the path in the request, after the first space
    const char *start_of_path = strchr(request, ' ') + 1;

    if ((*method = (char *) calloc((start_of_path - request - 1), sizeof(char))) == NULL) {
        perror("malloc method");
        return EXIT_FAILURE;
    }
    strncpy(*method, request, start_of_path - request - 1);
    (*method)[strlen(*method)] = '\0';

    //find the end of the path at the next space
    const char *end_of_path = strchr(start_of_path, ' ');
    if ((*path = (char *) calloc((end_of_path - start_of_path + 1), sizeof(char))) == NULL) {
        perror("malloc path");
        return EXIT_FAILURE;
    }
    strncpy(*path, start_of_path, end_of_path - start_of_path);
    (*path)[strlen(*path)] = '\0';

    char *name = NULL, *value_start = NULL, *value_end = NULL, *value = NULL;
    char *next_line = strchr(end_of_path, '\n') + 1;


    //check the last two bytes to see if the header has ended
    char *last_bytes = calloc(3, sizeof(char));
    strncpy(last_bytes, next_line, 2);
    last_bytes[2] = '\0';

    int body_len = 0;

    //Parse the header arguments
    while ((strcmp(last_bytes, "\r\n") != 0) && next_line != NULL) {
        //find place of name
        value_start = strchr(next_line, ':');
        if ((name = (char *) calloc((value_start - next_line), sizeof(char))) == NULL) {
            perror("malloc name");
            return EXIT_FAILURE;
        }
        //copy name into variable
        strncpy(name, next_line, value_start - next_line);
        name[strlen(name)] = '\0';
//        printf("Name: %s\n", name);

        value_start = value_start + 2;
        value_end = strchr(value_start, '\r'); //find end of value
        if ((value = (char *) calloc((value_end - value_start), sizeof(char))) == NULL) {
            perror("malloc value");
            return EXIT_FAILURE;
        }
        //copy value into variable
        strncpy(value, value_start, value_end - value_start);
        value[strlen(value)] = '\0';
//        printf("Value: %s\n", value);

        if (strcmp(name, "Connection") == 0) { //check if header had connection field, copy into variable
            if (((*connection) = (char *) calloc((value_end - value_start), sizeof(char))) == NULL) {
                perror("malloc connection");
                return EXIT_FAILURE;
            }
            strncpy((*connection), value, strlen(value));
        }

        if (strcmp(name, "Content-Length") == 0) { //check if header had content-length field, copy into variable
            body_len = atoi(value);
        }

        free(name);
        free(value);
        next_line = value_end + 2;

        strncpy(last_bytes, next_line, 2);
        last_bytes[2] = '\0';
    }

    //check if the requests contains a body
    if (body_len != 0) {
        if ((*body = (char *) calloc( body_len + 1, sizeof(char))) == NULL) {
            perror("malloc body");
            return EXIT_FAILURE;
        }
        strncpy(*body, next_line + 2, body_len);
        (*body)[strlen(*body)] = '\0';
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

    bcopy((char *) rem->h_addr, (char *) &server.sin_addr, rem->h_length);
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

    //connect to openweathermap
    if ((connect_socket(OPENWEATHERMAP_SERVER, OPENWEATHERMAP_PORT, &sock1)) == EXIT_FAILURE) {
        printf("Error in connect socket\n");
        return EXIT_FAILURE;
    }

    bzero(buf, sizeof buf); /* Initialize buffer */
    sprintf(buf, OPENWEATHERMAP_GET, "weather", LOCATION, API_KEY); //get info for current weather
    if (write(sock1, buf, sizeof buf) < 0) {
        perror("write");
        return EXIT_FAILURE;
    }
    char current_json_string[1000];
    bzero(current_json_string, sizeof(current_json_string));
    bzero(buf, sizeof buf); /* Initialize buffer */
    while ((res = read(sock1, buf, sizeof(buf))) > 0) {
        if (res < 0) {
            perror("read openweather");
            return EXIT_FAILURE;
        }
        strncat(current_json_string, buf, sizeof(buf));
        bzero(buf, sizeof buf); /* Initialize buffer */
    }

    //printf("%s\n", current_json_string);
    close(sock1);

    if ((connect_socket(OPENWEATHERMAP_SERVER, OPENWEATHERMAP_PORT, &sock2)) == EXIT_FAILURE) {
        printf("Error in connect socket\n");
        return EXIT_FAILURE;
    }
    bzero(buf, sizeof buf); /* Initialize buffer */
    sprintf(buf, OPENWEATHERMAP_GET, "forecast", LOCATION, API_KEY, "close");  //get info for forecast

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

void *serve_client() {
    char buf[256000];
    char *method = NULL;
    char *path = NULL;
    char *connection = NULL;
    char *body = NULL;
    int current_write = 0;

    while (1){
        //lock when getting the socket so that only one thread gets the socket
        pthread_mutex_lock(&thread_lock);
        pthread_cond_wait(&client_ready, &change_work);
        int newsock = new_socket;
        pthread_mutex_unlock(&thread_lock);
        int read_bytes;

        do {
            if(current_write == 0)
               bzero(buf, sizeof buf); /* Initialize buffer */
            if ( (read_bytes = read(newsock, buf + current_write, sizeof buf)) < 0) {  /* Receive message */
                perror("read thread");
                return NULL;
            }
            //we check in case read function has not read the whole request
            //If it hasn't, continue so read can get the rest of the request, else pass buffer to parse function
            if (strstr(buf, "\r\n\r\n") == NULL){
                //if the whole request wasn't read, save a counter so we know where the rest of the request will be appended
                current_write += read_bytes;
                continue;
            }else{
                current_write = 0;
            }
//            while (read(newsock, buf, sizeof buf) == 0){}
//            printf("BUF: %s\n\n", buf);
            parse(buf, &method, &path, &connection, &body);
            if (connection == NULL) { //if connection: close field doesn't exist, it means that connection is keep-alive
                connection = "keep-alive";
            }
            printf("Method: %s\n", method);
            printf("Path: %s\n", path);
            printf("Connection: %s\n", connection);
            printf("Body: %s\n", body);
            bzero(buf, sizeof buf);
            int send = 0, rep = 0;
            // First check if method exist
            if (method_exist(method) == EXIT_FAILURE) {
                strncpy(buf, NOT_IMPLEMENTED, strlen(NOT_IMPLEMENTED));
                send = 1;   // Send the reply 
                // If path exist handle the request for each option
            } else if (path_exist(path) == EXIT_SUCCESS) {
                // If GET request send from client
                if (strcmp(method, "GET") == 0) {
                    get_request(newsock, path, connection, 0);
                    // If HEAD request send from client
                } else if (!strcmp(method, "HEAD")) {
                    int length = 0;
                    length = get_request(newsock, path, connection, 1);
                    if (length != EXIT_FAILURE)
                        header_reply(newsock, connection, length, NULL, 0);
                    // If PUT request send from client
                } else if (!strcmp(method, "PUT")) {
                    //char *body = "[{\"link\": \"http://myserver.org:8080/items/station_id\", \"state\": \"15000\", \"stateDescription\": {\"pattern\": \"%s\", \"readonly\": true, \"options\": []}, \"editable\": true, \"type\": \"String\", \"name\": \"WeatherAndForecast_Station_StationId\", \"label\": \"Station Id\", \"tags\": [], \"groupNames\": []}]";
                    rep = put_request(newsock, path, connection, body);
                    if (rep == 2) {
                        item_not_found_reply(newsock);
                    }
                    // If DELETE request send from client
                } else if (!strcmp(method, "DELETE")) {
                    delete_request(newsock, path, connection);
                }
                // If method not implemented
            } else {
                strncpy(buf, NOT_FOUND, strlen(NOT_FOUND));
                send = 1;   // Send the reply 
            }
            // Send the reply if Path not found or Method not Implemented
            if (send && write(newsock, buf, sizeof buf) < 0) {
                perror("write");
                return NULL;
            }
        } while (strcmp(connection, "close") != 0); /* Finish on "end" */
        close(newsock);
//        printf("Connection from someone is closed\n" ); //rem -> h_name

        free(method);
        free(path);
        free(connection);
        free(body);

        //lock when decreasing available work value so that there is not a race condition
        pthread_mutex_lock(&change_work);
        printf("Thread finished\n");
        available_work--;
        pthread_mutex_unlock(&change_work);
    }

}

void signal_handler() {
    //get data from openweater when an alarm is sent
    printf("RETRIEVING DATA...\n");
    if (get_weather_data() == EXIT_FAILURE) {
        printf("Error in get_weather_data\n");
    }
    alarm(DURATION);
}

int main(int argc, char *argv[]) { /* Server with Internet stream sockets */
    pthread_t *threads;
    int i;

    //read configuration file
    if (read_config("config.txt") == EXIT_FAILURE){
        printf("Error in read_config\n");
    }

    //allocate space for the threads
    if ((threads = malloc(THREADS * sizeof(pthread_t))) == NULL) {
        perror("malloc threads");
        exit(1);
    }

    //create a thread pool
    for (i = 0; i < THREADS; i++) {
        pthread_create(&threads[i], NULL, serve_client, NULL);
    }

    //get data from openweather
    if (get_weather_data() == EXIT_FAILURE) {
        printf("Error in get_weather_data\n");
    }
    signal(SIGALRM, signal_handler);
    alarm(DURATION); //alarm every DURATION seconds so we get new data from openweather

    int sock, serverlen, yes = 1; // clientlen;
    socklen_t clientlen;
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
        if ((new_socket = accept(sock, clientptr, &clientlen)) < 0) {
            perror("accept");
            exit(1);
        } /* Accept connection */

        //lock when changing available work variable so that there is not a race condition
        pthread_mutex_lock(&change_work);
        available_work++;
//        printf("Available work: %d\n", available_work);
        //if there are available thread, signal a thread to handle the request
        if (available_work <= THREADS)
            pthread_cond_signal(&client_ready);
        else {
            //reject connection if there are no available threads
            available_work--;
            printf("Refused connection\n");
            close(new_socket);
        }
        pthread_mutex_unlock(&change_work);

    }
}
