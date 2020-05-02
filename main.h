/*
 * main.h fichier qui provient du TP5
 * Projet microinformatique 2020
 * Complété et modifié par : Groupe 11 Roxane Pangaud, Matthieu Duret
 *
 */

#ifndef MAIN_H
#define MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

#include "camera/dcmi_camera.h"
#include "msgbus/messagebus.h"
#include "parameter/parameter.h"


/** Robot wide IPC bus. */
extern messagebus_t bus;

extern parameter_namespace_t parameter_root;

#ifdef __cplusplus
}
#endif

#endif
