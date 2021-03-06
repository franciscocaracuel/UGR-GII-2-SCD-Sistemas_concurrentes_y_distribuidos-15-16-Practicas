///////////////////////////////////////////////////////////////////////////////
// Francisco Javier Caracuel Beltrán
// 2º B - B3 - Grado en Ingeniería Informática
// SCD 2015/2016
//
// Práctica 2. Barbero durmiente
//
///////////////////////////////////////////////////////////////////////////////

import java.util.Random;
import monitor.*;

///////////////////////////////////////////////////////////////////////////////
// Clase Barbería que contiene el monitor para la gestión de las hebras
//
class Barberia extends AbstractMonitor{
    
    boolean pelando;
    
    Condition barbero, clienteEsperando, clientePelando;
    
    // Se utiliza esta variable para saber a modo informativo cuantos clientes
    // están esperando el la barbería
    int numCLientes = 0;
    
    ///////////////////////////////////////////////////////////////////////////
    // Constructor
    //
    public Barberia(){
                
        pelando = false;
        
        barbero = makeCondition();
        
        clienteEsperando = makeCondition();
        clientePelando = makeCondition();
                
    }
    
    ///////////////////////////////////////////////////////////////////////////
    // Invocado por los clientes para cortarse el pelo
    //
    public void cortarPelo(){
        
        enter();
       
        if(pelando){
            System.out.println("Entra un cliente y el barbero está pelando. El cliente espera en la sala.");
            numCLientes++;
            System.out.println("Hay " + numCLientes + " clientes esperando...");
            clienteEsperando.await();
        } else{
            System.out.println("El cliente entra y no hay clientes esperando. Despierta al barbero y se espera.");
            barbero.signal();
            clientePelando.await();
        }
        
        leave();
        
    }
    
    ///////////////////////////////////////////////////////////////////////////
    // Invocado por el barbero para esperar a un nuevo cliente y sentarlo 
    // para el corte
    //
    public void siguienteCliente(){
        
        enter();
        
        System.out.println("El barbero busca a un cliente.");
        
        if(!clienteEsperando.isEmpty()){
            System.out.println("El barbero ha encontrado un cliente en la sala y empieza a pelarlo.");
            numCLientes--;
            System.out.println("Quedan " + numCLientes + " clientes esperando...");
            clienteEsperando.signal();
        } else if(clientePelando.isEmpty()){
            System.out.println("El barbero no tiene clientes, espera a que lleguen...");
            barbero.await();
            System.out.println("El barbero se despierta y empieza a pelar al cliente.");
        } else{
            System.out.println("El cliente ha llegado antes que el barbero y ya está preparado. El barbero empieza a pelar.");
        }
        
        pelando = true;
                
        leave();
                
    }
    
    ///////////////////////////////////////////////////////////////////////////
    // Invocado por el barbero para indicar que ha terminado de cortar el pelo
    //
    public void finCliente(){
        
        enter();
        
        System.out.println("El barbero ha terminado de cortar el pelo al cliente.");
        
        clientePelando.signal();
        pelando = false;
        
        leave();
        
    }
    
}

///////////////////////////////////////////////////////////////////////////////
// Clase Cliente
//
class Cliente implements Runnable{

    // Hebra
    public Thread thr;
    
    // Monitor
    private Barberia barberia;
    
    ///////////////////////////////////////////////////////////////////////////
    // Constructor
    //
    public Cliente(Barberia barberia, String cliente){
        
        this.barberia = barberia;
        thr = new Thread(this, cliente);
        
    }
    
    ///////////////////////////////////////////////////////////////////////////
    // Método que se ejecuta al inicio de la hebra
    //
    @Override
    public void run() {
        
        while(true){
            
            // El cliente esperará si procede y se corta el pelo
            barberia.cortarPelo();
            
            // El cliente espera fuera de la barbería
            aux.dormir_max(2000);
            
        }
        
    }
    
}

///////////////////////////////////////////////////////////////////////////////
// Clase Barbero
//
class Barbero implements Runnable{

    // Hebra
    public Thread thr;
    
    // Monitor
    Barberia barberia;
    
    public Barbero(Barberia barberia, String barbero){
        
        this.barberia = barberia;
        
        thr = new Thread(this, barbero);
        
    }
    
    ///////////////////////////////////////////////////////////////////////////
    // Método que se ejecuta al inicio de la hebra
    //
    @Override
    public void run() {
        
        while(true){
            
            barberia.siguienteCliente();
            
            // El barbero está cortando el pelo
            aux.dormir_max(2500);
            
            barberia.finCliente();
            
        }
        
    }
    
}

///////////////////////////////////////////////////////////////////////////////
// main
//
class main{
    
    public static final int NUM_CLIENTES = 100000;
    
    public static void main(String[] args) {
        
        // Se crea el monitor
        Barberia barberia = new Barberia();
        
        // Se crean las hebras
        Barbero barbero = new Barbero(barberia, "Barbero");
        
        Cliente[] clientes = new Cliente[NUM_CLIENTES];
        for(int i=0; i<NUM_CLIENTES; i++){
            clientes[i] = new Cliente(barberia, "Cliente " + i);
        }
        
        System.out.println("\n\nEjercicio del barbero durmiente\n\n");
        
        // Se lanzan las hebras
        barbero.thr.start();
        
        for(int i=0; i<NUM_CLIENTES; i++){
            clientes[i].thr.start();
        }
        
    }
    
}
