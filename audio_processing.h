/*
 * audio_processing.h  fichier qui provient du TP5
 * Projet microinformatique 2020
 * Complété et modifié par : Groupe 11 Roxane Pangaud, Matthieu Duret
 *
 */

#ifndef AUDIO_PROCESSING_H
#define AUDIO_PROCESSING_H


#define FFT_SIZE 	4096

void processAudioData(int16_t *data, uint16_t num_samples);

#endif /* AUDIO_PROCESSING_H */
