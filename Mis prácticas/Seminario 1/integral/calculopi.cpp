// *********************************************************************
// SCD. Ejemplos del seminario 1.
//
// Plantilla para el ejercicio de cálculo de PI
// *********************************************************************

#include <iostream>
#include <cstdlib>
#include <pthread.h>
#include "fun_tiempo.h"

using namespace std;

// ---------------------------------------------------------------------
// constante y variables globales (compartidas entre hebras)

unsigned long m; // número de muestras
unsigned long n; // número de hebras

double* resultado_parcial; // tabla de sumas parciales (una por hebra)

// ---------------------------------------------------------------------
// implementa función $f$

double f(double x) {
    return 4.0 / (1 + x * x); // $~~~~f(x)\,=\,4/(1+x^2)$
}
// ---------------------------------------------------------------------
// cálculo secuencial

double calcular_integral_secuencial() {
    double suma = 0.0; // inicializar suma
    for (unsigned long i = 0; i < m; i++) // para cada $i$ entre $0$ y $m-1$
        suma += f((i + 0.5) / m); // $~~~~~$ añadir $f(x_i)$ a la suma actual
    return suma / m; // devolver valor promedio de $f$
}
// ---------------------------------------------------------------------
// función que ejecuta cada hebra

void * funcion_hebra(void * ih_void) {
    
    unsigned long ih = (unsigned long) ih_void; // número o índice de esta hebra
    double sumap = 0.0;
    
    // Recorre todos los valores de n en n
    for(unsigned long i=ih; i<m; i+=n){
        sumap += f((i + 0,5) / m);
    }
    
    resultado_parcial[ih] = sumap; // guardar suma parcial en vector.
    
    return NULL;
    
}
// ---------------------------------------------------------------------
// cálculo concurrente

double calcular_integral_concurrente() {
    
    // Resultado total
    double res = 0.0;   
    
    // crear y lanzar $n$ hebras, cada una ejecuta "funcion\_concurrente"
    pthread_t id_hebra[n];
   
    for(unsigned long i=0; i<n; i++){
        pthread_create(&(id_hebra[i]), NULL, funcion_hebra, (void *)i);
    }
    
    // esperar (join) a que termine cada hebra, sumar su resultado
    for(unsigned long i=0; i<n; i++){
        pthread_join(id_hebra[i], NULL);
        res += resultado_parcial[i];
    }
    
    return res/m;
    
}

// ---------------------------------------------------------------------

int main(int argc, char *argv[]) {

    // Necesita 2 argumentos
    if (argc == 3) {
        
        // Se leen los argumentos recibidos
        n = atol(argv[1]);
        m = atol(argv[2]);
        
        // Se comprueba que m sea múltiplo de n
        if(m%n!=0){
            cout << "Error: num_muestras debe ser múltiplo de num_hebras\n\n";
            return 1;
        }
        
        // Se inicializa el vector para los resultados
        resultado_parcial = new double[n];
        
        cout << "\nCálculo de PI - Francisco J. Caracuel Beltrán" << endl;
        double pi_sec = 0.0, pi_conc = 0.0;
        
        struct timespec inicio_sec = ahora();
        pi_sec = calcular_integral_secuencial();        
        struct timespec fin_sec = ahora();
                
        struct timespec inicio_con = ahora();
        pi_conc = calcular_integral_concurrente();        
        struct timespec fin_con = ahora();
        
        cout << "valor de PI (calculado secuencialmente)  == " << pi_sec 
                    << " en " << duracion(&inicio_sec, &fin_sec) << " segundos " << endl
                << "valor de PI (calculado concurrentemente) == " << pi_conc 
                    << " en " << duracion(&inicio_con, &fin_con) << " segundos " << endl << endl;
        
        delete resultado_parcial;

        return 0;

    } else {
        
        cout << "\nIntroduce 2 parámetros: \n\n" << argv[0] << " num_hebras num_muestras\n\n";
        
        return 2;
    }

}
// ---------------------------------------------------------------------



// ----------------------------------------------------------------------------
