#include "requests.h"

// Replies from server to client according to Connection, Method, Path
const char *NOT_IMPLEMENTED = "HTTP/1.1 501 Not Implemented\r\nServer: my_webserver\r\nConnection: close\r\nContent-Type: text/plain\r\nContent-Length: 24\r\n\r\nMethod not implemented!";
const char *REPLY_OK_CLOSE = "HTTP/1.1 200 OK\r\nServer: my_webserver\r\nConnection: close\r\nContent-Type: application/json\r\nContent-Length: ";
const char *REPLY_OK_ALIVE = "HTTP/1.1 200 OK\r\nServer: my_webserver\r\nConnection: keep-alive\r\nContent-Type: application/json\r\nContent-Length: ";
const char *NOT_FOUND = "HTTP/1.1 404 Not Found\r\nServer: my_webserver\r\nConnection: close\r\nContent-Type: text/plain\r\nContent-Length: 16\r\n\r\nPath not found!";
const char *ITEM_NOT_FOUND = "HTTP/1.1 404 Not Found\r\nServer: my_webserver\r\nConnection: close\r\nContent-Type: text/plain\r\nContent-Length: 16\r\n\r\nItem not found!";

// If item does not exist (18 pre made items) send reply 404_NOT_FOUND
int item_not_found_reply(int newsock) {
    char buf[256];
    bzero(buf, sizeof(buf));
    strncpy(buf, ITEM_NOT_FOUND, strlen(ITEM_NOT_FOUND)); // Copy the message to the buffer
    if (write(newsock, buf, sizeof buf) < 0) {            // Write the message to the socket
        perror("write");
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}

// Reply according to Connection (close/keep-alive) and body with estimated content-length
int header_reply(int newsock, char *connection, int content_length, char *temp_buf, int body_reply) {
    char buf[25600];
    char content_header[10];
    sprintf(content_header, "%d\r\n\r\n", (content_length)); // Prepare Content-Length reply header
    bzero(buf, sizeof buf);
    if (!strcmp(connection, "close"))
        strncpy(buf, REPLY_OK_CLOSE, strlen(REPLY_OK_CLOSE)); // If Connection is close
    else
        strncpy(buf, REPLY_OK_ALIVE, strlen(REPLY_OK_ALIVE)); // If Connection is keep-alive
    strcat(buf, content_header);                              // Concatenate the header with Content-Length header
    if (body_reply)
        strcat(buf, temp_buf);                      // If true add the body to the reply
    if (write(newsock, buf, strlen(buf)) < 0) {     // Write the message to the socket
        perror("write");
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}

// Check if the Item exist (18 pre made items)
int item_exists(char *item) {
    if (!strcmp(item, "station_id") || !strcmp(item, "current_time") || !strcmp(item, "current_temp") ||
            !strcmp(item, "current_pressure") || !strcmp(item, "current_humidity") || !strcmp(item, "current_speed") ||
            !strcmp(item, "current_cloudiness") || !strcmp(item, "current_rain") || !strcmp(item, "current_sunrise") ||
            !strcmp(item, "current_sunset") || !strcmp(item, "forecast3_time") || !strcmp(item, "forecast3_temp") ||
            !strcmp(item, "forecast6_time") || !strcmp(item, "forecast6_temp") || !strcmp(item, "forecast9_time") ||
            !strcmp(item, "forecast9_temp") || !strcmp(item, "forecast12_time") || !strcmp(item, "forecast12_temp")) {
        return EXIT_SUCCESS;  // Exist
    }
    return EXIT_FAILURE;      // Not Exist
}

int path_exist(char *path) {
    if (!strcmp(path, "/items")) {
        return EXIT_SUCCESS;
    } else if (!strncmp("/items/", path, 7)) {
        char *item_name = &path[7];
        if (item_exists(item_name) == EXIT_SUCCESS) {
            return EXIT_SUCCESS;
        }
    }
    return EXIT_FAILURE;
}

// Check if the method used from client exist (4 pre made Methods)
int method_exist(char *method) {
    if (!strcmp(method, "GET") || !strcmp(method, "PUT") || !strcmp(method, "DELETE") || !strcmp(method, "HEAD")) {
        return EXIT_SUCCESS; // Exist
    }
    return EXIT_FAILURE;     // Not Exist
}

// Handle GET request (/items or /items/{itemname})
int get_request(int newsock, char *path, char *connection, int find_length) {
    char *temp_buf = malloc(sizeof(char) * 100000);
    bzero(temp_buf, 100000);
    int i, count = 0;
    // If client request to get All Items
    if (!strcmp(path, "/items")) {
        strcat(temp_buf, "[");
        // For each item check if exist and create the array of existing Items
        for (i = 0; i < 18; i++) {
            char *temp_buf2 = json_dumps(json_array_get(weather_json_struct, i), JSON_ENSURE_ASCII);
            if (strcmp(temp_buf2, "{}")) {
                strcat(temp_buf, temp_buf2);
                strcat(temp_buf, ",");
                count++;
            }
        }
        // Close the array
        temp_buf[strlen(temp_buf) - 1] = ']';
        // If No Items found reply with 404_NOT_FOUND
        if (count == 0) {
            item_not_found_reply(newsock);
            free(temp_buf);
            return EXIT_FAILURE;
        }

        // If client request to get One Item
    } else if (!strncmp("/items/", path, 7)) {
        char *item_name = &path[7];     // Isolate the itemname
        // For each item find the position in the array of existing Items
        for (i = 0; i < 18; i++) {
            if (strcmp(item_name, choices_array[i]) == 0) {
                break;
            }
        }
        //json_object_clear(json_array_get(weather_json_struct, i));
        temp_buf = json_dumps(json_array_get(weather_json_struct, i), JSON_ENSURE_ASCII);

        // If No Item found reply with 404_NOT_FOUND
        if (!strcmp(temp_buf, "{}")/*strlen(temp_buf)==2*/) {
            item_not_found_reply(newsock);
            free(temp_buf);
            return EXIT_FAILURE;
        }
    }
    if (find_length) {                  // If true just return the Content-Length(size of Body)
        int len = strlen(temp_buf);
        free(temp_buf);
        return len;
    }
    // Sent the reply with Content-Length and Body of Items/Item
    header_reply(newsock, connection, strlen(temp_buf), temp_buf, 1);
    free(temp_buf);
    return EXIT_SUCCESS;
}

// Handle DELETE request (/items or /items/{itemname})
int delete_request(int newsock, char *path, char *connection) {
    int i;
    // If client request to delete All Items
    if (!strcmp(path, "/items")) {
        for (i = 0; i < 18; i++) {
            // Delete the item from array (Make it {})
            json_object_clear(json_array_get(weather_json_struct, i));
        }
        // If client request to delete One Item
    } else if (!strncmp("/items/", path, 7)) {
        char *item_name = &path[7];     // Isolate the itemname
        // For each item find the position in the array of existing Items
        for (i = 0; i < 18; i++) {
            if (strcmp(item_name, choices_array[i]) == 0) {
                break;
            }
        }
        // Delete the item from array (Make it {})
        json_object_clear(json_array_get(weather_json_struct, i));
    }
    // Sent the reply with success
    header_reply(newsock, connection, 0, NULL, 0);
    return EXIT_SUCCESS;
}

// Handle PUT request (/items or /items/{itemname})
int put_request(int newsock, char *path, char *connection, char *body) {
    int i, j;
    json_error_t error;
    //printf("BODY:\n%s", body);
    json_t *new_obj = json_loads(body, 0, &error);      // Load the json from the body
    // If Body not in json format reply with error
    if (!new_obj) {
        fprintf(stderr, "error on line %d %s\n", error.line, error.text);
        return 2;
    }
    // If client request to put All Items
    if (!strcmp(path, "/items")) {
        // If Body json is not an array reply with 404_NOT_FOUND
        if (!json_is_array(new_obj)) {
            fprintf(stderr, "json file is not a list\n");
            return 2;
        }
        // If items given in the array is not correct reply with 404_NOT_FOUND
        if (json_array_size(new_obj) > 18 || json_array_size(new_obj) <= 0) {
            fprintf(stderr, "json has not enough or more than 18 items\n");
            return 2;
        }
        // First check if all items exist
        for (i = 0; i < json_array_size(new_obj); i++) {
            // Isolate the name in the link field
            const char *tok = json_string_value(json_object_get(json_array_get(new_obj, i), "link"));
            char *last = strrchr(tok, '/');
            last++;
            int found = 0;
            // For each item find the position in the array of existing Items
            for (j = 0; j < 18; j++) {
                if (strcmp(last, choices_array[j]) == 0) {
                    found = 1;
                    break;
                }
            }
            // If any item not found reply with 404_NOT_FOUND
            if (!found) {
                return 2;
            }
        }
        // After check done that items exist update with the new items
        for (i = 0; i < json_array_size(new_obj); i++) {
            // Isolate the name in the link field
            const char *tok = json_string_value(json_object_get(json_array_get(new_obj, i), "link"));
            char *last = strrchr(tok, '/');
            last++;
            int found = 0;
            // For each item find the position in the array of existing Items
            for (j = 0; j < 18; j++) {
                if (strcmp(last, choices_array[j]) == 0) {
                    // Delete the old item
                    json_object_clear(json_array_get(weather_json_struct, j));
                    // Update with the new item
                    json_object_update(json_array_get(weather_json_struct, j), json_array_get(new_obj, i));
                    found = 1;
                    break;
                }
            }
        }
        //printf("%s\n", json_dumps(weather_json_struct, JSON_ENSURE_ASCII));
        // If client request to put one Items
    } else if (!strncmp("/items/", path, 7)) {
        char *item_name = &path[7];              // Isolate the itemname
        // For each item find the position in the array of existing Items
        for (i = 0; i < 18; i++) {
            if (strcmp(item_name, choices_array[i]) == 0) {
                break;
            }
        }
        // Isolate the name in the link field
        const char *tok = json_string_value(json_object_get(new_obj, "link"));
        char *last = strrchr(tok, '/');
        last++;
        // If the itemname is corrent in the Body json
        if (strcmp(last, choices_array[i]) == 0) {
            // Delete the old item
            json_object_clear(json_array_get(weather_json_struct, i));
            // Update with the new item
            json_object_update(json_array_get(weather_json_struct, i), new_obj);
        }
        else {
            return 2;
        }
        //printf("%s\n",json_dumps(weather_json_struct, JSON_ENSURE_ASCII) );
    }
    // Sent the reply with success
    header_reply(newsock, connection, 0, NULL, 0);
    return EXIT_SUCCESS;
}

