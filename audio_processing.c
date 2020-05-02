/*
 * audio_processing.c  fichier qui provient du TP5
 * Projet microinformatique 2020
 * Complété et modifié par : Groupe 11 Roxane Pangaud, Matthieu Duret
 *
 */

#include "ch.h"
#include "hal.h"
#include <main.h>
#include <usbcfg.h>
#include <chprintf.h> // TO BE DELETED

#include <motors.h>
#include <audio/microphone.h>
#include <audio_processing.h>
#include <fft.h>
#include <arm_math.h>
#include <leds.h>
#include <selector.h>
#include <detecteur_ir.h>



//2 times FFT_SIZE because these arrays contain complex numbers (real + imaginary)
static float micLeft_cmplx_input[2 * FFT_SIZE];

//Arrays containing the computed magnitude of the complex numbers
static float micLeft_output[FFT_SIZE];

//seuil d'intensité du son : permet d'être moins sensible au bruit ambiant
#define MIN_VALUE_THRESHOLD	10000 


//we define here the string frequency to aim for
#define	MI_LOW		21  //81.9 Hz
#define LA			28	//109.2 Hz
#define RE			38	//148.2 Hz
#define SOL			50 	//195 Hz
#define SI			63	//245.7 Hz
#define MI_HIGH		85	//331.5 Hz

//we define here the frequency threshold for each string
#define MI_LOW_MIN		(MI_LOW - 1)
#define MI_LOW_MAX		(MI_LOW + 1)
#define LA_MIN			LA
#define LA_MAX			(LA + 1)
#define RE_MIN			(RE - 1)
#define RE_MAX			(RE + 1)
#define SOL_MIN			(SOL - 1)
#define SOL_MAX			(SOL + 1)
#define SI_MIN			SI
#define SI_MAX			(SI + 1)
#define MI_HIGH_MIN		MI_HIGH
#define MI_HIGH_MAX		(MI_HIGH + 1)

#define MIN_FREQ		16	//we don't analyze before this index to not use resources for nothing
#define MAX_FREQ		55	//we don't analyze after this index to not use resources for nothing

#define OFFSET 			5 // permet de définir l'intervalle de frequence sur lequel on travaille

#define FREQ_FORWARD	MI_LOW
#define FREQ_LEFT		LA
#define FREQ_RIGHT		RE
#define FREQ_BACKWARD	SOL

/*
*	Retourne la fréquence de plus grande intensité captée par le micro dans un intervalle défini
*	Paramètres : float* data : buffer qui contient les fréquences mesurées par le micro
*				 uint8_t min_freq : borne inférieure de l'intervalle de fréquence sur lequel on travaille
*				 uint8_t max_freq : borne supérieure de l'intervalle de fréquence sur lequel on travaille
*/
int16_t get_freq (float* data, uint8_t min_freq, uint8_t max_freq)
{
	float max_norm = MIN_VALUE_THRESHOLD;
	int16_t freq_index = -1;	// correspond à la fréquence reçue

	//search for the highest peak
	for(uint16_t i = min_freq ; i <= max_freq ; i++){
		if(data[i] > max_norm){
			max_norm = data[i];
			freq_index = i;
		}
	}

	return freq_index;
}

/* Indique si la fréquence mesurée se trouve dans l'intervalle désiré ou non. Donne un feedback à l'utilisateur avec les leds
 * Paramètres : int16_t freq_index : fréquence mesurée
 * 				int16_t min_string : borne inférieure de l'intervalle de fréquence désiré
 * 				int16_t max_string : borne supérieure de l'intervalle de fréquence désiré
 */
void tune_string(int16_t freq_index, int16_t min_string, int16_t max_string)
{
	if (freq_index < min_string  && freq_index != -1){	// fréquence trop basse
		clear_leds();
		set_rgb_led(LED8,1,0,0);
	}

	else if (freq_index > max_string){ // fréquence trop haute
		clear_leds();
		set_rgb_led(LED2,1,0,0);
	}

	else if (freq_index >= min_string && freq_index <= max_string ){ // fréquence juste
		clear_leds();
		for(int i=0; i<4; i++)
			set_rgb_led(i, 0, 1, 0);
	}

	else if (freq_index == -1)
		clear_leds();
}

/* Sélectionne la corde à accorder en fonction de la position du sélecteur
 * Paramètre : float* data :  buffer qui contient les fréquences mesurées par le micro
 */
