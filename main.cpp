#include <iostream>
#include <vector>
#include <thread>   // para hacer que el loop de procesos, el de eliminarlos y el de consultar direcciones sean hilos separados.
#include <chrono>   // para usar sleep y simular tiempos.
#include <mutex>    // para que la impresión se vea bien. 
#include <cstdlib>  // para randomizar cosas
#include <ctime>    // tmbn para randomizar cosas

using namespace std;


double tamañoFisica=0; //tamaño de la memoria física.
double tamañoVirtual=0; //tamaño de la memoria virtual.

double tamañoPagina=0; //tamaño de cada página/marco.

int proc_min=0; //tamaño mínimo de los procesos.
int proc_max=0; //tamaño máximo de los procesos.

class pagina; //declaracion adelantada.

vector<pagina> marcos; //vector global de marcos.
vector<pagina> paginas; //vector global de paginas.


int contPg=0; //para los ids de las paginas.

//esta clase representa tanto las paginas como los marcos.
class pagina{
    private:
        int id;
        bool libre;
        int procesoId; //id del proceso que la ocupa, -1 si está libre.
        int uso; //porcentaje de uso.
    public:
        pagina(): id(contPg), libre(true), procesoId(-1), uso(0) {
            contPg++; 
        } 

        int getId() const {
            return id;
        }
        int getProcesoId() const {
            return procesoId;
        }
        int getUso() const {
            return uso;
        }
        bool isLibre() const {
            return libre;
        }

        void ocupar(int idProceso, double ocupacion) {
            libre = false;
            procesoId = idProceso;
            uso = round((ocupacion/tamañoPagina)*100); //calcula el porcentaje de uso, redondeado.
        }
        void liberar() {
            libre = true;
            procesoId = -1;
            uso = 0;
        }
};

int contPs=0; //para los ids de los procesos.

class proceso{
    private:
        int id;
        double tamaño;
        vector<pagina> paginas;

    public:
    proceso(double t): tamaño(t) { 
        id = contPs; 
        contPs++;

    } 
    int gettamaño() const {
        return tamaño;
    }
    vector<pagina> getamañoPaginas() const {
        return paginas;
    }

};
//función para convertir de bytes a MB y redondear a 2 decimales.
double MBround(double num){
    double resultado = round((num/pow(2,20))*100)/100;
    return resultado;
}

//función para generar un número aleatorio entre min y max (double, numero grande en resumen, xq está en Bytes).
double randomDouble(double min, double max) {
    double r = (double)rand() / (double)RAND_MAX; // [0, 1]
    return min + r * (max - min);                 // escala al rango
}

//función para verificar si la memoria está llena.
bool memoria_llena(){
    for (auto& p : paginas) {
        if (p.isLibre()) {
            return false;
        }
    }
    return true;
}

//función para inicializar los vectores de páginas y marcos según los tamaños dados.
void setPaginas(){
    int num_marcos = tamañoFisica / tamañoPagina; // AMBAS REDONDEAN HACIA ABAJO, x lo tanto sobrará espacio, casi siempre.
    int num_paginas = tamañoVirtual / tamañoPagina;

    marcos.clear();
    paginas.clear();

    for (int i = 0; i < num_marcos; i++) {
        marcos.push_back(pagina());
    }
    for (int i = 0; i < num_paginas; i++) {
        paginas.push_back(pagina());
    }
}

//función para renderizar el estado actual de las páginas y marcos.
void render() {
    system("clear"); // Linux/macOS
    
    const int leftWidth = 35;   // ancho gráfico para columnas izquierda
    const int rightWidth = 35;  // ancho gráfico para columnas derecha

    cout << "+----------+----------+---------+"
         << "     "
         << "+----------+----------+---------+\n";

    cout << "|            PAGINAS            |     |            MARCOS             |\n";

    cout << "+----------+----------+---------+"
         << "     "
         << "+----------+----------+---------+\n";

    // Tamaño máximo para recorrer filas
    size_t rows = max(paginas.size(), marcos.size());

    for (size_t i = 0; i < rows; i++) {
        
        // ---- Bloque derecho (PAGINAS) ----
        if (i < paginas.size()) {
            auto &p = paginas[i];
            cout << "| " << i;
            cout << "\t   | " << p.getUso() << "%\t   | "
                 << p.getProcesoId() << "\t   |";
        } else {
            // fila vacía para mantener alineación
            cout << "|          |          |         |";
        }

        cout << "     "; // espacio

        // Bloque derecho (MARCOS)
        if (i < marcos.size()) {
            auto &m = marcos[i];
            cout << "| " << i;
            cout << "\t   | " << m.getUso() << "%\t   | "
                 << m.getProcesoId() << "\t   |";
        } else {
            cout << "|          |          |         |";
        }

        cout << "\n";
    }
    // linea final
    cout << "+----------+----------+---------+"
         << "     "
         << "+----------+----------+---------+\n";
}


void loop_procesos() {
    while (!memoria_llena()) {
        
    }
}


int main() {
    double input = 0;
    int mCont = 0;
    cout << "===MENÚ PRINCIPAL===" << endl;
    while (true){
        switch (mCont){
            case 0: {
                cout << "\nInserte tamaño de la memoria fisica en MB (numero natural mayor a 0): "; 
                cin >> input;
                tamañoFisica = input * pow(2,20); // 2^20 = MB, para representarla en Bytes.
                if(tamañoFisica > 0){
                    int randomN = round(randomDouble(1.5,4.0)*100)/100; //número aleatorio entre 1.5 y 4.0, redondeado a 2 decimales.
                    tamañoVirtual = tamañoFisica * randomN; //la memoria virtual será entre 1.5 y 4 veces la física.

                    cout << "Su memoria virtual es: " << MBround(tamañoVirtual) << "MB, " << randomN << " veces la memoria fisica." << endl;
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
                cout << "\nInserte tamaño en MB para las paginas (menor a " << MBround(tamañoFisica) << "MB, porfavor)\n(0 Para volver atrás): ";
                cin >> input;
                tamañoPagina = input * pow(2,20);
                if(tamañoPagina > 0){
                    proc_min = tamañoPagina * 0.2;
                    proc_max = tamañoPagina * 3; //el tamaño maximo de los procesos es 3 paginas.
                    cout << "Su tamaño de pagina es: " << MBround(tamañoPagina) << "MB. \nPor lo tanto, se harán procesos de " << MBround(proc_min) << "MB, hasta " << MBround(proc_max) << "MB." << endl;
                    mCont++;
                    continue;
                }
                else if (input == 0){
                    cout << "\nVolvió atrás...\n";
                    mCont--;
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
                    mCont--;
                    continue;
                }
                break;
            }
        }
    }
    

}