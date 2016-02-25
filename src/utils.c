#include <stdlib.h>
#include <string.h>

void strstrip(char *str)
{
	if (strlen(str) == 0) {
		return;
	}

	long len = strlen(str);
	if (str[len - 1] == '\n') {
		str[len - 1] = '\0';
	}
}

char *charalloc(size_t len)
{
	char *buffer;

	buffer = malloc(len * sizeof(char));
	if (buffer == NULL) {
		/* Err */
	}
	buffer[0] = '\0';

	return buffer;
}

char *charrealloc(char *buffer, size_t len)
{
	char *new_buffer;

	new_buffer = realloc(buffer, len);
	if (new_buffer == NULL) {
		/* Err */
	}

	return new_buffer;
}
