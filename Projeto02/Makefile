#!/bin/bash

#Makefile for generator.c and sauna.c

allfiles: generator.c sauna.c
		gcc -Wall generator.c -o generator
		gcc -Wall sauna.c -o sauna

generator: generator.c
		gcc -Wall generator.c -o generator

sauna: sauna.c
		gcc -Wall sauna.c -o sauna

clean:
		rm -f generator
		rm -f sauna
