#include <iostream>
#include <vector>

using namespace std;

void imprimirMemorias(vector<int> fis, vector<int> vir){
    int i_fis = 0;
    int i_vir = 0;
    for(size_t i = 0 ; i < fis.size() + vir.size() ; i++){
        if (i_fis < fis.size()){
            if(i!=NULL){
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
        }
        if (i_vir < vir.size()){
            if(i!=NULL){
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
        }
        cout << endl;
        
    }
}

int main(){
    int fisica; //en MB, 
    int vrtual; // de 1.5 a 4 veces la fisica (aleatorio)
    int tam_pagina; //en MB
    vector <int> tProc;
    int rango_proc; // es el tamaño maximo por proceso, depende de la memoria virtual
    vector <int> dirFis;
    vector <int> dirVir;

}
