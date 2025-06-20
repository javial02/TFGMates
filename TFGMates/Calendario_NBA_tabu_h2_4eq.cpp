//EN ESTE MODELO NO SE ESTÁ CONTANDO CON LA DISTANCIA RECORRIDA DE LOS EQUIPOS VISITANTES EN LA PRIMERA JORNADA DE LA TEMPORADA.
//SE DA POR HECHO DE QUE SE EMPIEZA A CONTAR A PARTIR DE LA JORNADA 1

#include <vector>
#include <string>
#include <iostream>
#include <fstream>
#include <cstdlib>
#include <ctime>
#include <cmath>
#include <algorithm>
#include <set>
#include <tuple>
#include "gurobi_c++.h"


using namespace std;
const int N = 4; // Número de equipos
const int TOTAL_JORNADAS = 12; // jornada 0 y jornada 13 ficticias
const int EQUIPOS_POR_DIVISION = 4;

struct InfoEquipo {
    int id;
    string nombre;
    string conferencia;
    string division;
    vector<int> rivales_division;
    vector<int> rivales_conf1;
    vector<int> rivales_conf2;
    vector<int> rivales_conf3;
    vector<int> rivales_interconf;
};

vector<InfoEquipo> equipos = {
    {0, "Boston Celtics", "Este", "Atlántico", {1, 2, 3}, {8, 7, 5, 10, 12}, {9, 13}, {6, 11}, {}},
    {1, "Brooklyn Nets", "Este", "Atlántico", {0, 2, 3}, {13, 6, 9, 10, 5, 11}, {8}, {7, 12}, {}},
    {2, "Philadelphia 76ers", "Este", "Atlántico", {0, 1, 3}, {6, 9, 5, 13, 12, 11}, {7, 10}, {8}, {}},
    {3, "New York Knicks", "Este", "Atlántico", {0, 1, 2}, {7, 8, 11, 13, 6}, {5, 12}, {10, 9}, {}}
};

vector<vector<double>> distanciasNBA = {
    {0, 217.9, 309.3, 213.78},
    {217.9, 0, 98.12, 5.45},
    {309.3, 98.12, 0, 96.67},
    {213.78, 5.45, 96.67, 0},
};



double calculaDistancias_h2(int k, int kantes, int kdespues, const vector<vector<int>>& viajes) {
    double total_dist = 0;
    for (int i = 0; i < N; i++) {
        if (kantes >= 0 && kdespues <= TOTAL_JORNADAS - 1) {
            total_dist += distanciasNBA[viajes[i][kantes]][viajes[i][k]] + distanciasNBA[viajes[i][k]][viajes[i][kdespues]];
        }
        else if (kantes < 0) {
            total_dist += distanciasNBA[viajes[i][k]][viajes[i][kdespues]] + distanciasNBA[viajes[i][k]][i];
        }
        else if (kdespues > TOTAL_JORNADAS - 1) {
            total_dist += distanciasNBA[viajes[i][kantes]][viajes[i][k]] + distanciasNBA[viajes[i][k]][i];
        }
    }

    return total_dist;
}

void cambiaJornadas_h2(int k1, int k2, vector<vector<int>>& viajes) {
    for (int i = 0; i < N; i++) {
        int aux = viajes[i][k1];
        viajes[i][k1] = viajes[i][k2];
        viajes[i][k2] = aux;
    }
}

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
                    distancia_inicial = calculaDistancias_h2(k1, k1 - 1, k1 + 1, viajes) + calculaDistancias_h2(k2, k2 - 1, k2 + 1, viajes);
                    distancia_final = calculaDistancias_h2(k2, k1 - 1, k1, viajes) + calculaDistancias_h2(k1, k2, k2 + 1, viajes);
                }
                else {
                    distancia_inicial = calculaDistancias_h2(k1, k1 - 1, k1 + 1, viajes) + calculaDistancias_h2(k2, k2 - 1, k2 + 1, viajes);
                    distancia_final = calculaDistancias_h2(k2, k1 - 1, k1 + 1, viajes) + calculaDistancias_h2(k1, k2 - 1, k2 + 1, viajes);
                }

                //vector<vector<int>> vecino = copiar_calendario(viajes);
                cambiaJornadas_h2(k1, k2, viajes);
                double diferencia = distancia_inicial - distancia_final;

                //cout << diferencia << " " << mejor_dist_vec << endl;
                if (diferencia > mejor_dist_vec) {
                    mejor_dist_vec = diferencia;
                    mejor_vecino = copiar_calendario(viajes);
                    mejor_mov = { 1,k1,k2 };

                }
                cambiaJornadas_h2(k2, k1, viajes);

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

    ifstream archivo("Calendario_modelo_4_equipos_con_numeros.txt"); // Abre el archivo en modo lectura

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
        distancia += distanciasNBA[i][viajes[i][0]] + distanciasNBA[i][viajes[i][11]];
    }

    cout << "Distancia inicial dada por el modelo: " << distancia << endl;

    vector<vector<int>> calendario_inicial = copiar_calendario(viajes);
    double dist_inicial = distancia;

    vector<vector<int>> mejor_calendario;
    double dist_mejor = distancia;


    ofstream archivo2("Tabu_h2_4eq.txt"); // Abre el archivo en modo lectura

    if (!archivo2) { // Verifica si el archivo se abrió correctamente
        cerr << "Error al abrir el archivo" << std::endl;
        return 1;
    }

    vector<int> max_iter = { 100, 500, 1000 };
    vector<int> max_sin_mej = { 50, 100 };

    for(int iter : max_iter) {
        for (int mej : max_sin_mej) {
            busqueda_tabu(viajes, distancia, iter, mej);
            archivo2 << "Max_iter = " << iter << ", Lista = " << mej << " -> Mejor Distancia: " << distancia << endl;
            cout << "Max_iter = " << iter << ", Lista = " << mej << " -> Mejor Distancia: " << distancia << endl;
            viajes = copiar_calendario(calendario_inicial);
            distancia = dist_inicial;
        }
    }




    /*for (int i = 0; i < N; i++) {
        for (int k = 0; k < TOTAL_JORNADAS; k++) {
            archivo2 << viajes[i][k] << " ";
        }
        archivo2 << endl;
    }*/

    archivo2.close(); // Cierra el archivo


    return 0;
}

