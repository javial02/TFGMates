#include "datosNBA.h"
#include "heuristica2.h"

int calculaRival(const vector<vector<int>> viajes, int jornada, int rival) {
    bool encontrado = false;
    int ind = -1;
    for (int i = 0; i < N && !encontrado; i++) {
        if (viajes[i][jornada] == rival && i != rival) {
            ind = i;
            encontrado = true;
        }
    }

    return ind;
}


void imprimeCalendario(const vector<vector<int>> viajes) {
    for (int k = 0; k < TOTAL_JORNADAS; k++) {
        cout << "Jornada " << k + 1 << ":" << endl;
        for (int i = 0; i < N; i++) {
            if (viajes[i][k] == i) {
                int rival = calculaRival(viajes, k, i);
                cout << equipos[i].nombre << " vs " << equipos[rival].nombre << endl;
            }
        }

        cout << "----------------------" << endl;
    }
}

vector<vector<int>> copiar_calendario(const vector<vector<int>> nuevo) {
    vector<vector<int>> copia;
    for (int i = 0; i < N; i++) {
        vector<int> recorrido;
        for (int k = 0; k < TOTAL_JORNADAS; k++) {
            int aux = nuevo[i][k];
            recorrido.push_back(aux);
        }
        copia.push_back(recorrido);
    }
    return copia;
}

void busqueda_tabu(vector<vector<int>>& viajes, double& distancia, int max_iter, int max_tabu_size) {
    double distancia_mejor = distancia;
    vector<vector<int>> mejor_calendario;
    mejor_calendario = copiar_calendario(viajes);

    set<tuple<int, int, int>> lista_tabu;

    for (int iter = 0; iter < max_iter; iter++) {
        double mejor_dist_vec = -numeric_limits<double>::max();
        tuple<int, int, int> mejor_mov;
        vector<vector<int>> mejor_vecino;

        for (int k1 = 0; k1 < TOTAL_JORNADAS; k1++) {
            for (int k2 = k1 + 1; k2 < TOTAL_JORNADAS; k2++) {
                if (lista_tabu.count({ 1,k1,k2 })) {
                    continue;
                }

                double distancia_inicial;
                double distancia_final;
                if (k2 - k1 == 1) {
                    distancia_inicial = calculaDistancias2(k1, k1 - 1, k1 + 1, viajes) + calculaDistancias2(k2, k2 - 1, k2 + 1, viajes);
                    distancia_final = calculaDistancias2(k2, k1 - 1, k1, viajes) + calculaDistancias2(k1, k2, k2 + 1, viajes);
                }
                else {
                    distancia_inicial = calculaDistancias2(k1, k1 - 1, k1 + 1, viajes) + calculaDistancias2(k2, k2 - 1, k2 + 1, viajes);
                    distancia_final = calculaDistancias2(k2, k1 - 1, k1 + 1, viajes) + calculaDistancias2(k1, k2 - 1, k2 + 1, viajes);
                }

                //vector<vector<int>> vecino = copiar_calendario(viajes);
                cambiaJornadas2(k1, k2, viajes);
                double diferencia = distancia_inicial - distancia_final;
                
                //cout << diferencia << " " << mejor_dist_vec << endl;
                if (diferencia > mejor_dist_vec) {
                    mejor_dist_vec = diferencia;
                    mejor_vecino = copiar_calendario(viajes);
                    mejor_mov = { 1,k1,k2 };

                }
                cambiaJornadas2(k2, k1, viajes);

                //cout << k1 << " " << k2 <<  endl;

            }
        }

        //cout << "hola" << endl;
        viajes = copiar_calendario(mejor_vecino);
        //cout << "hola2" << endl;
        distancia -= mejor_dist_vec;
        
        lista_tabu.insert(mejor_mov);
        if (lista_tabu.size() > max_tabu_size) {
            lista_tabu.erase(lista_tabu.begin());
        }

        
        if (distancia < distancia_mejor) {
            mejor_calendario = copiar_calendario(viajes);
            distancia_mejor = distancia;
            cout << "[It: " << iter << "] Mejor distancia: " << distancia_mejor << endl;
        }
    }

    viajes = copiar_calendario(mejor_calendario);
    distancia = distancia_mejor;
}




int main() {

    ifstream archivo("calendario.txt"); // Abre el archivo en modo lectura

    if (!archivo) { // Verifica si el archivo se abrió correctamente
        cerr << "Error al abrir el archivo" << std::endl;
        return 1;
    }

    vector<vector<int>> viajes;
    for (int i = 0; i < N; i++) {
        vector<int> recorrido;
        int n;
        for (int k = 0; k < TOTAL_JORNADAS; k++) {
            archivo >> n;
            recorrido.push_back(n);
        }
        viajes.push_back(recorrido);
    }

    archivo.close(); // Cierra el archivo

    //-----------------Distancia que genera el modelo--------------------------
    double distancia = 0;
    for (int i = 0; i < N; i++) {
        for (int k = 0; k < TOTAL_JORNADAS - 1; k++) {
            distancia += distanciasNBA[viajes[i][k]][viajes[i][k + 1]];
        }
    }

    for (int i = 0; i < N; i++) {
        distancia += distanciasNBA[i][viajes[i][0]] + distanciasNBA[i][viajes[i][81]];
    }

    cout << "Distancia inicial dada por el modelo: " << distancia << endl;

    busqueda_tabu(viajes, distancia, 500, 50);

    distancia = 0;
    for (int i = 0; i < N; i++) {
        for (int k = 0; k < TOTAL_JORNADAS - 1; k++) {
            distancia += distanciasNBA[viajes[i][k]][viajes[i][k + 1]];
        }
    }

    for (int i = 0; i < N; i++) {
        distancia += distanciasNBA[i][viajes[i][0]] + distanciasNBA[i][viajes[i][81]];
    }

    cout << "Distancia final: " << distancia << endl;


    /*ofstream archivo2("resultados_tabu_t2.txt"); // Abre el archivo en modo lectura

    if (!archivo2) { // Verifica si el archivo se abrió correctamente
        cerr << "Error al abrir el archivo" << std::endl;
        return 1;
    }

    

    /*for (int i = 0; i < N; i++) {
        for (int k = 0; k < TOTAL_JORNADAS; k++) {
            archivo2 << viajes[i][k] << " ";
        }
        archivo2 << endl;
    }

    archivo2.close();*/ // Cierra el archivo


    return 0;
}

