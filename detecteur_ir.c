/*
 * detecteur_ir.c
 *
 *  Created on: 30 avr. 2020
 *      Author: Roxane
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ch.h"
#include "hal.h"
#include "memory_protection.h"
#include <usbcfg.h>
#include <chprintf.h>
#include <detecteur_ir.h>
#include <sensors/proximity.h>


#define		NB_CAPTEUR		8
#define		SEUIL			400

//initialisation du bus pour les proximity sensor
messagebus_t bus;
MUTEX_DECL(bus_lock);
CONDVAR_DECL(bus_condvar);

void detecteur_ir_init (void)
{
    proximity_start();
    messagebus_init(&bus, &bus_lock, &bus_condvar);
    calibrate_ir();
}

uint8_t detecteur_ir_trop_près (void)
{
	for (i=0; i<NB_CAPTEUR; i++)
	{
		if(get_prox(i)>SEUIL)
			return i+1;

		else return 0;
	}
}


void detecteur_ir_print(void)
{
	int a, b, c, d, e, f, g, h;
			//Récupère les valeur des capteurs IR et donne la "distance"
			chprintf("1e capteur:%d  2e capteur : %d  3e capteur:%d  "
					"4e capteur : %d  5e capteur:%d  6e capteur : %d  7e capteur:%d  8e capteur : %d \r\n",

					a=get_prox(0),
					b=get_prox(1),
					c=get_prox(2),
					d=get_prox(3),
					e=get_prox(4),
					f=get_prox(5),
					g=get_prox(6),
					h=get_prox(7)
					);
}
