/*
 * fft.h  fichier qui provient du TP5
 * Projet microinformatique 2020
 * Modifié par : Groupe 11 Roxane Pangaud, Matthieu Duret
 *
 */

#ifndef FFT_H
#define FFT_H


typedef struct complex_float{
	float real;
	float imag;
}complex_float;

void doFFT_optimized(uint16_t size, float* complex_buffer);


#endif /* FFT_H */
