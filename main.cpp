#include <iostream>
#include <fstream>
#include <math.h>
#include <time.h>
#include <ctime> 

using namespace std;


//Declaracion de funciones.

//Funciones SA
float getProbTemp(float T, float evalFirst, float evalSecond );
float objetivefunction(int *solution);
int *randomSolution(int N);
int *simulatedAnnealing(int *initSolution, float initT, float varT, int iterT, float randProb, int nIter);

//Funciones de ayuda
void showProb();
void showArr(int *arrayNum);
void freeMemoria();
void copyArray(int *entryArray, int *finalArray);
bool inArray( int* arrayNumber, int n,int number);



//Variables Globales
int N; // Numero de armas, objetivos => Tamaño de matriz cuadrada, tamaño de solucion
int *destroyValues;
float **probMat;



int main(int argc, char *argv[]) {
    
   
  unsigned t0 = clock(); // Inicio Ejecucion

   
   /////////////////////////////////////////////////////////////////////////////////
   ///////////////////// Read File /////////////////////////////////////////////////
   /////////////////////////////////////////////////////////////////////////////////
   
    //get parametros
    string fileName = argv[1];    
    ifstream file(fileName);


    // Get N
    file >> N;
 
   
    //Initialize matriz
    probMat =  new float*[N];
    for (int i = 0; i< N ; i++ ){
        probMat[i] = new float[N];
    } 
  
   
    //Get Values Destruction 
    destroyValues = new int[N];
    for(int i = 0; i < N; i++){
        file >> destroyValues[i] ;
    }
    
    //Get Probabilities
    for (int i = 0; i < N; i++){
        for (int j = 0; j < N ; j++){
            file >> probMat[j][i];
        }
    }
  



    //////////////////////////////////////////////////////////////////////////////
    ///////////////////////////// Simulated Annealing ////////////////////////////
    //////////////////////////////////////////////////////////////////////////////


    // Parametros
    
    float initT = 100;   // Temperatura inicial 
    float varT = 0.1;     // Razon de disminucion de Temperatura
    int iterT = 100;     // Cada cuantos iteraciones varia Temperatura
    float randProb = 0.5; //Random a comparar con probabilidad
    int nIter = 10000000;      // Numero iteraciones totales

  
    
     // Crear con solucion inicial aleatoria
    int *initSolution = new int[N];
    initSolution = randomSolution(N);

    //Mejor solucion encontrada
    int *finalSolution = new int[N];
    finalSolution = simulatedAnnealing(initSolution, initT, varT, iterT, randProb, nIter);
    
      

    //print 
    /*cout << "probMat" ;
    showProb();
    cout << "destroyValues" << endl; 
    showArr(destroyValues);
    */
     
    //Solucion inicial junto a su valor de evaluacion
    cout << "Init Solution" << endl;
    showArr(initSolution);
    cout << "Obj: " << objetivefunction(initSolution) << endl;

    cout << endl;
    
     //Mejor solucion  junto a su valor de evaluacion
    cout << "Final Solution" << endl;
    showArr(finalSolution);
    cout << "Obj: " << objetivefunction(finalSolution) << endl;
   
    cout << endl;
   
    // Liberar memoria pedida memoria
    freeMemoria();
    delete initSolution;
    delete[] finalSolution;

    
    //Clock
    unsigned t1 = clock(); // Termino Ejecucion 

    double time = (double(t1-t0)/CLOCKS_PER_SEC);
    cout << "Execution Time: " << time << "[s]"<< endl;
    

}

/////////////////////////////////////////////////////////////////////////////
//////////////////////////// Functions //////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////


//Funcion que calcula el valor en la funcion de evaluacionde una solucion 
float objetivefunction(int *solution){
    float sum = 0;
    for(int i = 0; i < N; i++){
        float val = destroyValues[i] * (1 - probMat[solution[i]][i]);
        sum = sum + val;
    }
    return sum; 
}


//Obtiene la probabilidad para que una solucion se acepte sin ser mejor que la anterior
float getProbTemp(float T, float lastsolution, float thissolution ){
    float delta = lastsolution - thissolution;
    float prob = exp(delta/T);
    return prob;
}


//Genera solucion inicial aleatoria
int *randomSolution(int N){
    int *randomSolution = new int[N];
    srand (time(NULL));
    for(int i = 0; i < N; i++) randomSolution[i] = -1;
    
    for(int i = 0; i < N; i++){
        int number = rand() % N;
        while(inArray(randomSolution, N, number)) number = rand() % N;
        randomSolution[i] = number;
    }
    return randomSolution;
}   


