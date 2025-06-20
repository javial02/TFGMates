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
#include "gurobi_c++.h"
#include "datosNBA.h"


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

void temple_simulado(vector<vector<int>>& viajes, double &distancia, double t_inicial, double t_minima, int M, double alpha) {
    double distancia_mejor = distancia;
    vector<vector<int>> mejor_calendario;

    mejor_calendario = copiar_calendario(viajes);

    double distancia_nueva = 0;
    double distancia_inicial;
    double distancia_final;
    while (t_inicial > t_minima) {
        if (t_inicial < 60 && t_inicial > 25) {
            M = M/2;
        }
        else if (t_inicial < 25) {
            M = 1;
        }
        for (int i = 0; i < M; i++) {
            int k1 = rand() % TOTAL_JORNADAS;
            int k2 = rand() % TOTAL_JORNADAS;
            while (k1 >= k2) {
                k1 = rand() % TOTAL_JORNADAS;
                k2 = rand() % TOTAL_JORNADAS;
            }

            distancia_inicial = calculaDistancias_h2(k1, k1 - 1, k1 + 1, viajes) + calculaDistancias_h2(k2, k2 - 1, k2 + 1, viajes);

            if (k1 < k2) {
                if (k2 - k1 == 1) {
                    
                    distancia_final = calculaDistancias_h2(k2, k1 - 1, k1, viajes) + calculaDistancias_h2(k1, k2, k2 + 1, viajes);
                }
                else {
                
                    distancia_final = calculaDistancias_h2(k2, k1 - 1, k1 + 1, viajes) + calculaDistancias_h2(k1, k2 - 1, k2 + 1, viajes);
                }
            }

            cambiaJornadas_h2(k1, k2, viajes);
            distancia_nueva = distancia_final - distancia_inicial;
            distancia += distancia_nueva;
            double dist = distancia;

        }

        int j1 = 0;
        double diferencia = 0;
        double distancia_inicial = 0;
        double distancia_final = 0;
        while (j1 < TOTAL_JORNADAS) {
            int j2 = j1 + 1;
            while (j2 < TOTAL_JORNADAS) {
                distancia_inicial = calculaDistancias_h2(j1, j1 - 1, j1 + 1, viajes) + calculaDistancias_h2(j2, j2 - 1, j2 + 1, viajes);
                if (j2 - j1 == 1) {
                    distancia_final = calculaDistancias_h2(j2, j1 - 1, j1, viajes) + calculaDistancias_h2(j1, j2, j2 + 1, viajes);
                }
                else {
                    distancia_final = calculaDistancias_h2(j2, j1 - 1, j1 + 1, viajes) + calculaDistancias_h2(j1, j2 - 1, j2 + 1, viajes);
                }


                if (distancia_final < distancia_inicial) {
                    diferencia = distancia_inicial - distancia_final;

                    distancia -= diferencia;
                    cambiaJornadas_h2(j1, j2, viajes);
                    //cout << "He cambiado la jornada " << j1 + 1 << " por la jornada " << j2 + 1 << " recortando " << diferencia << " millas" << endl;
                    j1 = -1;
                    break;
                }
                j2++;
            }
            j1++;
        }


        //cout << "Distancia tras cambios de jornada: " << distancia << endl;

        if (distancia < distancia_mejor) {
            distancia_mejor = distancia;
            //mejor_calendario = copiar_calendario(viajes);
        }


        t_inicial *= alpha;
        //cout << t_inicial << endl;

    }

    distancia = distancia_mejor;
    //viajes = copiar_calendario(mejor_calendario);
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

    vector<vector<int>> calendario_inicial = copiar_calendario(viajes);
    double dist_inicial = distancia;

    vector<vector<int>> mejor_calendario;
    double dist_mejor = distancia;

    vector<double> t = { 50, 100};
    //double t_inicial = 100;
    double t_minimo = 0.01;
    vector<int> Ms = { 2, 3, 4, 5, 6, 8, 10 };
    vector<double> alphas = { 0.90, 0.95, 0.99 };

    ofstream archivo2("Temple_simulado_h2.txt"); // Abre el archivo en modo lectura

    if (!archivo2) { // Verifica si el archivo se abrió correctamente
        cerr << "Error al abrir el archivo" << std::endl;
        return 1;
    }


    for (double t_inicial : t) {
        for (int M : Ms) {
            for (double alpha : alphas) {
                temple_simulado(viajes, distancia, t_inicial, t_minimo, M, alpha);
                archivo2 << "T = " << t_inicial << ", M = " << M << ", alpha = " << alpha << " -> Mejor Distancia: " << distancia << endl;
                cout << "T = " << t_inicial << ", M = " << M << ", alpha = " << alpha << " -> Mejor Distancia: " << distancia << endl;
                viajes = copiar_calendario(calendario_inicial);
                distancia = dist_inicial;
            }
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

