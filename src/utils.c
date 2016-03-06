#include "ed.h"
#include <stdlib.h>
#include <string.h>

void strstrip(char *str)
{
	if (strlen(str) == 0) {
		return;
	}

	size_t len = strlen(str);
	if (str[len - 1] == '\n') {
		str[len - 1] = '\0';
	}
}

void clrstr(char *str)
{
	if (strlen(str) == 0) {
		return;
	}

	while (*str != '\0') {
		*str == '\0';
		str++;
	}
}

char *charalloc(size_t len)
{
	char *buffer;

	buffer = malloc((len + 1) * sizeof(char));
	if (buffer == NULL) {
		perror(NULL);
		exit(EXIT_FAILURE);
	}
	buffer[0] = '\0';

	return buffer;
}

char *charrealloc(char *buffer, size_t len)
{
	char *new_buffer;

	new_buffer = realloc(buffer, len);
	if (new_buffer == NULL) {
		perror(NULL);
		exit(EXIT_FAILURE);
	}

	return new_buffer;
}
