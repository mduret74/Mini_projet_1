/*
 * detecteur_ir.c
 *
 *  Created on: 30 avr. 2020
 *      Author: Roxane
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sensors/proximity.h>
#include <detecteur_ir.h>


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
