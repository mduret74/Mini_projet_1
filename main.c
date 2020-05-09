/*
 * main.c fichier qui provient du TP5
 * Projet microinformatique 2020
 * Complété et modifié par : Groupe 11 Roxane Pangaud, Matthieu Duret
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "ch.h"
#include "hal.h"
#include "memory_protection.h"
#include "spi_comm.h"
#include <main.h>
#include <motors.h>
#include <audio/microphone.h>

#include <audio_processing.h>
#include <arm_math.h>
#include <sensors/proximity.h>


//initialisation du bus pour les proximity sensor
messagebus_t bus;
MUTEX_DECL(bus_lock);
CONDVAR_DECL(bus_condvar);


int main(void)
{
    halInit();
    chSysInit();
    mpu_init();

    //start the spi_thread
    spi_comm_start();

    //inits the motors
    motors_init();

    //start the proximity sensors
    proximity_start();
	messagebus_init(&bus, &bus_lock, &bus_condvar);
	calibrate_ir();


    //starts the microphones processing thread.
    //it calls the callback given in parameter when samples are ready
    mic_start(&processAudioData);


    /* Infinite loop. */
    while (1) {}
}

#define STACK_CHK_GUARD 0xe2dee396
uintptr_t __stack_chk_guard = STACK_CHK_GUARD;

void __stack_chk_fail(void)
{
    chSysHalt("Stack smashing detected");
}
