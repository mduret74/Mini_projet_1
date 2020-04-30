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
#define		SEUIL			50



uint8_t get_detecteur_ir (void)
{
	for (uint8_t i=0; i<NB_CAPTEUR; i++)
	{
		if(get_prox(i)>SEUIL)
			return i+1;

	}

	return 0;
}


uint8_t get_zone_detecteur_ir (void)
{
	uint8_t capteur = get_detecteur_ir();

	if (capteur == CAPTEUR_1 || capteur == CAPTEUR_2 || capteur == CAPTEUR_7 || capteur == CAPTEUR_8)
		return FRONT;

	else if (capteur == CAPTEUR_4 || capteur == CAPTEUR_5)
		return BACK;

	else
		return NONE;

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
