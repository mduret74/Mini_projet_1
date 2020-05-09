/*
 * detecteur_ir.c
 * Projet microinformatique 2020
 *
 * Authors	: Groupe 11 Roxane Pangaud, Matthieu Duret
 */
#include "ch.h"
#include "hal.h"
#include <detecteur_ir.h>
#include <sensors/proximity.h>
#include <leds.h>

#define		NB_CAPTEUR		8

// seuil de luminosité pour la détection d'obstacle
#define		SEUIL			100


/* Renvoie le numéro du capteur (entre 1 et 8) de proximité qui détecte un obstacle
 * Renvoie 0 si aucun obstacle n'est détecté
 */
uint8_t get_detecteur_ir (void)
{
	for (uint8_t i=0; i<NB_CAPTEUR; i++){
		if(get_prox(i)>SEUIL)
			return i+1;
	}

	return 0;
}

/* Indique si l'obstacle se trouve devant ou derrière le robot (respectivement FRONT ou BACK)
 * Renvoie NONE sinon
 */
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

/* Allume les leds en fonction de la position de l'obstacle détecté
 */
void toggle_leds_collision(void)
{
	if(get_zone_detecteur_ir() == FRONT){
		clear_leds();
		set_led(LED1,1);
	}

	else if (get_zone_detecteur_ir() == BACK){
		clear_leds();
		set_led(LED5,1);
	}

	else
		clear_leds();
}
