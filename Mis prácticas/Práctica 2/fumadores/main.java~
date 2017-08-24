///////////////////////////////////////////////////////////////////////////////
// Francisco Javier Caracuel Beltrán
// 2º B - B3 - Grado en Ingeniería Informática
// SCD 2015/2016
//
// Práctica 2. Fumadores
//
///////////////////////////////////////////////////////////////////////////////

import java.util.Random;
import monitor.*;

///////////////////////////////////////////////////////////////////////////////
// Clase Estanco. Contiene el monitor.
//
class Estanco extends AbstractMonitor{
    
    // Variable de instancia
    private int ingrediente;
    private int idIngrediente;
    
    // Variables condición
    Condition estanquero;
    Condition fumadores[];
    
    // Constructor
    public Estanco(int numFumadores){
    
    	// Se inicializa el ingrediente indicando que no existe ninguno sobre
    	// la mesa
    	idIngrediente = -1;
        
        estanquero = makeCondition();
        
    	fumadores = new Condition[numFumadores];
    	
    	for(int i=0; i<numFumadores; i++){
    		fumadores[i] = makeCondition();
    	}
        
    }
    
    ///////////////////////////////////////////////////////////////////////////
    // Método que ejecuta cada fumador indicando el ingrediente que necesita
    //
    public void obtenerIngrediente(int miIngrediente){
    
    	enter();
     
     	if(miIngrediente!=idIngrediente){
     		            
     	    	System.out.println("Fumador " + miIngrediente + " espera el ingrediente");
     
     		fumadores[miIngrediente].await();
     		
     	}
     	
     	System.out.println("Fumador " + miIngrediente + " ha cogido el ingrediente");
     	
     	System.out.println("Fumador " + miIngrediente + " está fumando...");
     	
     	idIngrediente = -1;
     	
     	estanquero.signal();
     	
        leave();
        
    }
    
    //////////////////////////////
    /////////////////////////////////////////////
    // Método que ejecuta el estanquero indicando el ingrediente que pone
    //
    public void ponerIngrediente(int ingrediente){
    
    	enter();
    	
    	System.out.println("/////////////////////////////////////////////////////////////////");
    	System.out.println("El estanquero pone el ingrediente " + ingrediente + " en la mesa");
    	
    	idIngrediente = ingrediente;
        
        fumadores[ingrediente].signal();
        
        leave();
        
    }
    
    ///////////////////////////////////////////////////////////////////////////
    // Método que ejecuta el estanquero cuando espera que se recoja el 
    // ingrediente
    //
    public void esperarRecogidaIngrediente(){
        
        enter();
        
        if(idIngrediente!=-1){
        	System.out.println("El estanquero espera...");
 	        estanquero.await();
 		}
        
        leave();
        
    }
    
}

///////////////////////////////////////////////////////////////////////////////
// Clase Fumador. Contiene cada instancia de un fumador
//
class Fumador implements Runnable{
    
    // Variable de instancia
    private int miIngrediente;
    
    // Monitor
    private Estanco estanco;
    
    // Hebra
    public Thread thr;

    // Constructor
    public Fumador(Estanco estanco, String nombre, int miIngrediente){
        
        this.estanco = estanco;
        this.miIngrediente = miIngrediente;
        thr = new Thread(this, nombre);
        
    }
    
    // Método que se ejecuta al inicio de su invocación
    @Override
    public void run() {
                
        // Bucle infinito
        while(true){

            // Se recoge el ingrediente que le corresponde de la mesa
            estanco.obtenerIngrediente(miIngrediente);
                                                     
            // Se pone a fumar 2 segundos
            aux.dormir_max(2000);
                        
     	    System.out.println("Fumador " + miIngrediente + " ha terminado de fumar...");
            
        }
        
    }
    
}

///////////////////////////////////////////////////////////////////////////////
// Clase Estanquero
//
class Estanquero implements Runnable{
    
    // Monitor
    private Estanco estanco;
    
    // Hebra
    public Thread thr;
    
    // Constructor
    public Estanquero(Estanco estanco, String nombre){
    	this.estanco = estanco;
    	thr = new Thread(this, nombre);
    }
    
    // Método que se ejecuta al crear el objeto
    @Override
    public void run(){
        
        int ingrediente;
        
        while(true){
            
            // Genera aleatoriamente un ingrediente para 0, 1 o 2
            ingrediente = new Random().nextInt(3);
            
            // Pone el ingrediente en la mesa
            estanco.ponerIngrediente(ingrediente);
                      
            // Espera a que recojan los ingrediente de la mesa
            estanco.esperarRecogidaIngrediente();
            
        }
        
    }
    
}

///////////////////////////////////////////////////////////////////////////////
// main
//
class main{
    
    public static final int NUM_FUMADORES = 3;
    
    public static void main(String[] args){
        
        // Se crea el monitor
	Estanco estanco = new Estanco(NUM_FUMADORES);

	// Se crean las hebras
	Estanquero estanquero = new Estanquero(estanco, "Estanquero");
	Fumador[] fumadores = new Fumador[NUM_FUMADORES];
	
	for(int i=0; i<NUM_FUMADORES; i++){
		fumadores[i] = new Fumador(estanco, "Fumador "+i, i);
	}
	
	// Se lanzan las hebras
	estanquero.thr.start();
	
	for(Fumador fum: fumadores){
		fum.thr.start();
	}
        
    }
    
}
