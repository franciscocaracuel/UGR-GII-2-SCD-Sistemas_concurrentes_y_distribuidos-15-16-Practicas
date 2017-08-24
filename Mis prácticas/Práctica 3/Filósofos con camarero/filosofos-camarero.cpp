////////////////////////////////////////////////////////////////////////////////
// 
// Francisco Javier Caracuel Beltrán
//
// 2º B - Grupo B3
//
// Grado en Ingeniería Informática - 2015/2016 - SCD
//
// Práctica 3. Filósofos con camarero
//
////////////////////////////////////////////////////////////////////////////////

#include <iostream>
#include <time.h>      // incluye "time"
#include <unistd.h>    // incluye "usleep"
#include <stdlib.h>    // incluye "rand" y "srand"
#include <mpi.h>

#define NUM_FILO_TOTAL 5
#define NUM_TENEDORES_TOTAL 5
#define NUM_CAMAREROS_TOTAL 1

#define ID_CAMARERO 10

#define COMER 1
#define PENSAR 2
#define SENTARSE 3
#define LEVANTARSE 4

using namespace std;

void Filosofo(int id, int nprocesos);
void Tenedor(int id, int nprocesos);
void Camarero(int id, int nprocesos);

////////////////////////////////////////////////////////////////////////////////
// main
//

int main(int argc, char** argv) {

    int rank, size;

    srand(time(0));

    MPI_Init(&argc, &argv);

    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    
    cout << endl << "Proceso " << rank << " en ejecución..." << endl << flush;

    if (size != NUM_FILO_TOTAL + NUM_TENEDORES_TOTAL + NUM_CAMAREROS_TOTAL) {

        if (rank == 0)
            cout << "El numero de procesos debe ser " << NUM_FILO_TOTAL + NUM_TENEDORES_TOTAL + NUM_CAMAREROS_TOTAL << endl << flush;

        MPI_Finalize();

        return 0;

    }
    
    // El último será el proceso camarero
    if(rank==ID_CAMARERO)
        Camarero(rank, size);
    else{

        if ((rank % 2) == 0)
            Filosofo(rank, size); // Los pares son Filosofos 
        else
            Tenedor(rank, size); // Los impares son Tenedores 
    
    }
    
    MPI_Finalize();

    return 0;

}

////////////////////////////////////////////////////////////////////////////////
// Filósofo
//

void Filosofo(int id, int nprocesos) {

    int izq = (id + 1) % (nprocesos - 1);
    int der = ((id + nprocesos) - 2) % (nprocesos - 1);
    MPI_Status status;

    while (1) {
        
        // Solicita sentarse
        cout << "****Filosofo " << id << " solicita sentarse..." << endl << flush;
        MPI_Ssend(&izq, 1, MPI_INT, ID_CAMARERO, SENTARSE, MPI_COMM_WORLD);
        
        // Recibe el permiso para sentarse
        MPI_Recv(&izq, 1, MPI_INT, ID_CAMARERO, SENTARSE, MPI_COMM_WORLD, &status);
        cout << "****Filosofo " << id << " se ha sentado..." << endl << flush;

        // Solicita tenedor izquierdo
        cout << "Filosofo " << id << " solicita tenedor izq ..." << izq << endl << flush;
        MPI_Ssend(&izq, 1, MPI_INT, izq, COMER, MPI_COMM_WORLD);

        // Solicita tenedor derecho
        cout << "Filosofo " << id << " coge tenedor der ..." << der << endl << flush;
        MPI_Ssend(&der, 1, MPI_INT, der, COMER, MPI_COMM_WORLD);

        cout << "Filosofo " << id << " COMIENDO" << endl << flush;
        sleep((rand() % 3) + 1); //comiendo

        // Suelta el tenedor izquierdo
        cout << "Filosofo " << id << " suelta tenedor izq ..." << izq << endl << flush;
        MPI_Ssend(&izq, 1, MPI_INT, izq, PENSAR, MPI_COMM_WORLD);

        // Suelta el tenedor derecho
        cout << "Filosofo " << id << " suelta tenedor der ..." << der << endl << flush;
        MPI_Ssend(&der, 1, MPI_INT, der, PENSAR, MPI_COMM_WORLD);
        
        // Solicita levantarse
        cout << "****Filosofo " << id << " ha terminado de comer..." << endl << flush;
        MPI_Ssend(&izq, 1, MPI_INT, ID_CAMARERO, LEVANTARSE, MPI_COMM_WORLD);
        
        // Recibe el permiso para levantarse
        MPI_Recv(&izq, 1, MPI_INT, ID_CAMARERO, LEVANTARSE, MPI_COMM_WORLD, &status);
        cout << "****Filosofo " << id << " se ha levantado..." << endl << flush;
        
        // Piensa (espera bloqueada aleatorio del proceso)
        cout << "Filosofo " << id << " PENSANDO" << endl << flush;

        // espera bloqueado durante un intervalo de tiempo aleatorio 
        usleep(5000U * (100U + (rand() % 900U)));

    }

}

////////////////////////////////////////////////////////////////////////////////
// Tenedor
//

void Tenedor(int id, int nprocesos) {

    int buf;
    MPI_Status status;
    int filo;

    while (1) {

        // Espera una peticion desde cualquier filósofo vecino ...
        MPI_Recv(&buf, 1, MPI_INT, MPI_ANY_SOURCE, COMER, MPI_COMM_WORLD, &status);

        // Recibe la peticion del filósofo ...
        filo = status.MPI_SOURCE;

        cout << "Ten. " << id << " recibe petic. de " << filo << endl << flush;

        // Espera a que el filosofo suelte el tenedor...
        MPI_Recv(&buf, 1, MPI_INT, filo, PENSAR, MPI_COMM_WORLD, &status);
        cout << "Ten. " << id << " recibe liberac. de " << filo << endl << flush;

    }
}

////////////////////////////////////////////////////////////////////////////////
// Camarero
//

void Camarero(int id, int nprocesos) {

    int buf;
    int numFilo = 0;
    MPI_Status status;

    while (1) {
        
        cout << endl << "*****Número de filósofos sentados en la mesa: " << numFilo << endl << flush;
        
        // Espera una peticion desde cualquier filósofo...
        // Si pueden sentarse filósofos recibirá cualquiera
        if (numFilo < NUM_FILO_TOTAL-1)
            MPI_Probe(MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
        else
            // Si solo pueden levantarse recibe la de levantarse
            MPI_Probe(MPI_ANY_SOURCE, LEVANTARSE, MPI_COMM_WORLD, &status);
        
        // Si ha llegado hasta aquí puede tanto sentarse como levantarse
        if (status.MPI_TAG == SENTARSE){
            
            int pet = status.MPI_SOURCE;
            
            // Se recibe la petición de sentarse
            MPI_Recv(&buf, 1, MPI_INT, pet, SENTARSE, MPI_COMM_WORLD, &status);
            
            // Se aumenta el número de filósofos
            numFilo++;

            // Se le da permiso para sentarse
            MPI_Send(&buf, 1, MPI_INT, pet, SENTARSE, MPI_COMM_WORLD);
           
        }else{
            
            int pet = status.MPI_SOURCE;
            
            // Se recibe la petición de levantarse
            MPI_Recv(&buf, 1, MPI_INT, pet, LEVANTARSE, MPI_COMM_WORLD, &status);
            
            // Se disminuye el número de filósofos
            numFilo--;
            
            // Se le da permiso para levantarse
            MPI_Send(&buf, 1, MPI_INT, status.MPI_SOURCE, LEVANTARSE, MPI_COMM_WORLD);

        }

    }
}