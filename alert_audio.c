#include "alert_audio.h"
#include <glib.h>
#include <ao/ao.h>
#include <gio/gio.h>
#include <string.h>
#include <math.h>

#define PLAYBACK_RATE 8000
#define ALERT_HIT_LENGTH 900
#define ALERT_PAUSE_LENGTH 220
#define ALERT_HIT_BYTES ALERT_HIT_LENGTH * sizeof(short)
#define ALERT_PAUSE_BYTES ALERT_PAUSE_LENGTH * sizeof(short)

struct AlertAudio {
	short hit[ALERT_HIT_LENGTH];
	short pause[ALERT_PAUSE_LENGTH];
};

void alert_audio_init()
{
	ao_initialize();
}

void add_fade(float *fade, int i, int length) {
	if (i < length) {
		*fade = i/(float)length;
	}
}

AlertAudio* alert_audio_new()
{
	AlertAudio* audio = g_new(AlertAudio, 1);
	memset(audio->pause, 0, ALERT_PAUSE_BYTES);
	for (int i = 0; i < ALERT_HIT_LENGTH; i++) {
		float fade = 1.;
		add_fade(&fade, i, 50);
		add_fade(&fade, ALERT_HIT_LENGTH - i - 1, 200);
		audio->hit[i] = 20000. * sin((float)i * 1.3)*fade;
	}

	return audio;
}

static ao_device* open_audio_device()
{
	ao_sample_format format = {
		.bits = 16,
		.channels = 1,
		.rate = PLAYBACK_RATE,
		.byte_format = AO_FMT_NATIVE,
		.matrix = NULL,
	};

	return ao_open_live(ao_default_driver_id(), &format, NULL);
}

static void play_alert_async(GTask *task, gpointer source_object, AlertAudio* audio, GCancellable *cancellable) {
	(void) task;
	(void) source_object;
	(void) cancellable;

	ao_device* device = open_audio_device();
	if (device == NULL) {
		fprintf(stderr, "[timer] couldn't play audio!");
		return;
	}

	for (int j = 0; j < 2; j++) {
		for (int i = 0; i < 4; i++) {
			ao_play(device, (char*)audio->hit, ALERT_HIT_BYTES);
			ao_play(device, (char*)audio->pause, ALERT_PAUSE_BYTES);
		}
		for (int i = 0; i < 16; i++) {
			ao_play(device, (char*)audio->pause, ALERT_PAUSE_BYTES);
		}
	}
	ao_close(device);
}

void alert_audio_play(AlertAudio* audio)
{
	GTask *task = g_task_new (NULL, NULL, NULL, NULL);
	g_task_set_task_data (task, audio, NULL);
	g_task_run_in_thread (task, (GTaskThreadFunc)play_alert_async);
}
