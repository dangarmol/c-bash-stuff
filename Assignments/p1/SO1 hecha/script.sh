#!/bin/bash

#Script de comprobación del programa mitar

#Comprobación si existe el archivo mitar y es ejecutable.
if ! test -x ./mitar
then 
echo "ERROR!.No existe el ejecutable del programa."
return 1
#En caso de encontrar el script continua, y hace la comprobación de si existe la carpeta temp, en caso de que no este creada la crea y en caso de que este creada y la elimina y crea una nueva.
else
#Comprobación.
if test -e ./tmp
then
#Borra carpeta Temp en caso de existir.
rm -r ./tmp
fi
#Crea carpeta Temp.
mkdir tmp
echo "Carpeta Temp creada con exito."
#Nos colocamos dentro de la carpeta temp y comenzamos con la creación de ficheros.
cd ./tmp
#Creamos el fichero1 que contendra un Hola Mundo.
echo "¡Hola Mundo!" > fichero1.txt 
#Por medio del head cogemos las 10 primeras lineas del fichero /etc/passwd y se las colocamos a nuestro fichero2.
head /etc/passwd > fichero2.txt
#Volviendo a utilizar el head -c 1024 nos permite coger 1024 bytes, en este caso de manera aleatoria debido al /dev/urandom.
head -c 1024 /dev/urandom > fichero3.txt
#Es la linea para realizar la compresión de los 3 archivos en un unico fichero llamado fichero.mtar.
../mitar -c -f fichero.mtar fichero1.txt fichero2.txt fichero3.txt
#Creacion de la carpeta out.
mkdir out
#Movemos el fichero creado dentro de la carpeta creada anteriormente.
mv fichero.mtar ./out
#Nos colocamos dentro de la carpeta out.
cd ./out
#Realizamos la descompresion de archivo.
../../mitar -x -f fichero.mtar
#Fin del If y cierre del Script.
fi