//Funcion que busca la mejor solucion siguiendo la heuristica de Simulated Annelaing
int *simulatedAnnealing(int *initSolution, float initT, float varT, int iterT, float randProb, int nIter){
    int *bestSolution = new int[N];  //Almacena la mejor solucion por iteracion
    float minLocal = 999999;  //Almacena el valor de la mejor solucion

    int *thisSolution = new int[N]; //Almacena vecino generado
    int *sonSolution = new int[N]; //Almacena soluciones de otras iteraciones
    copyArray(initSolution, thisSolution); //Copia
    
    if(nIter == 0) return thisSolution; // Terminar el ciclo cuando las iteraciones terminen
    
    //Creamos el vecindario
    for (int i = 0; i < N; i++){
        ///// Create a new solution -- SWAP
        if (i == (N - 1)){
            thisSolution[0] = initSolution[i];
            thisSolution[i] = initSolution[0];
        } 
        else {
            thisSolution[i] = initSolution[i+1];
            thisSolution[i+1] = initSolution[i];
        }

    
        ////////////////// Observar comportamiento de Metodo ///////////////////////
        
        /*
        cout << "Temp  varTemp  iterTemp  randProb nIter" << endl;
        cout << initT << "    "<< varT << "   " << iterT << "   " << randProb << "   " <<  nIter << endl;
        cout << "Solucion" << endl;
        showArr(thisSolution);
        cout << "Obj: " << objetivefunction(thisSolution) << endl;
       */
         
        //////////// Compare Solutions //////////////////
        
        if (objetivefunction(thisSolution) < objetivefunction(initSolution)){ //Si la nueva solucion es mejor que la anterior
            //cout << endl << "Por mejor" << endl;
            nIter = nIter - 1; //Descuenta  Iteraciones
            if(nIter % iterT == 0) initT = initT * (1 -varT);
            sonSolution =  simulatedAnnealing(thisSolution, initT, varT, iterT, randProb, nIter); //Tomamos la solucion y realizamos las siguientes iteraciones
            break;  //Rompemos el ciclo y evitamos la creacion de los vecinos restantes innecesarios (Alguna Mejora)
        }
        
        
        else { //Si la nueva solucion es peor que la anterior
            //Puede seleccionarse segun la probabilidad
            if (getProbTemp(initT, objetivefunction(initSolution), objetivefunction(thisSolution)) > randProb){  
                nIter = nIter - 1; //Descuenta iteraciones
                
                //showArr(thisSolution);
                //cout << endl << "Por probabilidad" << endl;
                //cout << "Probabilidad: " << getProbTemp(initT, objetivefunction(initSolution), objetivefunction(thisSolution)) << endl;
                
                if(nIter % iterT == 0 && iterT != nIter) initT = initT * (1 -varT); //Actualizamos temperatura segun las iteraciones que se han realizado
                sonSolution =  simulatedAnnealing(thisSolution, initT, varT, iterT, randProb, nIter); //Tomamos la solucion y realizamos las siguientes iteraciones
                break; //Rompemos el ciclo y evitamos la creacion de los vecinos restantes innecesarios (Alguna Mejora)
            }
            
            //No se selecciona -> Seguimos con el siguiente vecino
            else copyArray(initSolution, thisSolution); //Se resetea el arreglo para crear siguiente vecino         
        }     
    }

    //Comparamos el mejor valor de los vecinos para almacenar la mejor
    if(objetivefunction(thisSolution) < minLocal){
            minLocal = objetivefunction(thisSolution);
            copyArray(thisSolution,bestSolution);
        }    
    //Comparamos los valores de las iteraciones para almacenar la mejor
    if(objetivefunction(sonSolution) < minLocal){
        minLocal = objetivefunction(thisSolution);
        copyArray(sonSolution,bestSolution);
        }
    
    return bestSolution; //Retornamos la mejor solucion

}









///////////////////////////////////////////////////////////////////////////
//////////////////////////// Tools Functions //////////////////////////////
//////////////////////////////////////////////////////////////////////////

//Muestra matriz de probabilidades NxN
void showProb(){
    cout << "           " ;
    for (int i = 0; i < N; i++){
        cout << "\tArma " << i;
    }
    cout << endl;

    for (int i = 0; i < N; i++){
        cout << "Objetivo " << i << "\t";
        for (int j = 0; j < N ; j++){
            cout << probMat[i][j] << "\t";
        };
        cout << endl;
    };

    cout << endl; 
}

//Muestra arreglo de tamaño N
void showArr(int *arrayNum){
    for (int i = 0; i < N; i++){
        cout << arrayNum[i] << "\040";
    }
    cout  << endl; 
}

//libera Memoria
void freeMemoria(){
    for (int i = 0; i < N ; i++){
        delete[] probMat[i];
    };
    delete[] probMat;
    delete[] destroyValues;
    
}

//Copia un arreglo a otro
void copyArray(int *entryArray, int *finalArray){
        for(int i = 0; i < N; i++){
            finalArray[i] = entryArray[i];
        }
    } 

//Verifica si elemento se encuentra en arreglo
bool inArray( int* arrayNumber, int n,int number){
    for (int i = 0; i < n; i++) if (arrayNumber[i] == number) return true;
    return false;
}




