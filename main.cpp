#include <iostream>
#include <vector>
#include <random>
#include <cmath>

using namespace std;

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

bool memoriaLlena(vector<int> tProc, int tVirtual){
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

int main(){
    float input = 0;
    float tFisica = 0; // se inserta en MB pero se ve en B, 
    float tVirtual = 0; // de 1.5 a 4 veces la fisica (aleatorio)
    float tPagina = 0; // same as "fisica".
    vector <int> tProc; // tamaño de cada proceso.
    int rango_proc = 0; // es el tamaño maximo por proceso, depende de la memoria virtual.
    vector <int> dirFis; // direccion fisica del proceso, dir del frame.
    vector <int> dirVir; // dirección virtual del proceso, dir de la pagina.

    //cosas para generacion de numero random
    random_device rd;
    mt19937 gen(rd());

    uniform_real_distribution<> dist(1.5, 4.0);

    double randomN = dist(gen);

    int mCont = 0;
    cout << "===MENÚ PRINCIPAL===" << endl;
    while (true){
        switch (mCont){
            case 0: {
                cout << "Inserte tamaño de la memoria fisica en MB (numero natural mayor a 0): "; 
                cin >> input;
                tFisica = input * pow(2,20); // 2^20 = MB, para representarla en Bytes.
                if(tFisica > 0){
                    tVirtual = tFisica * randomN;
                    cout << "Su memoria virtual es: " << tVirtual << "B, " << randomN << " veces la memoria fisica." << endl;
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
                cout << "Inserte tamaño en MB para las paginas (menor a " << tFisica << "B, se recomienda que sean menores a 1): ";
                cin >> input;
                tPagina = input * pow(2,20);
                if(tPagina > 0){
                    rango_proc = tPagina * 3; //el tamaño maximo de los procesos es 3 paginas.
                    cout << "Su tamaño de pagina es: " << tPagina << "MB, por lo tanto, se harán procesos de hasta " << rango_proc << "B." << endl;
                    mCont++;
                    continue;
                }
                else {
                    cout << "yapue." << endl;
                    continue;
                }
                break;
            }
        }
        while(!memoriaLlena(tProc, tVirtual)){

        }
    }

    
}
