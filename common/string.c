#include <stdlib.h>
#include <string.h>
#include "string.h"

#define STR_INIT_SIZE 256
#define STR_EXT_SIZE 256

struct tagString{
	char* data;
	int size;
	int index;
}; // String

String* new_string(const char* init_string)
{
	String* str = malloc(sizeof(String));
	if ( str == NULL ){
		// abort
	}
	str->size = STR_INIT_SIZE;
	str->index = strlen(init_string);
	str->data = malloc(sizeof(char) * STR_INIT_SIZE);
	strncpy(str->data, init_string, str->index + 1);
	return str;
}

void string_append(String* str, const char* hind)
{
	while ( str->index + strlen(hind) > str->size ) {
		str->data = realloc(str->data, str->size + STR_EXT_SIZE);
		if ( str->data == NULL ) {
			// abort
		}
		str->size += STR_EXT_SIZE;
	}
	strncpy(&(str->data[str->index]), hind, strlen(hind) + 1);
	str->index += strlen(hind);
}

void string_append_char(String* str, const char c)
{
	if ( str->index == str->size ) {
		str->data = realloc(str->data, str->size + STR_EXT_SIZE);
		if ( str->data == NULL ) {
			// abort
		}
		str->size += STR_EXT_SIZE;
	}
	str->data[str->index] = c;
	str->index++;
	str->data[str->index] = '\0';
}

void string_renew(String* str, const char* renew_string)
{
	delete_string(str);
	str = new_string(renew_string);
}

const char* string_c_str(String* str)
{
	return str->data;
}

int string_cmp1(String* str, const String* str2)
{
	return strncmp(str->data, str2->data, str->index);
}

int string_cmp2(String* str, const char* str2)
{
	return strncmp(str->data, str2, str->index);
}

int string_size(String* str)
{
	return str->index;
}

void delete_string(String* str)
{
	free(str->data);
	free(str);
	return;
}
