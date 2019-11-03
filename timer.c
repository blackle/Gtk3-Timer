#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdint.h>

#include <glib.h>
#include <gio/gio.h>
#include <gtk/gtk.h>

#include "duration_buffer.h"

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