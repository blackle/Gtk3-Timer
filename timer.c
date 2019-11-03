#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdint.h>
#include <math.h>

#include <glib.h>
#include <gio/gio.h>
#include <gtk/gtk.h>

#include "duration_buffer.h"
#include "alert_audio.h"

typedef struct {
	DurationBuffer *buffer;
	GtkBuilder *builder;
	AlertAudio *alert;
	GTimer *timer;
	guint timer_id;
} TimerProgramState;

GtkWidget* get_widget (const TimerProgramState* state, const gchar* name)
{
	return GTK_WIDGET(gtk_builder_get_object (state->builder, name));
}

void update_duration_label(TimerProgramState *state) {
	gchar *formatted = duration_buffer_format (state->buffer);
	gtk_label_set_label (GTK_LABEL(get_widget (state, "duration_text")), formatted);
	g_free (formatted);
}

gchar* format_seconds (unsigned seconds) {
	unsigned minutes = seconds / 60;
	unsigned hours = minutes / 60;
	minutes %= 60;
	seconds %= 60;

	if (hours > 0) {
		return g_strdup_printf ("%d:%02d:%02d", hours, minutes, seconds);
	}
	if (minutes > 0) {
		return g_strdup_printf ("%d:%02d", minutes, seconds);
	}
	return g_strdup_printf ("%d", seconds);
}

void update_countdown_label(TimerProgramState *state, double remaining) {
	unsigned seconds_remaining = (unsigned) fmax (ceil (remaining), 0);
	(void) seconds_remaining;
	(void) state;

	gchar *formatted = format_seconds (seconds_remaining);
	gtk_label_set_label (GTK_LABEL(get_widget (state, "countdown_label")), formatted);
	g_free (formatted);
}

void on_number_clicked(GtkButton *number_button, TimerProgramState *state)
{
	const gchar* label = gtk_button_get_label (number_button);
	duration_buffer_push_number (state->buffer, label[0]);

	update_duration_label (state);
}

void on_undo_clicked(GtkButton *undo_button, TimerProgramState *state)
{
	(void) undo_button;
	duration_buffer_pop_number (state->buffer);

	update_duration_label (state);
}

void stop_timer(TimerProgramState *state) {
	if (state->timer != NULL) {
		g_free (state->timer);
		state->timer = NULL;
	}
	if (state->timer_id != 0) {
		g_source_remove (state->timer_id);
		state->timer_id = 0;
	}
}

void on_timer_finished(TimerProgramState *state) {
	stop_timer (state);
	alert_audio_play (state->alert);
	//todo: UI changes?
}

gboolean on_timer_tick(TimerProgramState *state) {
	unsigned seconds = duration_buffer_get_seconds (state->buffer);

	double elapsed = g_timer_elapsed (state->timer, NULL);
	double remaining = seconds - elapsed;
	update_countdown_label (state, remaining);

	GtkProgressBar *progress = GTK_PROGRESS_BAR(get_widget (state, "countdown_progress"));
	gtk_progress_bar_set_fraction (progress, elapsed/seconds);

	if (remaining <= 0) {
		on_timer_finished (state);
	}

	return TRUE;
}

void start_timer(TimerProgramState *state) {
	stop_timer (state);

	state->timer = g_timer_new ();
	state->timer_id = g_timeout_add (20, (GSourceFunc)on_timer_tick, state);
}

void on_start_clicked(GtkButton *start_button, TimerProgramState *state)
{
	(void) start_button;
	start_timer (state);

	GtkStack *stack = GTK_STACK(get_widget (state, "main_stack"));
	gtk_stack_set_visible_child_name (stack, "countdown_window");
}

void on_back_clicked(GtkButton *start_button, TimerProgramState *state)
{
	(void) start_button;
	stop_timer (state);

	GtkStack *stack = GTK_STACK(get_widget (state, "main_stack"));
	gtk_stack_set_visible_child_name (stack, "timer_window");
}

void connect_numpad_buttons(TimerProgramState* state)
{
	for (int i = 0; i < 10; i++) {
		gchar* object_name = g_strdup_printf ("numpad_button_%d", i);
		GtkWidget* button = get_widget (state, object_name);
		g_signal_connect (button, "clicked", G_CALLBACK(on_number_clicked), state);
		g_free (object_name);
	}
}

int main(int argc, char** argv)
{ 
	gtk_init (&argc, &argv);
	alert_audio_init ();

	TimerProgramState state = {
		.buffer = duration_buffer_new (),
		.builder = gtk_builder_new_from_resource ("/com/blackle/Timer/main.glade"),
		.alert = alert_audio_new (),
		.timer = NULL,
		.timer_id = 0,
	};

	connect_numpad_buttons (&state);
	g_signal_connect (get_widget (&state, "button_undo"), "clicked", G_CALLBACK(on_undo_clicked), &state);
	g_signal_connect (get_widget (&state, "button_start"), "clicked", G_CALLBACK(on_start_clicked), &state);
	g_signal_connect (get_widget (&state, "button_back"), "clicked", G_CALLBACK(on_back_clicked), &state);
	update_duration_label (&state);

	gtk_builder_connect_signals (state.builder, NULL);

	GtkWidget *window = get_widget (&state, "main_window");

	gtk_widget_show_all (window);
	gtk_main ();
}