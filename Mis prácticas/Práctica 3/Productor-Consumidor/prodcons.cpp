#include <mpi.h>
#include <iostream>
#include <math.h>
#include <time.h>      // incluye "time"
#include <unistd.h>    // incluye "usleep"
#include <stdlib.h>    // incluye "rand" y "srand"

#define NUM_PRODUCTORES     5
#define NUM_CONSUMIDORES    4
#define BUFFER       5
#define PRODUCTOR    1  // Lo identifica en el paso de mensajes
#define CONSUMIDOR   2  // Lo identifica en el paso de mensajes
#define ITERS       20
#define TAM          5

using namespace std;

////////////////////////////////////////////////////////////////////////////////
// Función que ejecuta cada productor
//
void productor(){
    
    // Rank se utiliza para guardar el número del proceso
    int value, rank;
    
    // Se guarda en rank el número del proceso del productor
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    for (unsigned int i=0; i<ITERS/NUM_PRODUCTORES; i++) {
        
        value = i;
        cout << "Productor " << rank << " produce valor " 
                << value << endl << flush;

        // Espera bloqueado durante un intervalo de tiempo aleatorio 
        // (entre una décima de segundo y un segundo)
        usleep(1000U * (100U + (rand() % 900U)));

        // Enviar 'value'
        MPI_Ssend(&value, 1, MPI_INT, BUFFER, PRODUCTOR, MPI_COMM_WORLD);
        
    }
    
}

////////////////////////////////////////////////////////////////////////////////
// Función que ejecuta el proceso que regula los datos
//
void buffer() {
    
    int value[TAM], peticion, pos = 0, rama;
    MPI_Status status;

    for (unsigned int i = 0; i < ITERS * 2; i++) {
  
        // El consumidor no puede consumir
        if (pos == 0) 
            rama = 0;
        // El productor no puede producir
        else if (pos == TAM) 
            rama = 1;
        // Ambas guardas son ciertas
        else{ 
   
            // Leer 'status' del siguiente mensaje (esperando si no hay)
            MPI_Probe(MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &status);

            // Calcular la rama en función del origen del mensaje
            if (status.MPI_TAG == PRODUCTOR)
                rama = 0;
            else
                rama = 1;

        } 
        
        switch (rama){
            
            // Productor
            case 0:
              
                MPI_Recv(&value[pos], 1, MPI_INT, MPI_ANY_SOURCE, PRODUCTOR, MPI_COMM_WORLD, &status);
                cout << "Buffer recibe " << value[pos] << " de Productor " 
                        << status.MPI_SOURCE << endl << flush;
                pos++;
                break;
                
            // Consumidor
            case 1:
               
                MPI_Recv(&peticion, 1, MPI_INT, MPI_ANY_SOURCE, CONSUMIDOR, MPI_COMM_WORLD, &status);
            
                MPI_Ssend(&value[pos - 1], 1, MPI_INT, status.MPI_SOURCE, CONSUMIDOR, MPI_COMM_WORLD);
                cout << "Buffer envía " << value[pos - 1] << " a Consumidor " 
                        << status.MPI_SOURCE << endl << flush;
                pos--;
                break;
                
        }
        
    }
    
}

////////////////////////////////////////////////////////////////////////////////
// Proceso que ejecuta el consumidor
//
void consumidor() {
    
    int value, peticion = 1, rank;
    float raiz;
    MPI_Status status;
    
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    for (unsigned int i = 0; i < ITERS/NUM_CONSUMIDORES; i++) {
       
        MPI_Ssend(&peticion, 1, MPI_INT, BUFFER, CONSUMIDOR, MPI_COMM_WORLD);
        MPI_Recv(&value, 1, MPI_INT, BUFFER, CONSUMIDOR, MPI_COMM_WORLD, &status);
        cout << "Consumidor " << rank << " recibe valor " << value 
                << " de Buffer " << endl << flush;

        // Espera bloqueado durante un intervalo de tiempo aleatorio 
        // (entre una décima de segundo y un segundo)
        usleep(1000U * (100U + (rand() % 900U)));

        raiz = sqrt(value);
        
    }
    
}

////////////////////////////////////////////////////////////////////////////////
// main
//
int main(int argc, char *argv[]) {
    
    int rank, size;

    // Inicializar MPI, leer identif. de proceso y número de procesos
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    // Inicializa la semilla aleatoria:
    srand(time(NULL));

    // Comprobar el número de procesos con el que el programa 
    // ha sido puesto en marcha (debe ser 10)
    if (size != NUM_PRODUCTORES+NUM_CONSUMIDORES+1) {
        cout << "El numero de procesos debe ser " 
                << NUM_PRODUCTORES+NUM_CONSUMIDORES+1 << endl;
        return -1;
    }

    // Verificar el identificador de proceso (rank), y ejecutar la
    // operación apropiada a dicho identificador
    if (rank<NUM_PRODUCTORES)
        productor();
    // El buffer se ejecuta después de que los productores se hayan iniciado
    else if (rank==NUM_PRODUCTORES)
        buffer();
    else
        consumidor();

    // Al terminar el proceso, finalizar MPI
    MPI_Finalize();
    
    return 0;
    
}

