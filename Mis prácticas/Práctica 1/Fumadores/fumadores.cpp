/******************************************************************************
 Francisco Javier Caracuel Beltrán
 2º B - Grado en Ingeniería Informática
 Sistemas Concurrentes y Distribuidos
 2015

 --

 Práctica 2.

 Práctica donde se resuelve el problema de los fumadores.

 Existe un estanquero que produce los elementos que necesitan los fumadores.
 Hay 3 fumadores y cada uno necesita un elemento diferente. Cuando el
 estanquero se los da, comienzan a fumar.
******************************************************************************/

#include <iostream>
#include <cassert>
#include <pthread.h>
#include <semaphore.h>
#include <string>
#include <time.h>      // incluye "time(....)"
#include <unistd.h>    // incluye "usleep(...)"
#include <stdlib.h>    // incluye "rand(...)" y "srand"

using namespace std;

///////////////////////////////////////////////////////////////////////////////
// Constantes
//
const unsigned int NUM_SMOKERS = 3;

///////////////////////////////////////////////////////////////////////////////
// Variables

// Vector de semáforos. Cada posición pertenece a un fumador. Debe estar
// inicializado a 0 para que esperen a que el estanquero produzca
sem_t semSmokers[NUM_SMOKERS];

// Semáforo del estanquero. Debe estar inicializado a 1 para que produzca el
// primer elemento
sem_t semTobacconist;

///////////////////////////////////////////////////////////////////////////////
// Función que simula la acción de fumar como un retardo aleatorio de la hebra
//
void smoke(){ 

	// Inicializa la semilla aleatoria  (solo la primera vez)
	static bool first = true;

	if (first){
 
		first = false;
		srand(time(NULL)); 
   
	}
   
   // Calcular un numero aleatorio de milisegundos (entre 1/10 y 2 segundos)
   const unsigned miliseg = 100U + (rand()%1900U); 

   // Retraso bloqueado durante 'miliseg' milisegundos
   usleep(1000U*miliseg); 

}

///////////////////////////////////////////////////////////////////////////////
// Devuelve que elemento ha producido
//
string getElement(unsigned int element){

	string name = "";

	switch(element){

		case 0:
			name = "cerillas";
			break;

		case 1:
			name = "tabaco";
			break;

		case 2:
			name = "papel";
			break;

	}

	return name;

}

///////////////////////////////////////////////////////////////////////////////
// Función que ejecuta el estanquero para proporcionar un elemento
//
void* takeElement(void*){

	while(true){

		// Si está preparado para coger un elemento lo coge, si no, espera
		// a que algún fumador lo avise.
		sem_wait(&semTobacconist);

		// Genera un número desde 0 hasta el número de fumadores, por
		// defecto, 3. 
		unsigned int element = rand()%NUM_SMOKERS;

		// Se muestra por pantalla que elemento ha producido.
		cout << "El estanquero ha suministrado " << getElement(element) 
			<< " al fumador " << element << "." << endl;

		// Desbloquea o indica que ya puede fumar un fumador aleatorio.
		sem_post(&semSmokers[element]);

	}

}

///////////////////////////////////////////////////////////////////////////////
// Función que ejecutan los fumadores
//
void* smoker(void* index){

	while(true){

		// Se indica que está esperando a recoger un elemento
		cout << "Fumador " << (unsigned long)index << " está esperando "
			<< getElement((unsigned long)index) << " para fumar." << endl;

		// Se espera a que esté disponible el elemento que necesita el 
		// fumador
		sem_wait(&semSmokers[(unsigned long)index]);

		// Aquí habrá cogido el elemento y se le avisa al estanquero que 
		// puede continuar
		sem_post(&semTobacconist);

		// Comienza a fumar
		cout << "Fumador " << (unsigned long)index << " comienza a fumar."
			<< endl;

		smoke();

		// Ha terminado de fumar
		cout << "Fumador " << (unsigned long)index << " ha terminado de fumar."
			<< endl;

	}

}

///////////////////////////////////////////////////////////////////////////////
// main
//
int main(){

	// Se inicializan los semáforos
	sem_init(&semTobacconist, 0, 1);

	for(unsigned int i=0; i<NUM_SMOKERS; i++)
		sem_init(&semSmokers[i], 0, 0);

	// Se definen las hebras
	pthread_t tobacconist, smokers[NUM_SMOKERS];

	// Se crean las hebras
	pthread_create(&tobacconist, NULL, takeElement, NULL);

	for(unsigned long i=0; i<NUM_SMOKERS; i++)
		pthread_create(&smokers[i], NULL, smoker, (void*)i);

	// Se esperan a todas las hebras
	pthread_join(tobacconist, NULL);

	for(unsigned int i=0; i<NUM_SMOKERS; i++)		
		pthread_join(smokers[i], NULL);

	// Se eliminan todos los semáforos
	sem_destroy(&semTobacconist);

	for(unsigned int i=0; i<NUM_SMOKERS; i++)
		sem_destroy(&semSmokers[i]);

	return 0 ;

}

