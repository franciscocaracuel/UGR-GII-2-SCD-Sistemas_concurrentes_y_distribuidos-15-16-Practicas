////////////////////////////////////////////////////////////////////////////////
// 
// Francisco Javier Caracuel Beltrán
//
// 2º B - Grupo B3
//
// Grado en Ingeniería Informática - 2015/2016 - SCD
//
// Práctica 3. Filósofos
//
////////////////////////////////////////////////////////////////////////////////

#include <iostream>
#include <time.h>      // incluye "time"
#include <unistd.h>    // incluye "usleep"
#include <stdlib.h>    // incluye "rand" y "srand"
#include <mpi.h>

#define COMER 1
#define PENSAR 2

using namespace std;

void Filosofo(int id, int nprocesos);
void Tenedor(int id, int nprocesos);

////////////////////////////////////////////////////////////////////////////////
// main
//
int main(int argc, char** argv) {

    int rank, size;

    srand(time(0));

    MPI_Init(&argc, &argv);

    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    if (size != 10) {

        if (rank == 0)
            cout << "El numero de procesos debe ser 10" << endl << flush;

        MPI_Finalize();

        return 0;

    }

    if ((rank % 2) == 0)
        Filosofo(rank, size); // Los pares son Filosofos 
    else
        Tenedor(rank, size); // Los impares son Tenedores 

    MPI_Finalize();

    return 0;

}

////////////////////////////////////////////////////////////////////////////////
// Filósofo
//
void Filosofo(int id, int nprocesos) {

    int izq = (id + 1) % nprocesos;
    int der = ((id + nprocesos) - 1) % nprocesos;

    while (1) {
        
        switch(id){
            
            case 0:
                
                // Solicita tenedor derecho
                cout << "Filosofo " << id << " coge tenedor der ..." << der << endl << flush;
                MPI_Ssend(&der, 1, MPI_INT, der, COMER, MPI_COMM_WORLD);
                
                // Solicita tenedor izquierdo
                cout << "Filosofo " << id << " solicita tenedor izq ..." << izq << endl << flush;
                MPI_Ssend(&izq, 1, MPI_INT, izq, COMER, MPI_COMM_WORLD);
                
                break;
                
            default:
                
                // Solicita tenedor izquierdo
                cout << "Filosofo " << id << " solicita tenedor izq ..." << izq << endl << flush;
                MPI_Ssend(&izq, 1, MPI_INT, izq, COMER, MPI_COMM_WORLD);

                // Solicita tenedor derecho
                cout << "Filosofo " << id << " coge tenedor der ..." << der << endl << flush;
                MPI_Ssend(&der, 1, MPI_INT, der, COMER, MPI_COMM_WORLD);
            
        }

        cout << "Filosofo " << id << " COMIENDO" << endl << flush;
        sleep((rand() % 3) + 1); //comiendo

        // Suelta el tenedor izquierdo
        cout << "Filosofo " << id << " suelta tenedor izq ..." << izq << endl << flush;
        MPI_Ssend(&izq, 1, MPI_INT, izq, PENSAR, MPI_COMM_WORLD);

        // Suelta el tenedor derecho
        cout << "Filosofo " << id << " suelta tenedor der ..." << der << endl << flush;
        MPI_Ssend(&der, 1, MPI_INT, der, PENSAR, MPI_COMM_WORLD);

        // Piensa (espera bloqueada aleatorio del proceso)
        cout << "Filosofo " << id << " PENSANDO" << endl << flush;

        // espera bloqueado durante un intervalo de tiempo aleatorio 
        // (entre una décima de segundo y un segundo)
        usleep(1000U * (100U + (rand() % 900U)));

    }

}

////////////////////////////////////////////////////////////////////////////////
// Tenedor
//
void Tenedor(int id, int nprocesos) {

    int buf;
    MPI_Status status;
    int filo;

    while (true) {
        
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
