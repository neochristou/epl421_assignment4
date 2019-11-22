#include <sys/types.h> // For sockets
#include <sys/socket.h> // For sockets
#include <netinet/in.h> // For Internet sockets
#include <netdb.h> // For gethostbyaddr
#include <stdio.h> // For I/O
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>

int PORT = 2000;
char *URL = "172.20.234.101";

int connect_socket(const char *server_name, int port, int *sock) {
    int serverlen;
    struct sockaddr_in server;
    struct sockaddr *serverptr;
    struct hostent *rem;

    if ((*sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) { /* Create socket */
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

int main() {

    char buf[25600];

    int res;
    int sock;

    if (connect_socket(URL, PORT, &sock) == EXIT_SUCCESS) {
        printf("Requested connection to host %s port %d\n", URL, PORT);
    }
    else {
        printf("Connection has not granded\n");
        return -1;
    }

    bzero(buf, sizeof buf); /* Initialize buffer */

    strcpy(buf, "GET /items HTTP/1.1\r\nHost: nicolas\r\nConnection: close\r\n\r\n"); //User-Agent: cpanta02Web\r\nAccept: application/json\r\n
    // printf("%li\n", sizeof(buf) );
    // sleep(3000);

    if (write(sock, buf, sizeof(buf)) < 0) { /* Send message */
        perror("write"); exit(1);
    }

    bzero(buf, sizeof(buf)); /* Initialize buffer */

    if (read(sock, buf, sizeof(buf)) < 0) { /* Receive message */
        perror("read"); exit(1);
    }
    printf("\n%s\n", buf);

    while ( read(sock, buf, sizeof(buf)) < 0) {
        
        printf("%s\n",buf );
        bzero(buf, sizeof buf); /* Initialize buffer */
    }
   
    return 1;
}