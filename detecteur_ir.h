/*
 * detecteur_ir.h
 * Projet microinformatique 2020
 * Authors	: Groupe 11 Roxane Pangaud, Matthieu Duret
 */

#ifndef DETECTEUR_IR_H_
#define DETECTEUR_IR_H_

#define CAPTEUR_1	1
#define CAPTEUR_2	2
#define CAPTEUR_3	3
#define CAPTEUR_4	4
#define CAPTEUR_5	5
#define CAPTEUR_6	6
#define CAPTEUR_7	7
#define CAPTEUR_8	8

#define NONE	0
#define FRONT 	1
#define BACK	2

//fonction qui renvoie 0 si les détecteurs détectent
//une surface loin ou alors le numéro du capteur s'il
//y en a un qui détecte un objet proche (de 1 à 8)
uint8_t get_detecteur_ir (void);

// renvoie NONE, FRONT ou BACK en fonction de la position de l'obstacle par rapport aux capteurs
uint8_t get_zone_detecteur_ir (void);

// allume la led de la zone proche de l'obstacle
void toggle_leds_collision(void);

#endif /* DETECTEUR_IR_H_ */
