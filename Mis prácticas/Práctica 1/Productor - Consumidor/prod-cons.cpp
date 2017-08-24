/******************************************************************************
 Francisco Javier Caracuel Beltrán
 2º B - Grado en Ingeniería Informática
 Sistemas Concurrentes y Distribuidos
 2015

 --

 Práctica 1.

 Práctica donde se hace uso de semáforos POSIX para la sincronización de
 hebras en el ejemplo del Productor-Consumidor.

 Se produce una serie de números, guardados en un buffer intermedio, para que
 otro proceso los lea convenientemente y los muestre por pantalla
******************************************************************************/

#include <iostream>
#include <cassert>
#include <pthread.h>
#include <semaphore.h>

using namespace std ;

///////////////////////////////////////////////////////////////////////////////
// Constantes
//
const unsigned NUM_ITEMS = 40 , LENGTH_BUFFER = 1 ;  

///////////////////////////////////////////////////////////////////////////////
// Variables globales
//

// Vector intermedio con los datos que se van a ir generando y consumiendo
int buffer[LENGTH_BUFFER];

// Indican la primera posición que se encuentra con algún dato para leer 
// (occupied) y la primera posición para escribir (free). Las variables irán 
// aumentando desde 0 hasta LENGTH_BUFFER.
int occupied = 0, free = 0;

// Semáforo que controla si se puede consumir un dato. Debe comenzar en 0 
// porque hasta que no se produzca al menos 1, no se puede leer.
sem_t semC;

// Semáforo que controla si se puede producir un dato. Debe comenzar en
// LENGTH_BUFFER para ir disminuyendo según produzca.
sem_t semP;

///////////////////////////////////////////////////////////////////////////////
// Genera un número autoincremental comenzando en 1
// Devuelve el número generado
//
unsigned getData(){

	static int i = 1;

	cout << "Producido: " << i << "\n" << endl << flush ;

	return i++ ;

}

///////////////////////////////////////////////////////////////////////////////
// Escribe por pantalla el número que recibe por parámetro
//
void showData(int data){

	cout << "\t\tDato recibido: " << data << "\n" << endl << flush;

}

///////////////////////////////////////////////////////////////////////////////
// Función que ejecuta el productor
//
void* prod(void*){   

	for(unsigned i=0; i<NUM_ITEMS; i++){ 

		// Comprueba si existen huecos libres para producir. Si no existen
		// huecos libres, esperará hasta que el consumidor avise
	    	sem_wait(&semP);

		// Se guarda en la posición que indica free el número generado
		buffer[free] = getData();

		// Se avisa al consumidor que puede leer un dato más
		sem_post(&semC);

		// Se aumenta la posición donde debe escribir el productor
		free++;

		// Si ha llegado al final del buffer, debe seguir por el principio
		if(free%LENGTH_BUFFER==0)
			free=0;

	}

	return NULL;

}

///////////////////////////////////////////////////////////////////////////////
// Función que ejecuta el consumidor
//
void* cons(void *){   

  	for(unsigned i=0; i<NUM_ITEMS; i++){   
    
		// Se comprueba si se puede leer. Si no se puede leer esperará hasta
		// lo indique el productor
    		sem_wait(&semC);

		// Se muestra por pantalla la variable que corresponde a la posición
		// que apunta occupied
		showData(buffer[occupied]);

		// Se le indica al productor que puede producir un dato más
		sem_post(&semP);

		// Se aumenta el índice para leer del buffer
		occupied++;

		// Si ha llegado al final del buffer, debe empezar desde el principio
		if(occupied%LENGTH_BUFFER==0)
			occupied = 0;

	}

	return NULL;

}

///////////////////////////////////////////////////////////////////////////////
// main
//
int main(){

	// Se inicializan los semáforos
	sem_init(&semC, 0, 0);
	sem_init(&semP, 0, LENGTH_BUFFER);

	// Se definen las dos hebras
	pthread_t producer, consumer;

	// Se lanzan las hebras productor y consumidor
	pthread_create(&producer, NULL, prod, NULL);
	pthread_create(&consumer, NULL, cons, NULL);

	// Se espera a que terminen las dos hebras
	pthread_join(producer, NULL);
	pthread_join(consumer, NULL);

	// Se destruyen los semáforos
	sem_destroy(&semC);
	sem_destroy(&semP);

	// Se muestra un mensaje que indica la finalización del programa
	cout << "\nFIN" << endl;

	return 0 ; 

}
