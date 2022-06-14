#include <iostream>
#include <fstream>
#include <math.h>
#include <time.h>
#include <ctime> 


//Declaracion de funciones.
void initSolutionFunc(int *initSolution, int N);
float getProbTemp(float T, float evalFirst, float evalSecond );
float objetivefunction(int *solution);
int *randomSolution(int N);
int *simulatedAnnealing(int *initSolution, float initT, float varT, int iterT, float randProb, int nIter);

//Funciones de ayuda
void showProb();
void show(int *arrayNum);
void fillArray(int* array, int N);
void freeMemoria();
void copyArray(int *entryArray, int *finalArray);
bool inArray( int* arrayNumber, int n,int number);



using namespace std;

//Variables Globales
int N; // Numero de armas, objetivos => Tamaño de matriz cuadrada, tamaño de solucion
int *destroyValues;
float **probMat;



int main(int argc, char *argv[]) {
    
   
  unsigned t0 = clock(); // Inicio Codigo

   
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
  
    destroyValues = new int[N];
    //Get Values Destruction 
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


    
    
    float initT = 100;   // Temperatura inicial 
    float varT = 0.1;     // Razon de disminucion de Temperatura
    int iterT = 100;     // Cada cuantos iteraciones varia Temperatura
    float randProb = 0.9; //Random a comparar con probabilidad
    int nIter = 100000;      // Numero iteraciones totales

  
    
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
    show(destroyValues);
    */
     
    //Solucion inicial junto a su valor de evaluacion
    cout << "Init Solution" << endl;
    show(initSolution);
    cout << "Obj: " << objetivefunction(initSolution) << endl;
    
     //Mejor solucion  junto a su valor de evaluacion
    cout << "Final Solution" << endl;
    show(finalSolution);
    cout << "Obj: " << objetivefunction(finalSolution) << endl;
   
   
   
    // Liberar memoria pedida memoria
    freeMemoria();
    delete initSolution;
    delete[] finalSolution;

    
    //Clock
    unsigned t1 = clock(); // Termino codigo

    double time = (double(t1-t0)/CLOCKS_PER_SEC);
    cout << "Execution Time: " << time << "[s]"<< endl;
    

}

/////////////////////////////////////////////////////////////////////////////
//////////////////////////// Functions //////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////


//Funcion que calcula el valor de una solucion 
float objetivefunction(int *solution){
    float sum = 0;
    for(int i = 0; i < N; i++){
        float val = destroyValues[i] * (1 - probMat[solution[i]][i]);
        sum = sum + val;
    }
    return sum; 
}


//Obtiene la probabilidad para que una solucion se acepte sin ser mejor que la anterior
float getProbTemp(float T, float evalFirst, float evalSecond ){
    float prob = exp((-1)*(evalSecond/evalFirst)/T);
    return prob;
}


//Genera solucion aleatoria
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

    int *thisSolution = new int[N]; //Almacena vecino generados en esta iteracion
    int *sonSolution = new int[N]; //Almacena soluciones de otras iteraciones
    copyArray(initSolution, thisSolution);
    

    
    if(nIter == 0) return thisSolution; // Terminar el ciclo cuando las iteraciones terminen
    
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
        show(thisSolution);
        cout << "Obj: " << objetivefunction(thisSolution) << endl;
        */
       
       
        //////////// Compare Solutions //////////////////
        if (objetivefunction(thisSolution) < objetivefunction(initSolution)){
            //cout << endl << "Por mejor" << endl;
            nIter = nIter - 1; //Descuenta  Iteraciones
            if(nIter % iterT == 0) initT = initT * (1 -varT);
            sonSolution =  simulatedAnnealing(thisSolution, initT, varT, iterT, randProb, nIter); 
            break;
        }
        else {
            if (getProbTemp(initT, objetivefunction(initSolution), objetivefunction(thisSolution)) > randProb){ //Obtiene probabilidad 
               // cout << "Probabilidad: " << getProbTemp(initT, objetivefunction(initSolution), objetivefunction(thisSolution)) << endl;
                //cout << endl << "Por probabilidad" << endl;
                
                nIter = nIter - 1; //Descuenta iteraciones
                if(nIter % iterT == 0 && iterT != nIter) initT = initT * (1 -varT);
                sonSolution =  simulatedAnnealing(thisSolution, initT, varT, iterT, randProb, nIter);
                break;
            }
            else { 
                copyArray(initSolution, thisSolution);
                //cout << endl << "Siguiente Vecino" << endl;
                continue;
            }
        }     

       

    }

    if(objetivefunction(thisSolution) < minLocal){
            minLocal = objetivefunction(thisSolution);
            copyArray(thisSolution,bestSolution);
        }    
    
    if(objetivefunction(sonSolution) < minLocal){
            minLocal = objetivefunction(thisSolution);
            copyArray(sonSolution,bestSolution);
        }
    
    return bestSolution;

}









///////////////////////////////////////////////////////////////////////////
//////////////////////////// Tools Functions //////////////////////////////
//////////////////////////////////////////////////////////////////////////

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

void show(int *arrayNum){
    for (int i = 0; i < N; i++){
        cout << arrayNum[i] << "\040";
    }
    cout  << endl; 
}

void freeMemoria(){
    for (int i = 0; i < N ; i++){
        delete[] probMat[i];
    };
    delete[] probMat;
    delete[] destroyValues;
    
}

  void copyArray(int *entryArray, int *finalArray){
        for(int i = 0; i < N; i++){
            finalArray[i] = entryArray[i];
        }
    }


void fillArray(int* array, int N){
    cout << "Ingrese " << N << " Valores" << endl;
    for(int i = 0; i < N; i++){
        cin >> array[i];
    }    
    cout << endl;
}

bool inArray( int* arrayNumber, int n,int number){
    for (int i = 0; i < n; i++) if (arrayNumber[i] == number) return true;
    return false;
}







