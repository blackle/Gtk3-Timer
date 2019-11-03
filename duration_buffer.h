#pragma once
#include <glib.h>

typedef struct DurationBuffer DurationBuffer;

DurationBuffer* new_duration_buffer();

void duration_buffer_push_number(DurationBuffer* buff, gchar number);
void duration_buffer_pop_number(DurationBuffer* buff);

gchar* duration_buffer_format(const DurationBuffer* buff);

unsigned duration_buffer_get_seconds(const DurationBuffer* buff);
void duration_buffer_set_seconds(DurationBuffer* buff, unsigned seconds);
