#ifndef MYJSON_H
#define MYJSON_H

#include <stdlib.h>
#include <string.h>
#include <jansson.h>
#include <stdio.h>

json_t *createJsonStruct(char *, char *);
char * JsonHeaderRemover(char * );

#endif //EPL421_AS4_SERVER_H