#--------------------------------------------------------------
#    Example Makefile for compiling programs that use
#    the Matrix and Vector classes.
#
#    The setup is to compile both example, and exampleSVD
#    example uses only the Vector classes
#    exampleSVD uses both Matrix and Vector classes
#
#    For a project using only Vector classes, all the
#    lines referring to Matrix can be omitted
#
#--------------------------------------------------------------
#
#  Programmer: Donald House
#  Date: March 8, 1999
#
#  Copyright (C) - Donald H. House. 2005
#

CC      = clang++
C	= cpp
H	= h
CFLAGS 	=  

ifeq ("$(shell uname)", "Darwin")
  LDFLAGS     = -framework Foundation -framework GLUT -framework OpenGL -lm
else
  ifeq ("$(shell uname)", "Linux")
    LDFLAGS     = -L /usr/lib64/  -lGLEW -lglut -lGL -lGLU -lm -lXmu
  endif
endif

HFILES 	= Vector.${H} Utility.${H} Matrix.${H} 
OFILES 	= Vector.o Utility.o Matrix.o
PROJECT = main

${PROJECT}:	${PROJECT}.o $(OFILES)
	${CC} $(CFLAGS) -o ${PROJECT} ${PROJECT}.o $(OFILES) $(LDFLAGS)

${PROJECT}.o: ${PROJECT}.${C} $(HFILES)
	${CC} $(CFLAGS) -c ${PROJECT}.${C}

Matrix.o: Matrix.${C} Matrix.${H} Vector.${H} 
	${CC} $(CFLAGS) -c Matrix.${C}

Vector.o: Vector.${C} Vector.${H} Utility.${H} 
	${CC} $(CFLAGS) -c Vector.${C}

Utility.o: Utility.${C} Utility.${H}
	${CC} $(CFLAGS) -c Utility.${C}

debug:
	make 'DFLAGS = /usr/lib/debug/malloc.o'

clean:
	rm *.o *~ 

