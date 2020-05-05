
# Makefile  Mini_projet
# Projet microinformatique 2020
# Groupe 11 Roxane Pangaud, Matthieu Duret
 

# Define project name here
PROJECT = Mini_projet_1

#Define path to the e-puck2_main-processor folder
GLOBAL_PATH = ../../lib/e-puck2_main-processor

#Source files to include
CSRC += ./main.c \
		./audio_processing.c \
		./fft.c \
		./detecteur_ir.c\

#Header folders to include
INCDIR += 

#Jump to the main Makefile
include $(GLOBAL_PATH)/Makefile