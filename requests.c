
#include "requests.h"

const char *NOT_IMPLEMENTED = "HTTP/1.1 501 Not Implemented\r\nServer: my_webserver\r\nConnection: close\r\nContent-Type: text/plain\r\nContent-Length: 24\r\n\r\nMethod not implemented!";
const char *REPLY_OK_CLOSE = "HTTP/1.1 200 OK\r\nServer: my_webserver\r\nConnection: close\r\nContent-Type: application/json\r\nContent-Length: ";
const char *REPLY_OK_ALIVE = "HTTP/1.1 200 OK\r\nServer: my_webserver\r\nConnection: keep-alive\r\nContent-Type: application/json\r\nContent-Length: ";
const char *NOT_FOUND = "HTTP/1.1 404 Not Found\r\nServer: my_webserver\r\nConnection: close\r\nContent-Type: text/plain\r\nContent-Length: 16\r\n\r\nPath not found!";
const char *ITEM_NOT_FOUND = "HTTP/1.1 404 Not Found\r\nServer: my_webserver\r\nConnection: close\r\nContent-Type: text/plain\r\nContent-Length: 16\r\n\r\nItem not found!";


int item_not_found_reply(int newsock) {
    char buf[256];
    bzero(buf, sizeof(buf));
    strncpy(buf, ITEM_NOT_FOUND, strlen(ITEM_NOT_FOUND));
    if (write(newsock, buf, sizeof buf) < 0) {
        perror("write");
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}

int header_reply(int newsock, char *connection, int content_length, char *temp_buf, int body_reply) {
    char buf[25600];
    char content_header[10];
    sprintf(content_header, "%d\r\n\r\n", (content_length));
    bzero(buf, sizeof buf);
    if (!strcmp(connection, "close"))
        strncpy(buf, REPLY_OK_CLOSE, strlen(REPLY_OK_CLOSE));
    else
        strncpy(buf, REPLY_OK_ALIVE, strlen(REPLY_OK_ALIVE));
    strcat(buf, content_header);
    if (body_reply)
        strcat(buf, temp_buf);
    if (write(newsock, buf, strlen(buf)) < 0) {
        perror("write");
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}

int item_exists(char *item) {
    if (!strcmp(item, "station_id") || !strcmp(item, "current_time") || !strcmp(item, "current_temp") ||
        !strcmp(item, "current_pressure") || !strcmp(item, "current_humidity") || !strcmp(item, "current_speed") ||
        !strcmp(item, "current_cloudiness") || !strcmp(item, "current_rain") || !strcmp(item, "current_sunrise") ||
        !strcmp(item, "current_sunset") || !strcmp(item, "forecast3_time") || !strcmp(item, "forecast3_temp") ||
        !strcmp(item, "forecast6_time") || !strcmp(item, "forecast6_temp") || !strcmp(item, "forecast9_time") ||
        !strcmp(item, "forecast9_temp") || !strcmp(item, "forecast12_time") || !strcmp(item, "forecast12_temp")) {
        return EXIT_SUCCESS;
    }
    return EXIT_FAILURE;
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
        strcat(temp_buf, "[");
        for (i = 0; i < 18; i++) {
            char *temp_buf2 = json_dumps(json_array_get(weather_json_struct, i), JSON_ENSURE_ASCII);
            if (strcmp(temp_buf2, "{}")) {
                strcat(temp_buf, temp_buf2);
                strcat(temp_buf, ",");
                count++;
            }
        }

        temp_buf[strlen(temp_buf)-1]=']';

        if (count == 0) {
            item_not_found_reply(newsock);
            free(temp_buf);
            return EXIT_FAILURE;
        }
    } else if (!strncmp("/items/", path, 7)) {
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
        int len = strlen(temp_buf);
        free(temp_buf);
        return len;
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
    } else if (!strncmp("/items/", path, 7)) {
        char *item_name = &path[7];
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

int put_request(int newsock, char *path, char *connection, char *body) {
    int i, j;
    json_error_t error;
    //printf("BODY:\n%s", body);
    json_t *new_obj = json_loads(body, 0, &error);
    if (!new_obj) {
        fprintf(stderr, "error on line %d %s\n", error.line, error.text);
        return EXIT_FAILURE;
    }
    if (!strcmp(path, "/items")) {
        if (!json_is_array(new_obj)) {
            fprintf(stderr, "json file is not a list\n");
            return 2;
        }
        if (json_array_size(new_obj) > 18 || json_array_size(new_obj) <= 0) {
            fprintf(stderr, "json has not enough or more than 18 items\n");
            return 2;
        }
        for (i = 0; i < json_array_size(new_obj); i++) {
            const char *tok = json_string_value(json_object_get(json_array_get(new_obj, i), "link"));
            char *last = strrchr(tok, '/');
            last++;
            int found = 0;
            for (j = 0; j < 18; j++) {
                if (strcmp(last, choices_array[j]) == 0) {
                    found = 1;
                    break;
                }
            }
            if (!found) {
                return 2;
            }
        }
        for (i = 0; i < json_array_size(new_obj); i++) {
            const char *tok = json_string_value(json_object_get(json_array_get(new_obj, i), "link"));
            char *last = strrchr(tok, '/');
            last++;
            int found = 0;
            for (j = 0; j < 18; j++) {
                if (strcmp(last, choices_array[j]) == 0) {
                    json_object_clear(json_array_get(weather_json_struct, j));
                    json_object_update(json_array_get(weather_json_struct, j), json_array_get(new_obj, i));
                    found=1;
                    break;
                }
            }
            if (!found) {
                return 2;
            }
        }
        //printf("%s\n", json_dumps(weather_json_struct, JSON_ENSURE_ASCII));
    } else if (!strncmp("/items/", path, 7)) {
        char *item_name = &path[7];
        for (i = 0; i < 18; i++) {
            if (strcmp(item_name, choices_array[i]) == 0) {
                break;
            }
        }
        const char *tok = json_string_value(json_object_get(new_obj, "link"));
        char *last = strrchr(tok, '/');
        last++;
        if (strcmp(last, choices_array[i]) == 0) {
            json_object_clear(json_array_get(weather_json_struct, i));
            json_object_update(json_array_get(weather_json_struct, i), new_obj);
        }
        else {
            return 2;
        }
        //printf("%s\n",json_dumps(weather_json_struct, JSON_ENSURE_ASCII) );
    }
    header_reply(newsock, connection, 0, NULL, 0);
    return EXIT_SUCCESS;
}

