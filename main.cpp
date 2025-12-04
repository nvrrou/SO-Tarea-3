#include <iostream>
#include <vector>
#include <thread>       // para hacer que el loop de procesos, el de eliminarlos y el de consultar direcciones sean hilos separados.
#include <chrono>       // para usar sleep y simular tiempos.
#include <mutex>        // para que la impresión se vea bien. 
#include <cstdlib>      // para randomizar cosas
#include <ctime>        // tmbn para randomizar cosas
#include <queue>        // fifo
#include <fstream>      // para escribir en el archivo de consultas.
#include <cmath>        // para funciones matemáticas como pow y round.
#include <algorithm>    // para la función min y max
#include <string>       // para manejar strings
#include <random>       // para mejor generación de números aleatorios

using namespace std;

bool terminar = false;  // para terminar en caso de que un proceso no pueda ser asignado.
vector<string> log_consultas;

bool consulta_fuera_rango = false; //para que la primera consulta sea fuera de rango y probar el reemplazo.

int fifo = -1;           // índice para el reemplazo FIFO.

double tamañoFisica=0;  // tamaño de la memoria física.
double tamañoVirtual=0; // tamaño de la memoria virtual.

double tamañoPagina=0;  // tamaño de cada página/marco.

int proc_min=0;         // tamaño mínimo de los procesos.
int proc_max=0;         // tamaño máximo de los procesos.

class pagina;           // declaracion adelantada.

vector<pagina> marcos;  // vector global de marcos.
vector<pagina> paginas; // vector global de paginas.

//función para convertir de bytes a MB y redondear a 2 decimales.
double MBround(double num){
    double resultado = round((num/pow(2,20))*100)/100;
    return resultado;
}

