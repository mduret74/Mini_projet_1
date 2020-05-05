/*
 * fft.c  fichier qui provient du TP5
 * Projet microinformatique 2020
 * Modifié par : Groupe 11 Roxane Pangaud, Matthieu Duret
 *
 */
#include "ch.h"
#include "hal.h"
#include <main.h>
#include <fft.h>
#include <arm_math.h>
#include <arm_const_structs.h>


/*
*	Wrapper to call a very optimized fft function provided by ARM
*	which uses a lot of tricks to optimize the computations
*/
void doFFT_optimized(uint16_t size, float* complex_buffer){
	if(size == 4096)
		arm_cfft_f32(&arm_cfft_sR_f32_len4096, complex_buffer, 0, 1);
	
}

