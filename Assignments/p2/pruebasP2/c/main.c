#include <stdio.h>
#include <string.h>

int main(int argc, const char * argv[]) {
    // insert code here...
    printf("Hello, World!\n");
    printf("Estamos en el capítulo 2 del curso\n");
    
    int nota = 10;
    printf("La nota del examen es %i\n", nota);
    
    
    
    //para mostrar cadenas se establecen con nombre y [] y se muestran con %s
    //paramostrar cosas de formato lo que se emplea es %lu ?? o %i tmbn funciona
    char nombre[] = "Ainhoaaaaa";
    
    printf( "La novia de miguel que le quiere mucho es.. %s\n", nombre );
    printf( "Tamaño de la cadena: %lu bytes\n", sizeof nombre );
    
    char texto[]= "Ayona";
    int longitud;
    
    longitud= strlen(texto);
    
    printf("la cadena \"%s\" tiene %i caracteres\n",texto, longitud);
    
    char nombre_completo[30];
    char nombremikeh[]="miguel";
    char apellido[]="jerez";
    strcpy(nombre_completo, nombremikeh);
    strcat(nombre_completo," ");
    strcat(nombre_completo, apellido);
    printf("el nombre completo es %s \n",nombre_completo);
    
    printf("Con c por ejemplo podemos realizar operaciones varias %d \n", 2 + 10 / 5 * 2 - 7 % 1);
    
    
    char cadena[]="El puerto paralelo del PC";
    char *p;
    int espacios = 0, letras_e = 0;
    
    p = cadena;
    while (*p != '\0') {
        if (*p == ' ') espacios++;
        if (*p == 'e') letras_e++;
        p++;
    }
    
    printf( "En la cadena \"%s\" hay:\n", cadena );
    printf( "  %i espacios\n", espacios );
    printf( "  %i letras e\n", letras_e );
    
    int numeroUno=2;
    int numeroDos=3;
    int numeroTres=5;
    int suma = numeroUno + numeroDos + numeroTres;
    printf("el resultado de la suma es %d \n",suma);
    
    
    int primerNumero, segundoNumero, resultado;
    printf("introduce el primer numero");
    scanf("%d", &primerNumero);
    printf("introduce el segundo numero");
    scanf("%d", &segundoNumero);
    int suma2 = primerNumero+segundoNumero;
    printf("el resultado de la operacion de suma es %d",suma2);
    
    return 0;
}
