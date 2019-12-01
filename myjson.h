#ifndef MYJSON_H
#define MYJSON_H

#include <stdlib.h>
#include <string.h>
#include <jansson.h>
#include <stdio.h>

const char *choices_array[18];

extern json_t *weather_json_struct;


json_t *createJsonStruct(char *, char *);
char * JsonHeaderRemover(char * );

#endif