#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#include <glib.h>
#include <gio/gio.h>
#include <gtk/gtk.h>

#define DURATION_BUFFER_SIZE 6
#define DURATION_BUFFER_INDEX(buff, k) ((buff->offset + k + DURATION_BUFFER_SIZE) % DURATION_BUFFER_SIZE)
#define DURATION_BUFFER_AT(buff, k) (buff->data[DURATION_BUFFER_INDEX(buff, k)])

typedef struct {
	gchar data[DURATION_BUFFER_SIZE];
	unsigned offset;
} DurationBuffer;

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

DurationBuffer* global_buffer;

void update_duration_label(GtkLabel* duration_label, const DurationBuffer* buff) {
	gchar *formatted = duration_buffer_format(buff);
	gtk_label_set_label(duration_label, formatted);
	g_free(formatted);
}

void on_number_clicked(GtkButton *number_button, gpointer duration_label) {
	const gchar* label = gtk_button_get_label (number_button);
	duration_buffer_push_number(global_buffer, label[0]);

	update_duration_label(GTK_LABEL(duration_label), global_buffer);
}

void on_undo_clicked(GtkButton *undo_button, gpointer duration_label) {
	(void) undo_button;
	duration_buffer_pop_number(global_buffer);

	update_duration_label(GTK_LABEL(duration_label), global_buffer);
}

void on_start_clicked(GtkButton *start_button, gpointer stack) {
	(void) start_button;
	gtk_stack_set_visible_child_name (GTK_STACK(stack), "countdown_window");
}

void on_back_clicked(GtkButton *start_button, gpointer stack) {
	(void) start_button;
	gtk_stack_set_visible_child_name (GTK_STACK(stack), "timer_window");
}

int main(int argc, char** argv)
{ 
	gtk_init(&argc, &argv);
	global_buffer = new_duration_buffer();

	GtkBuilder *builder = gtk_builder_new_from_resource ("/com/blackle/Timer/main.glade");
	gtk_builder_connect_signals (builder, NULL);

	GtkWidget *window = GTK_WIDGET(gtk_builder_get_object (builder, "main_window"));

	gtk_widget_show_all (window);

	gtk_main();

	g_free(global_buffer);
}