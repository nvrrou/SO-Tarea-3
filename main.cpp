#include <iostream>
#include <vector>
#include <random>
#include <cmath>
#include <thread>
#include <atomic>
#include <chrono>
#include <condition_variable>
#include <mutex>

using namespace std;


double tFisica = 0;     // se inserta en MB pero se ve en B, 
double tVirtual = 0;    // de 1.5 a 4 veces la fisica (aleatorio)
double tPagina = 0;     // same as "fisica".

int prox_min = 0;       // es el tamaño minimo por proceso
int proc_max = 0;       // es el tamaño maximo por proceso, ambos dependen de la memoria fisica.

vector<double> tProc;

vector <int> dirFis;    // direccion fisica del proceso, dir del frame.
vector <int> dirVir;    // dirección virtual del proceso, dir de la pagina.

double round2d(double num){
    return round(num*100)/100;
}

double MBround(double num){
    double resultado = round((num/pow(2,20))*100)/100;
    return resultado;
}

void imprimirMemorias(vector<int> fis, vector<int> vir){
    int i_fis = 0;
    int i_vir = 0;
    for(size_t i = 0 ; i < fis.size() + vir.size() ; i++){
        if (i_fis < fis.size()){
            i_fis++;
            cout << "| ";
            if(i<10){
                cout << i_fis << "  ";
            }
            else{
                cout << i_fis << " ";
            }
            cout << " |";
        }
        if (i_vir < vir.size()){
            i_vir++;
            cout << "  | ";
            if(i<10){
                cout << i_vir << "  ";
            }
            else{
                cout << i_vir << " ";
            }
            cout << " |";
        }
        cout << endl;
        
    }
}

bool memoriaLlena(){
    int resultado = 0;
    for (size_t i = 0; i < tProc.size() ; i++){
        resultado += tProc[i];
    }
    if (resultado > tVirtual){
        return true;
    }
    else {
        return false;
    }

}

void loop_procesos() {
    while (!memoriaLlena()) {
        cout << "Creando proceso " << endl;

    }
}

int main(){
    //cosas para generacion de numero random
    random_device rd;
    mt19937 gen(rd());

    uniform_real_distribution<> dist(1.5, 4.0);     // campana de gauss
    
    double randomN = round(dist(gen) * 100) / 100;  //redondeo manteniendo 2 decimales

    
    double input = 0;
    int mCont = 0;
    cout << "===MENÚ PRINCIPAL===" << endl;
    while (true){
        switch (mCont){
            case 0: {
                cout << "\nInserte tamaño de la memoria fisica en MB (numero natural mayor a 0): "; 
                cin >> input;
                tFisica = input * pow(2,20); // 2^20 = MB, para representarla en Bytes.
                if(tFisica > 0){
                    tVirtual = tFisica * randomN;
                    cout << "Su memoria virtual es: " << MBround(tVirtual) << "MB, " << randomN << " veces la memoria fisica." << endl;
                    mCont++;
                    continue;
                }
                else {
                    cout << "yapue." << endl;
                    continue;
                }
                break;
            }
            
            case 1: {
                cout << "\nInserte tamaño en MB para las paginas (menor a " << MBround(tFisica) << "MB, porfavor)\n(0 Para volver atrás): ";
                cin >> input;
                tPagina = input * pow(2,20);
                if(tPagina > 0){
                    prox_min = tPagina * 0.2;
                    proc_max = tPagina * 3; //el tamaño maximo de los procesos es 3 paginas.
                    cout << "Su tamaño de pagina es: " << MBround(tPagina) << "MB. \nPor lo tanto, se harán procesos de hasta " << MBround(proc_max) << "MB y de minimo " << MBround(prox_min) << "MB." << endl;
                    mCont++;
                    continue;
                }
                else if (input == 0){
                    cout << "\nVolvió atrás...\n";
                    mCont--
                    continue;
                }
                else {
                    cout << "yapue." << endl;
                    continue;
                }
                break;
            }
            case 2: {
                cout << "\nPresione ENTER para iniciar la simulacion o 0 para volver atrás.";
                cin >> input;
                if (input != 0){
                    mCont++;
                    continue;
                }
                else if (input == 0){
                    cout << "\nVolvió atrás...\n";
                    mCont--
                    continue;
                }
                break;
            }
        }

        thread generador(loop_procesos);

        while(!memoriaLlena(tProc, tVirtual)){
            
        }
    }

    
}
