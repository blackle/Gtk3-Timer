#pragma once

typedef struct AlertAudio AlertAudio;

void alert_audio_init();
AlertAudio* alert_audio_new();
void alert_audio_play(AlertAudio* audio);
