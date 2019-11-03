#include "duration_buffer.h"
#include <string.h>

/*
 * I don't know why I made this struct
 * it's prematurely optimized and it was only after I wrote it that I realized this
 * basically all it does is represent a fixed length stack of numbers.
 * popping a number off the front of the stack puts a '0' at the end of the stack
 * pushing more numbers than can fit will cause the last number to be consumed
 */

#define DURATION_BUFFER_SIZE 6
#define DURATION_BUFFER_INDEX(buff, k) ((buff->offset + k + DURATION_BUFFER_SIZE) % DURATION_BUFFER_SIZE)
#define DURATION_BUFFER_AT(buff, k) (buff->data[DURATION_BUFFER_INDEX(buff, k)])

struct DurationBuffer {
	gchar data[DURATION_BUFFER_SIZE];
	unsigned offset;
};

DurationBuffer* new_duration_buffer() {
	DurationBuffer* buff = g_new(DurationBuffer, 1);
	memset(buff->data, '0', DURATION_BUFFER_SIZE);
	buff->offset = 0;
	return buff;
}

void duration_buffer_push_number(DurationBuffer* buff, gchar number) {
	buff->offset++;
	buff->data[DURATION_BUFFER_INDEX(buff, -1)] = number;
}

void duration_buffer_pop_number(DurationBuffer* buff) {
	buff->offset = DURATION_BUFFER_INDEX(buff, -1);
	buff->data[buff->offset] = '0';
}

gchar* duration_buffer_format(const DurationBuffer* buff) {
	gchar *formatted = g_strnfill(DURATION_BUFFER_SIZE + (DURATION_BUFFER_SIZE/2-1)*3, ' ');
	int index = 0;
	for (int i = 0; i < DURATION_BUFFER_SIZE; i++) {
		if (i != 0 && i % 2 == 0) {
			formatted[index + 1] = ':';
			index += 3;
		}

		formatted[index] = DURATION_BUFFER_AT(buff, i);
		index++;
	}
	return formatted;
}
