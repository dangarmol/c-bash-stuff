#!/bin/bash

echo "¿Que programa quieres ejecutar?"
read nombrePrograma
if [ $nombrePrograma == "prog1" ]; then
	gcc -o prog1 prog1.c;
	./prog1;
elif [ $nombrePrograma == "prog2" ]; then
	gcc -o prog2 prog2.c;
	./prog2;
elif [ $nombrePrograma == "ambos" ]; then
	gcc -o prog1 prog1.c;
	./prog1;	
	gcc -o prog2 prog2.c;
	./prog2;
else
	echo "Opción no válida."
fi