//esta clase representa tanto las paginas como los marcos.
class pagina{
    private:
        bool libre;
        int procesoId; //id del proceso que la ocupa, -1 si está libre.
        int parte; //parte del proceso que ocupa la página/marco.
        int uso; //porcentaje de uso.
    public:
        pagina(): libre(true), procesoId(-1), uso(0), parte(-1) {
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
        int getParte() const {
            return parte;
        }

        void ocupar(int idProceso, double ocupacion, int parteProceso) {
            parte = parteProceso;
            libre = false;
            procesoId = idProceso;
            uso = round((ocupacion/tamañoPagina)*100); //calcula el porcentaje de uso, redondeado.
        }
        void liberar() {
            libre = true;
            procesoId = -1;
            uso = 0;
            parte = -1;
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
    int getId() const {
        return id;
    }
    double gettamaño() const {
        return tamaño;
    }
    vector<pagina> getamañoPaginas() const {
        return paginas;
    }

};
//función para crear un proceso e intentar asignarle páginas.
void crearProceso(double tamañoProceso){
    proceso p(tamañoProceso);
    double tamañoRestante = tamañoProceso;
    vector<pagina>& pg = paginas;
    int parte = 1;
    cout << "Cantidad de Paginas: " << paginas.size() << endl;
    cout << "Tamaño Proceso: " << MBround(tamañoProceso) << "MB." << endl;
    cout << "Tamaño Pagina: " << MBround(tamañoPagina) << "MB." << endl;
    for (size_t i = 0 ; i < paginas.size() ; i++) {
        if (pg[i].isLibre() && tamañoRestante > 0) {
            double ocupacion = min(tamañoPagina, tamañoRestante);
            pg[i].ocupar(p.getId(), ocupacion, parte);
            
            int c = 0;
            for (auto& m : marcos) {
                if (m.isLibre()) {
                    m.ocupar(p.getId(), ocupacion, parte);
                    c++;
                    fifo = c;
                    break;
                }
            }
            
            parte++;
            tamañoRestante -= ocupacion;
        }
        if (tamañoRestante <= 0) {
            break;
        }
    }
    if (tamañoRestante > 0){
        cout << "No hay suficiente memoria para el proceso " << p.getId() + 1 << ". Terminando simulación.\n";
        terminar = true;
    }
}

//función para eliminar un proceso y liberar sus páginas y marcos.
void eliminarProceso(int idProceso){
    for (auto& pg : paginas) {
        if (pg.getProcesoId() == idProceso) {
            pg.liberar();
        }
    }
    for (auto& m : marcos) {
        if (m.getProcesoId() == idProceso) {
            m.liberar();
        }
    }
    cout << "Proceso " << idProceso << "# eliminado.\n";
}

//función para reemplazar la marco más antigua (FIFO).
void reemplazarMarco(int idProceso, double ocupacion, int parteProceso) {
    for (auto& m : marcos) {
        if (m.getProcesoId() == idProceso && m.getParte() == parteProceso){
            return;
        }
    }
     for (auto& m : marcos) {
        if (m.isLibre()) {
            m.ocupar(idProceso, ocupacion, parteProceso);
            return;
        }
    }
    // Si no hay marcos libres, reemplaza el mas nuevo (FIFO).
    marcos[fifo].liberar();
    marcos[fifo].ocupar(idProceso, ocupacion, parteProceso);

    cout << "Reemplazando marco " << fifo + 1 << " con proceso " << idProceso + 1 << ", parte " << parteProceso << ".\n";
    string msg =
                "Reemplazando marco " + to_string(fifo + 1) +
                " con proceso " + to_string(idProceso + 1) +
                ", parte " + to_string(parteProceso) +
                ".\n";

    // escribe en el archivo de consultas.
        {
            ofstream archivo("consultas.log", ios::app); // crea si no existe
            if (archivo.is_open()) {
                archivo << msg << "\n";
            }
        }


}

//función para generar un número aleatorio entre min y max (double, numero grande en resumen, xq está en Bytes).
double randomDouble(double min, double max) {
    static random_device rd;
    static mt19937 gen(rd());
    uniform_real_distribution<double> dist(min, max);
    return dist(gen);
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

    cout << "|            PAGINAS            |     |             MARCOS            |\n";

    cout << "+----------+----------+---------+"
         << "     "
         << "+----------+----------+---------+\n";

    // Tamaño máximo para recorrer filas
    size_t rows = max(paginas.size(), marcos.size());

    for (size_t i = 0; i < rows; i++) {
        
        // ---- Bloque derecho (PAGINAS) ----
        if (i < paginas.size()) {
            auto &p = paginas[i];
            cout << "| " << i+1;

            if(i+1 < 9){
                cout << " ";
            }

            if(p.getUso() > 99){
                cout << "\t   | " << p.getUso() << "%     | ";
            }
            else if(p.getUso() > 9) {
                cout << "\t   | " << p.getUso() << "%      | ";
            }
            else{
                cout << "\t   | " << p.getUso() << "%       | ";
            }

            string espacioFinal = (p.getParte() == -1) ? " " : "";
            string color = (p.getProcesoId() % 2 == 0) ? "34" : "36"; // azul para pares, cian para impares
            if(p.getProcesoId() == -1){
                cout << "\033[31m" << p.getProcesoId()  << "\033[0m, " << p.getParte() << espacioFinal + " |";
            }
            else if(p.getProcesoId() + 1 > 9){
                cout << "\033[" << color << "m" << p.getProcesoId() + 1 << "\033[0m, " << p.getParte() << espacioFinal + "   |";
            }
            else {
                cout << "\033[" << color << "m" << p.getProcesoId() + 1 << "\033[0m, " << p.getParte() << espacioFinal + "    |";
            }
        } else {
            // fila vacía para mantener alineación
            cout << "|          |          |         |";
        }

        cout << "     "; // espacio

        // Bloque derecho (MARCOS)
        if (i < marcos.size()) {
            auto &m = marcos[i];
            cout << "| " << i+1;
            if(i+1 < 9){
                cout << " ";
            }
            
            if(m.getUso() > 99){
                cout << "\t | " << m.getUso() << "%     | ";
            }
            else if(m.getUso() > 9) {
                cout << "\t | " << m.getUso() << "%      | ";
            }
            else{
                cout << "\t | " << m.getUso() << "%       | ";
            }

            string espacioFinal = (m.getParte() == -1) ? "" : " ";
            string color = (m.getProcesoId() % 2 == 0) ? "34" : "36"; // azul para pares, cian para impares
            if(m.getProcesoId() == -1){
                cout << "\033[31m" << m.getProcesoId()  << "\033[0m, " << m.getParte() << espacioFinal + "  |";
            }
            else if(m.getProcesoId() + 1 > 9){
                cout << "\033[" << color << "m" << m.getProcesoId() + 1 << "\033[0m, " << m.getParte() << espacioFinal + "  |";
            }
            else {
                cout << "\033[" << color << "m" << m.getProcesoId() + 1 << "\033[0m, " << m.getParte() << espacioFinal + "   |";
            }
        } else {
            cout << "| -        | -        | -       |";
        }

        cout << "\n";
    }
    // linea final
    cout << "+----------+----------+---------+"
         << "     "
         << "+----------+----------+---------+\n";
}

//función para renderizar el estado actual de las páginas y marcos y mandarla a un archivo (para el log de consultas).
void renderMSG() {
    ofstream archivo("consultas.log", ios::app); // crea si no existe, y agrega al final
    if (!archivo.is_open()) return;

    archivo << "+----------+----------+---------+"
            << "     "
            << "+----------+----------+---------+\n";

    archivo << "|            PAGINAS            |     |             MARCOS            |\n";

    archivo << "+----------+----------+---------+"
            << "     "
            << "+----------+----------+---------+\n";

    size_t rows = max(paginas.size(), marcos.size());

    for (size_t i = 0; i < rows; i++) {

        // ---- PAGINAS ----
        if (i < paginas.size()) {
            auto &p = paginas[i];
            archivo << "| " << i+1 << " ";

            if (i+1 < 10) archivo << " ";

            if (p.getUso() > 99)
                archivo << "\t   | " << p.getUso() << "%     | ";
            else if (p.getUso() > 9)
                archivo << "\t   | " << p.getUso() << "%      | ";
            else
                archivo << "\t   | " << p.getUso() << "%       | ";

            string espacioFinal = (p.getParte() == -1) ? "" : " ";
            if (p.getProcesoId() == -1) {
                archivo << p.getProcesoId() << ", " << p.getParte() << espacioFinal << "  |";
            } 
            else if (p.getProcesoId()+1 > 9) {
                archivo << p.getProcesoId() + 1 << ", " << p.getParte() << espacioFinal << "  |";
            } else {
                archivo << p.getProcesoId() + 1 << ", " << p.getParte() << espacioFinal << "   |";
            }

        } else {
            archivo << "|          |          |         |";
        }

        archivo << "     ";

        // ---- MARCOS ----
        if (i < marcos.size()) {
            auto &m = marcos[i];
            archivo << "| " << i+1;
            if (i+1 < 10) archivo << " ";

            if (m.getUso() > 99)
                archivo << "       | " << m.getUso() << "%     | ";
            else if (m.getUso() > 9)
                archivo << "       | " << m.getUso() << "%      | ";
            else
                archivo << "       | " << m.getUso() << "%       | ";

            string espacioFinal = (m.getParte() == -1) ? "" : " ";

            if (m.getProcesoId() == -1) {
                archivo << m.getProcesoId() << ", " << m.getParte() << espacioFinal << "  |";
            } 
            else if (m.getProcesoId() + 1 > 9) {
                archivo << m.getProcesoId() + 1  << ", " << m.getParte() << espacioFinal << "  |";
            } else {
                archivo << m.getProcesoId() + 1 << ", " << m.getParte() << espacioFinal + "   |";
            }

        } else {
            archivo << "| -        | -        | -       |";
        }

        archivo << "\n";
    }

    archivo << "+----------+----------+---------+"
            << "     "
            << "+----------+----------+---------+\n";

    archivo << "\n\n"; // separación entre renders
}

//loop para crear procesos cada cierto tiempo.
void* loop_procesos() {
    while (!memoria_llena() && !terminar) {
        float tamaño = randomDouble(proc_min, proc_max);
        crearProceso(tamaño);
        this_thread::sleep_for(chrono::milliseconds(2000)); // Actualiza cada 2 s
    }
    return nullptr;
}

//loop para renderizar el estado cada cierto tiempo.
void* loop_render() {
    while (!memoria_llena() && !terminar) {
        render();
        this_thread::sleep_for(chrono::milliseconds(500)); // Actualiza cada 500 ms
    }
    render(); // render final al terminar
    return nullptr;
}

//loop para eliminar procesos cada cierto tiempo.
void* loop_eliminador() {
    this_thread::sleep_for(chrono::milliseconds(30000)); // Espera 30 segundos antes de eliminar
    while (!memoria_llena() && !terminar) {
        if (!paginas.empty()) {
            int idProceso;
            if (!consulta_fuera_rango){
                int consulta = tamañoFisica + (tamañoVirtual - tamañoFisica) / 3; // dirección virtual fuera de rango, pero distinta de la q se va a consultar para forzar reemplazo.
                int pagina = consulta / tamañoPagina;
                idProceso = paginas[pagina].getProcesoId(); 

            } 
            //luego si es aleatorio.
            else{
                idProceso = paginas[rand() % paginas.size()].getProcesoId(); // Selecciona un proceso aleatorio.
            }
             
            if (idProceso != -1) { // Asegura que el proceso exista.
                eliminarProceso(idProceso);
            }
        }
        this_thread::sleep_for(chrono::milliseconds(5000)); // Espera 5 segundos para cada eliminación.
    }   
    return nullptr;
}

//loop para consultar direcciones cada cierto tiempo.
void* loop_consultador(){
    this_thread::sleep_for(chrono::milliseconds(30000)); // Espera 30s antes de la primera consulta
    fifo = (int)marcos.size() - 1; // inicializa el índice FIFO al último marco.

    while (!memoria_llena() && !terminar) {
        int pagina;
        double consulta;

        // Primera consulta forzada fuera de rango
        if (!consulta_fuera_rango){
            consulta_fuera_rango = true;
            consulta = tamañoFisica + (tamañoVirtual - tamañoFisica) / 2;
            pagina = consulta / tamañoPagina;
        }
        else{
            consulta = randomDouble(0, tamañoVirtual - (tamañoVirtual - tamañoPagina * paginas.size()));
            pagina = consulta / tamañoPagina;
        }

        // Si la página consultada NO tiene proceso (-1), solo registrar y continuar
        if (paginas[pagina].getProcesoId() == -1) {

            string msg =
                "- Consulta de dirección virtual: " +
                to_string(consulta) +
                "B (Página " +
                to_string(pagina + 1) +
                ") — Página sin proceso asignado.\n\n";

            // Registrar en el log
            {
                ofstream archivo("consultas.log", ios::app);
                if (archivo.is_open()) {
                    archivo << msg << "\n";
                }
            }

            // Mostrar en consola
            cout << msg << "\n";

            // Mantener ritmo entre consultas
            this_thread::sleep_for(chrono::milliseconds(5000));
            continue; // saltar a la siguiente iteración del ciclo
        }

        // Aquí continúa normalmente si sí tiene proceso asignado
        string msg = 
            "- Consulta de dirección virtual: " + 
            to_string(consulta) + 
            "B (Página " + 
            to_string(pagina + 1) + ") con proceso " + 
            to_string(paginas[pagina].getProcesoId() + 1) +
            ", parte " + to_string(paginas[pagina].getParte()) +
            ".\n";

        msg += "\nEstado de la memoria antes de la consulta:\n";

        {
            ofstream archivo("consultas.log", ios::app);
            if (archivo.is_open()) archivo << msg << "\n";
        }

        renderMSG();

        reemplazarMarco(
            paginas[pagina].getProcesoId(),
            (paginas[pagina].getUso() / 100.0) * tamañoPagina,
            paginas[pagina].getParte()
        );

        string msg1 = "\nEstado de la memoria después de la consulta:\n";

        {
            ofstream archivo("consultas.log", ios::app);
            if (archivo.is_open()) archivo << msg1 << "\n";
        }

        renderMSG();

        cout << msg << "\n";

        this_thread::sleep_for(chrono::milliseconds(5000));
    }

    return nullptr;
}

//menú principal para configurar la simulación.
int main() {
    ofstream limpiar("consultas.log", ios::trunc);
    limpiar.close();
    srand(static_cast<unsigned int>(time(0))); // inicializa la semilla para randomizar.

    double input = 0;
    int mCont = 0;
    bool menuActivo = true;
    cout << "===MENÚ PRINCIPAL===" << endl;
    while (menuActivo){
        switch (mCont){
            case 0: {
                cout << "\nInserte tamaño de la memoria fisica en MB (numero natural mayor a 0): "; 
                cin >> input;
                tamañoFisica = input * pow(2,20); // 2^20 = MB, para representarla en Bytes.
                if(tamañoFisica > 0){
                    double randomN = round( randomDouble(15, 40) * 100 ) / 1000; //número aleatorio entre 1.5 y 4.0, redondeado a 3 decimales.
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
                cout << "\nInserte tamaño en KB para las paginas \n(se recomienda entre "<< MBround(tamañoVirtual/36)*1000 << "KB y " << MBround(tamañoVirtual/30)*1000 << "KB, para que alcancen a pasar 30 segundos)\n(0 Para volver atrás): ";
                cin >> input;
                tamañoPagina = input * pow(2,10);
                if(tamañoPagina > 0){
                    proc_min = tamañoPagina * 0.05; //el tamaño minimo de los procesos es 5% de una pagina.
                    proc_max = tamañoPagina * 2.28 - 1; //el tamaño maximo de los procesos es 3 paginas, pero es poco probable.
                    cout << "Su tamaño de pagina es: " << MBround(tamañoPagina) << "MB. \nPor lo tanto, se harán procesos de " << MBround(proc_min) << "MB, hasta " << MBround(proc_max) << "MB." << endl;
                    setPaginas();
                    render();
                    cout<<  "Inicializando páginas y marcos..." << endl;
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
                cout << "\nCualquier numero + ENTER para iniciar la simulacion o 0 para volver atrás.";
                cin >> input;
                if (input != 0){
                    mCont++;
                    menuActivo = false;
                    break;
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
    cout << "\nIniciando simulación...\n";

    thread procesoThread(loop_procesos);
    thread renderThread(loop_render);
    thread eliminadorThread(loop_eliminador);
    thread consultadorThread(loop_consultador);

    procesoThread.join();
    renderThread.join();
    eliminadorThread.join();
    consultadorThread.join();
}