void tuner(float* data)
{
	uint8_t sel_state = get_selector();
	int16_t freq_index = -1 ;

	switch (sel_state){

	case 1 :
		freq_index = get_freq(data, MI_LOW-OFFSET, MI_LOW+OFFSET);
		tune_string(freq_index, MI_LOW_MIN, MI_LOW_MAX);
		break;

	case 2:
		freq_index = get_freq(data, LA-OFFSET, LA+OFFSET);
		tune_string(freq_index, LA_MIN, LA_MAX);
		break;

	case 3 :
		freq_index = get_freq(data, RE-OFFSET, RE+OFFSET);
		tune_string(freq_index, RE_MIN, RE_MAX);
		break;

	case 4 :
		freq_index = get_freq(data, SOL-OFFSET, SOL+OFFSET);
		tune_string(freq_index, SOL_MIN, SOL_MAX);
		break;

	case 5 :
		freq_index = get_freq(data, SI-OFFSET, SI+OFFSET);
		tune_string(freq_index, SI_MIN, SI_MAX);
		break;

	case 6 :
		freq_index = get_freq(data, MI_HIGH-OFFSET, MI_HIGH+OFFSET);
		tune_string(freq_index, MI_HIGH_MIN, MI_HIGH_MAX);
		break;

	default:
		clear_leds();
		break;
	}
}
/* Gère les déplacements du robots en fonction des fréquences mesurées et des éventuels obstacles
 * Paramètres : float* data :  buffer qui contient les fréquences mesurées par le micro
 */
void sound_remote(float* data)
{
	int16_t freq_index = get_freq(data, MIN_FREQ, MAX_FREQ);
	uint8_t capteur = get_zone_detecteur_ir ();

	//gère les leds qui indiquent les obstacles
	toggle_leds_collision();

	//go forward NOTE : on considère également les 2e harmoniques du MI_LOW, LA et RE qui sont en général de plus grande intensité


	 if(((freq_index >= (FREQ_FORWARD-1) && freq_index <= (FREQ_FORWARD+1)) ||
	    (freq_index >= (2*FREQ_FORWARD-1) && freq_index <= (2*FREQ_FORWARD+1))) && capteur != FRONT){
		left_motor_set_speed(400);
		right_motor_set_speed(400);
	}


	// turn left
	else if((freq_index >= (FREQ_LEFT - 1) && freq_index <= (FREQ_LEFT + 1)) ||
			(freq_index >= (2*FREQ_LEFT- 1) && freq_index <= (2*FREQ_LEFT + 1))){
		left_motor_set_speed(-400);
		right_motor_set_speed(400);
	}

	// turn right
	else if((freq_index >= (FREQ_RIGHT - 1) && freq_index <= (FREQ_RIGHT + 1)) ||
			(freq_index >= (2*FREQ_RIGHT - 1) && freq_index <= (2*FREQ_RIGHT + 1))){
		left_motor_set_speed(400);
		right_motor_set_speed(-400);
	}

	//go backward
	else if((freq_index >= (FREQ_BACKWARD - 1) && freq_index <= (FREQ_BACKWARD + 1)) && capteur != BACK){
		left_motor_set_speed(-400);
		right_motor_set_speed(-400);
	}

	 // stop si aucune commande
	else{
		left_motor_set_speed(0);
		right_motor_set_speed(0);

		for(int i=0; i<4; i++)
			set_rgb_led(i, 0, 1, 1);
	}
}

/*
*	Callback called when the demodulation of the four microphones is done.
*	We get 160 samples per mic every 10ms (16kHz)
*	
*	params :
*	int16_t *data			Buffer containing 4 times 160 samples. the samples are sorted by micro
*							so we have [micRight1, micLeft1, micBack1, micFront1, micRight2, etc...]
*	uint16_t num_samples	Tells how many data we get in total (should always be 640)
*/
void processAudioData(int16_t *data, uint16_t num_samples)
{

	/*
	*
	*	We get 160 samples per mic every 10ms
	*	So we fill the samples buffers to reach
	*	4096 samples, then we compute the FFTs.
	*
	*/

	static uint16_t nb_samples = 0;

	//loop to fill the buffers
	for(uint16_t i = 0 ; i < num_samples ; i+=4){
		//construct an array of complex numbers. Put 0 to the imaginary part
		micLeft_cmplx_input[nb_samples] = (float)data[i + MIC_LEFT];

		nb_samples++;

		micLeft_cmplx_input[nb_samples] = 0;

		nb_samples++;

		//stop when buffer is full
		if(nb_samples >= (2 * FFT_SIZE)){
			break;
		}
	}

	if(nb_samples >= (2 * FFT_SIZE)){
		/*	FFT processing
		*
		*	This FFT function stores the results in the input buffer given.
		*	This is an "In Place" function. 
		*/

		doFFT_optimized(FFT_SIZE, micLeft_cmplx_input);


		/*	Magnitude processing
		*
		*	Computes the magnitude of the complex numbers and
		*	stores them in a buffer of FFT_SIZE because it only contains
		*	real numbers.
		*
		*/
		arm_cmplx_mag_f32(micLeft_cmplx_input, micLeft_output, FFT_SIZE);

		nb_samples = 0;

		tuner(micLeft_output);

		if (get_selector() == 12)
			sound_remote(micLeft_output);
	}
}
