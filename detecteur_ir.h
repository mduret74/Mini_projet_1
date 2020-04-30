/*
 * detecteur_ir.h
 *
 *  Created on: 30 avr. 2020
 *      Author: Roxane
 */

#ifndef DETECTEUR_IR_H_
#define DETECTEUR_IR_H_

//fonction qui initialise les moteurs
void detecteur_ir_init (void);

//fonction qui renvoie 0 si les d�tecteurs d�tectent
//une surface loin ou alors le num�ro du capteur s'il
//y en a un qui d�tecte un objet proche (de 1 � 8)
uint8_t detecteur_ir_trop_pr�s (void);


//fonction qui permet l'�criture de ce que renvoie les capteurs dans realterm.
void detecteur_ir_print(void);



#endif /* DETECTEUR_IR_H_ */
