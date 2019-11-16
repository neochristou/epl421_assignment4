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

char *API_KEY = "64e92529c453f7621bd77a0948526d55";
char *LOCATION = "Nicosia,cy";
int THREADS;
int PORT;
int DURATION;
const char *OPENWEATHERMAP_SERVER = "api.openweathermap.org";
int OPENWEATHERMAP_PORT = 80;
const char* OPENWEATHERMAP_GET = "GET /data/2.5/%s?q=%s&APPID=%s HTTP/1.1\nHost: api.openweathermap.org\nUser-Agent: myOpenHAB\nAccept: application/json\nConnection: close\n\n";

int connect_socket(const char *server_name, int port, int *sock){
    int serverlen;
    struct sockaddr_in server;
    struct sockaddr *serverptr;
    struct hostent *rem;

    if ((*sock = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    { /* Create socket */
        perror("socket");
        return EXIT_FAILURE;
    }


    if ((rem = gethostbyname(server_name)) == NULL) {
        perror("gethostbyname");
        return EXIT_FAILURE;
    }

    bcopy((char *) rem -> h_addr, (char *) &server.sin_addr,rem -> h_length);
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

int get_weather_data(){

    char buf[256];
    int res;
    int sock1, sock2;

    if ( (connect_socket(OPENWEATHERMAP_SERVER, OPENWEATHERMAP_PORT, &sock1)) == EXIT_FAILURE){
        printf("Error in connect socket\n");
        return  EXIT_FAILURE;
    }

    /*Get current weather data and save to json file*/
    int file_current = open("../current.json", O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (file_current < 0)
    {
        perror("opening file");
        return EXIT_FAILURE;
    }
    bzero(buf, sizeof buf); /* Initialize buffer */
    sprintf(buf ,OPENWEATHERMAP_GET, "weather", LOCATION, API_KEY);
    if (write(sock1, buf, sizeof buf) < 0)
    { /* Send message */
        perror("write");
        return EXIT_FAILURE;
    }
    bzero(buf, sizeof buf); /* Initialize buffer */
    while ((res = read(sock1, buf, sizeof buf)) > 0){
        if (res < 0){
            perror("read");
            return EXIT_FAILURE;
        }
        if ( write(file_current, buf, strlen(buf)) != strlen(buf) ){
            perror ("write error");
            return EXIT_FAILURE;
        }
        bzero(buf, sizeof buf); /* Initialize buffer */
    }
    if (close(file_current) < 0){
        perror("Closing file:");
        return EXIT_FAILURE;
    }
    close(sock1);

    if ( (connect_socket(OPENWEATHERMAP_SERVER, OPENWEATHERMAP_PORT, &sock2)) == EXIT_FAILURE){
        printf("Error in connect socket\n");
        return  EXIT_FAILURE;
    }
    /*Get forecast weather data and save to json file*/
    int file_forecast = open("../forecast.json", O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (file_forecast < 0)
    {
        perror("opening file");
        return EXIT_FAILURE;
    }
    bzero(buf, sizeof buf); /* Initialize buffer */
    sprintf(buf, OPENWEATHERMAP_GET, "forecast", LOCATION, API_KEY, "close");

    if (write(sock2, buf, sizeof buf) < 0)
    { /* Send message */
        perror("write");
        return EXIT_FAILURE;
    }
    bzero(buf, sizeof buf); /* Initialize buffer */
    while ((res = read(sock2, buf, sizeof buf)) > 0){
        if (res < 0){
            perror("read");
            return EXIT_FAILURE;
        }
        if ( write(file_forecast, buf, strlen(buf)) != strlen(buf) ){
            perror ("write error");
            return EXIT_FAILURE;
        }
        bzero(buf, sizeof buf); /* Initialize buffer */
    }
    if (close(file_forecast) < 0){
        perror("Closing file:");
        return EXIT_FAILURE;
    }
    close(sock2);

    return EXIT_SUCCESS;
}

int main(int argc, char *argv[]) /* Server with Internet stream sockets */
{
    get_weather_data();

    int port, sock, newsock, serverlen;//, clientlen;
    socklen_t clientlen;
    char buf[256];
    struct sockaddr_in server, client;
    struct sockaddr *serverptr, *clientptr;
    struct hostent *rem;

    if (argc < 2)
    { /* Check if server's port number is given */
        printf("Please give the port number\n");
        exit(1);
    }

    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    { /* Create socket */
        perror("socket");
        exit(1);
    }

    port = atoi(argv[1]); /* Convert port number to integer */
    server.sin_family = AF_INET; /* Internet domain */
    server.sin_addr.s_addr = htonl(INADDR_ANY); /* My Internet address */
    server.sin_port = htons(port); /* The given port */
    serverptr = (struct sockaddr *) &server;
    serverlen = sizeof server;

    if (bind(sock, serverptr, serverlen) < 0)
    { /* Bind socket to an address */
        perror("bind");
        exit(1);
    }
    if (listen(sock, 5) < 0)
    { /* Listen for connections */
        perror("listen");
        exit(1);
    }

    printf("Listening for connections to port %d\n", port);

    while(1)
    {
        clientptr = (struct sockaddr *) &client;
        clientlen = sizeof client;
        if ((newsock = accept(sock, clientptr, &clientlen)) < 0)
        {
            perror("accept");
            exit(1);
        } /* Accept connection */

        //reverse DNS
        rem = gethostbyaddr((char *) &client.sin_addr.s_addr, sizeof client.sin_addr.s_addr, /* Find client's address */client.sin_family);
        if (rem == NULL)
        {
            perror("gethostbyaddr");
            exit(1);
        }

        printf("Accepted connection from %s\n", rem -> h_name);

        switch (fork())
        { /* Create child for serving the client */
            case -1:
            {
                perror("fork");
                exit(1);
            }
            case 0: /* Child process */
            {
                do {
                    bzero(buf, sizeof buf); /* Initialize buffer */
                    if (read(newsock, buf, sizeof buf) < 0)
                    { /* Receive message */
                        perror("read");
                        exit(1);
                    }
                    printf("Read string: %s\n", buf);

                    bzero(buf, sizeof buf);

                    sprintf(buf, "Reply\n");
                    if (write(newsock, buf, sizeof buf) < 0){ /* Send message */
                        perror("write");
                        exit(1);
                    }
                } while (1); /* Finish on "end" */
            }
                close(newsock); /* Close socket */
                printf("Connection from %s is closed\n", rem -> h_name);
                exit(0);
        }
    }
}
