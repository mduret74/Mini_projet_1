#include "ch.h"
#include "hal.h"
#include <main.h>
#include <usbcfg.h>
#include <chprintf.h>

#include <motors.h>
#include <audio/microphone.h>
#include <audio_processing.h>
#include <communications.h>
#include <fft.h>
#include <arm_math.h>
#include <leds.h>
#include <selector.h>


//semaphore
static BSEMAPHORE_DECL(sendToComputer_sem, TRUE);

//2 times FFT_SIZE because these arrays contain complex numbers (real + imaginary)
static float micLeft_cmplx_input[2 * FFT_SIZE];

//Arrays containing the computed magnitude of the complex numbers
static float micLeft_output[FFT_SIZE];

#define MIN_VALUE_THRESHOLD	10000 


//NOTE : we define here the string frequency to aim for
//       WARNING : (the frequencies corresponding to the index are calculated with the resolution obtained with a buffer size of 4096 )

#define	MI_LOW		21  	//juste cette ligne : res = 15.625	-> index 5		//21  	//81.9 Hz
#define LA			28	//109.2 Hz
#define RE			38	//148.2 Hz
#define SOL			50  //12  //50	//195 Hz
#define SI			63	//245.7 Hz
#define MI_HIGH		85	//331.5 Hz

//NOTE : we define here the frequency threshold for each string

#define MI_LOW_MIN		(MI_LOW - 1)
#define MI_LOW_MAX		(MI_LOW + 1)
#define LA_MIN			(LA - 1 )
#define LA_MAX			(LA + 1 )
#define RE_MIN			(RE - 1 )
#define RE_MAX			(RE + 1 )
#define SOL_MIN			(SOL - 1 )
#define SOL_MAX			(SOL + 1 )
#define SI_MIN			SI
#define SI_MAX			(SI + 2 )
#define MI_HIGH_MIN		MI_HIGH
#define MI_HIGH_MAX		(MI_HIGH + 2 )

#define MIN_FREQ		0	//we don't analyze before this index to not use resources for nothing
#define MAX_FREQ		40	//we don't analyze after this index to not use resources for nothing

#define OFFSET 			5 // permet de définir l'intervalle de frequence sur lequel on travaille

#define FREQ_FORWARD_L		(FREQ_FORWARD-1)
#define FREQ_FORWARD_H		(FREQ_FORWARD+1)
#define FREQ_LEFT_L			(FREQ_LEFT-1)
#define FREQ_LEFT_H			(FREQ_LEFT+1)
#define FREQ_RIGHT_L		(FREQ_RIGHT-1)
#define FREQ_RIGHT_H		(FREQ_RIGHT+1)
#define FREQ_BACKWARD_L		(FREQ_BACKWARD-1)
#define FREQ_BACKWARD_H		(FREQ_BACKWARD+1)

/*
*	Simple function used to detect and return the highest value in a buffer
*/

int16_t get_freq (float* data, uint8_t min_freq, uint8_t max_freq)
{
	float max_norm = MIN_VALUE_THRESHOLD;
	int16_t freq_index = -1;	// correspond à la fréquence jouée

	//search for the highest peak
	for(uint16_t i = min_freq ; i <= max_freq ; i++){
		if(data[i] > max_norm){
			max_norm = data[i];
			freq_index = i;
		}
	}

	return freq_index;
}

void tune_string(int16_t freq_index, int16_t min_string, int16_t max_string)
{
	if (freq_index < min_string ){
		clear_leds();
		set_rgb_led(LED8,1,0,0);
	}

	else if (freq_index > max_string){
		clear_leds();
		set_rgb_led(LED2,1,0,0);
	}

	else if (freq_index >= min_string && freq_index <= max_string ){
		clear_leds();
		for(int i=0; i<4; i++)
			set_rgb_led(i, 0, 1, 0);
	}
}
void tuner(float* data)
{

	uint8_t sel_state = get_selector();
	int16_t freq_index = -1 ;

	switch (sel_state){

	case 1 :
		freq_index = get_freq(data, MI_LOW-OFFSET, MI_LOW+OFFSET);
		tune_string(freq_index, MI_LOW, MI_LOW);
		break;

	case 2:
		freq_index = get_freq(data, LA-OFFSET, LA+OFFSET);
		tune_string(freq_index, LA, LA);
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


/*
*	Callback called when the demodulation of the four microphones is done.
*	We get 160 samples per mic every 10ms (16kHz)
*	
*	params :
*	int16_t *data			Buffer containing 4 times 160 samples. the samples are sorted by micro
*							so we have [micRight1, micLeft1, micBack1, micFront1, micRight2, etc...]
*	uint16_t num_samples	Tells how many data we get in total (should always be 640)
*/
void processAudioData(int16_t *data, uint16_t num_samples){

	/*
	*
	*	We get 160 samples per mic every 10ms
	*	So we fill the samples buffers to reach
	*	1024 samples, then we compute the FFTs.
	*
	*/

	static uint16_t nb_samples = 0;
	static uint8_t mustSend = 0;

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


		//sends only one FFT result over 10 for 1 mic to not flood the computer
		//sends to UART3
		if(mustSend > 8){
			//signals to send the result to the computer
			chBSemSignal(&sendToComputer_sem);
			mustSend = 0;
		}
		nb_samples = 0;
		mustSend++;

		//sound_remote(micLeft_output);
		tuner(micLeft_output);
	}
}

void wait_send_to_computer(void){
	chBSemWait(&sendToComputer_sem);
}

float* get_audio_buffer_ptr(BUFFER_NAME_t name){
	if(name == LEFT_CMPLX_INPUT){
		return micLeft_cmplx_input;
	}

	else if (name == LEFT_OUTPUT){
		return micLeft_output;
	}

	else{
		return NULL;
	}
}
