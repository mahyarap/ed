#include "ed.h"
#include <stdlib.h>
#include <string.h>

Buffer *new_buffer(const char *path)
{
	Buffer *buffer;

	buffer = malloc(sizeof(Buffer));
	if (path != NULL && strlen(path) != 0) {
		strcpy(buffer->path, path);
	}
	else {
		buffer->path[0] = '\0';
	}
	buffer->first_line = NULL;
	buffer->last_line = NULL;
	buffer->cur_line = NULL;

	return buffer;
}

Line *new_line(const char *text, int line_no)
{
	Line *line;

	line = malloc(sizeof(Line));
	line->text = charalloc(strlen(text) + 1);
	strcpy(line->text, text);
	line->line_no = line_no;
	line->next = NULL;
	line->prev = NULL;

	return line;
}


char *read_line(FILE *fs)
{
	int ch;
	char *buf;
	int i = 0;
	int buffsize = BUFFSIZE;

	buf = charalloc(BUFFSIZE);
	while ((ch = fgetc(fs)) != EOF) {
		/* check for overflow */
		if (i == BUFFSIZE) {
			buffsize += BUFFSIZE;
			buf = charrealloc(buf, buffsize);
		}

		buf[i++] = (char)ch;
		buf[i] = '\0';

		if (buf[i - 1] == '\n') {
			return buf;
		}
	}
	if (strlen(buf) != 0) {
		return buf;
	}

	return NULL;
}

ssize_t read_file(const char *path)
{
	FILE *fs;
	char *buf;
	int line_no = 0;
	ssize_t total = 0;

	fs = fopen(path, "r");
	if (fs == NULL) {
		perror(path);
		return -1;
	}

	while ((buf = read_line(fs)) != NULL) {
		Line *line;

		line_no++;
		line = new_line(buf, line_no);
		push_back_line(curbuf, line);
		total += strlen(buf);
	}
	fclose(fs);
	return total;
}

ssize_t write_buffer(const char *path)
{
	Line *p;
	FILE *fs;
	ssize_t total = 0;

	fs = fopen(path, "w+");
	if (fs == NULL) {
		perror(path);
		return -1;
	}

	for (p = curbuf->first_line; p != NULL; p = p->next) {
		fputs(p->text, fs);
		total += strlen(p->text);
	}
	fclose(fs);
	return total;
}

void push_back_line(Buffer *buffer, Line *line)
{
	assert(buffer != NULL);

	if (buffer->last_line == NULL) {
		buffer->first_line = line;
		buffer->last_line = line;
		buffer->cur_line = line;
	}
	else {
		line->prev = buffer->last_line;
		line->next = NULL;
		buffer->last_line->next = line;
		buffer->last_line = line;
	}
}

void push_front_line(Buffer *buffer, Line *line)
{
	assert(buffer != NULL);

	if (buffer->first_line == NULL) {
		buffer->first_line = line;
		buffer->last_line = line;
		buffer->cur_line = line;
	}
	else {
		line->prev = NULL;
		line->next = buffer->first_line;
		buffer->first_line->prev = line;
		buffer->first_line = line;
	}
}

void insert_line(Buffer *buffer, Line *line)
{
	assert(buffer != NULL);

	if (buffer->first_line == NULL) {
		buffer->first_line = line;
		buffer->last_line = line;
		buffer->cur_line = line;
	}
	else if (buffer->cur_line == buffer->last_line) {
		push_back_line(buffer, line);
	}
	else {
		line->prev = buffer->cur_line->next;
		line->next = buffer->cur_line->next->prev;
		buffer->cur_line->next = line;
		buffer->cur_line->next->prev = line;
		buffer->cur_line = line;
	}
}
