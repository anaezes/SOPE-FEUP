#!/bin/bash

#Makefile for generator.c and sauna.c

allfiles: generator.c sauna.c
		gcc -Wall -pthread generator.c request.c activity.c -o gerador
		gcc -Wall -pthread sauna.c request.c activity.c -o sauna

generator: generator.c
		gcc -Wall -pthread generator.c request.c activity.c -o gerador

sauna: sauna.c
		gcc -Wall -pthread sauna.c request.c activity.c -o sauna

clean:
		rm -f gerador
		rm -f sauna
