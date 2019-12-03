#ifndef MYJSON_H
#define MYJSON_H

#include <stdlib.h>
#include <string.h>
#include <jansson.h>
#include <stdio.h>

const char *choices_array[18];

extern json_t *weather_json_struct;

/*Take as input 2 json strings:
	First:  Current Weather json
	Second: Forecast Weather
After extracting the necessary values of the items hat we need, it creates our own json struct dynamically.
The function returns the pointer of that json struct.*/
json_t *createJsonStruct(char *, char *);

/*Take as an input the whole response of a GET request (Header + Body).
  Then it extract the body from the header and return only the body as a string*/
char * JsonHeaderRemover(char * );

#endif