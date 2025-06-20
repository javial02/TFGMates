#include "datosNBA.h"
#include "heuristica3.h"

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

int buscaPartido(vector<vector<int>> viajes, int e1, int e2) {
    int jornada = -1;
    for (int k = 0; k < TOTAL_JORNADAS; k++) {              //Awui se podria poner aleatoriadad al valor inicial de k para darle mas aleatoriedad al partido
        if (viajes[e1][k] == e1 && viajes[e2][k] == e1) {
            jornada = k;
            break;
        }
    }

    return jornada;
}

void temple_simulado(vector<vector<int>>& viajes, double& distancia, double t_inicial, double t_minima, int M, double alpha) {

    double distancia_mejor = distancia;
    vector<vector<int>> mejor_calendario;

    mejor_calendario = copiar_calendario(viajes);

    double distancia_nueva = 0;
    double distancia_inicial;
    double distancia_final;
    int count = 0;
    while (t_inicial > t_minima) {
        if (t_inicial < 60 && t_inicial > 25) {
            M = 2;
        }
        else if (t_inicial < 25) {
            M = 1;
        }
        for (int iter = 0; iter < M; iter++) {

            int e1 = rand() % N;
            int e2 = rand() % N;
            while (e1 == e2) {
                e2 = rand() % N;
            }

            int jornada = buscaPartido(viajes, e1, e2);

            int j_partido = -1;
            double dist_nueva = 0;
            double dist_actual = 0;
            double max = 0;
            for (int k = 0; k < TOTAL_JORNADAS; k++) {
                if (viajes[e2][k] == e2 && viajes[e1][k] == e2) {
                    if (k - jornada == 1) {
                        dist_actual = calculaDistancias_h3(jornada, jornada - 1, jornada + 1, viajes, e1, e2) + calculaDistancias_h3(k, k - 1, k + 1, viajes, e2, e1);
                        dist_nueva = calculaDistancias_h3(k, k, k + 1, viajes, e1, e2) + calculaDistancias_h3(jornada, jornada - 1, jornada, viajes, e2, e1);   //aqui el local pasa a ser visitante y viceversa
                    }
                    else if (k - jornada == -1) {
                        dist_actual = calculaDistancias_h3(jornada, jornada - 1, jornada + 1, viajes, e1, e2) + calculaDistancias_h3(k, k - 1, k + 1, viajes, e2, e1);
                        dist_nueva = calculaDistancias_h3(k, k - 1, k, viajes, e1, e2) + calculaDistancias_h3(jornada, jornada, jornada + 1, viajes, e2, e1);   //aqui el local pasa a ser visitante y viceversa
                    }
                    else {
                        dist_actual = calculaDistancias_h3(jornada, jornada - 1, jornada + 1, viajes, e1, e2) + calculaDistancias_h3(k, k - 1, k + 1, viajes, e2, e1);
                        dist_nueva = calculaDistancias_h3(k, k - 1, k + 1, viajes, e1, e2) + calculaDistancias_h3(jornada, jornada - 1, jornada + 1, viajes, e2, e1);   //aqui el local pasa a ser visitante y viceversa
                    }

                    max = dist_actual - dist_nueva;
                    j_partido = k;
                }
            }

            cambiaJornadas_h3(jornada, j_partido, viajes, e1, e2);
            distancia -= max;

        }

        double diferencia = 0;
        int i = 0;

        while (i < N) {
            int j = 0;
            while (j < N) {
                if (i != j) {
                    for (int k = 0; k < TOTAL_JORNADAS - 1; k++) {
                        if (viajes[i][k] == i && viajes[j][k] == i) {
                            int cambio;
                            diferencia = buscaPartido(viajes, i, j, k, cambio);
                            if (cambio != -1) {
                                distancia -= diferencia;
                                cambiaJornadas_h3(k, cambio, viajes, i, j);
                                //cout << "He cambiado los partidos del equipo " << i << " y " << j << " en las jornadas " << k + 1 << " y " << cambio + 1 << " reduciendo " << diferencia << " millas" << endl;
                                j = -1;
                                break;
                            }
                        }
                    }
                }
                j++;
            }
            i++;
        }

        //cout << "Distancia tras cambios de jornada: " << distancia << endl;

        if (distancia < distancia_mejor) {
            distancia_mejor = distancia;
            mejor_calendario = copiar_calendario(viajes);
        }


        t_inicial *= alpha;

    }

    distancia = distancia_mejor;
    viajes = copiar_calendario(mejor_calendario);
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

    vector<double> t = { 50, 100, 200};
    //double t_inicial = 100;
    double t_minimo = 0.01;
    vector<int> Ms = { 2, 3, 4, 5, 6, 8, 10, 15, 20, 25};
    vector<double> alphas = { 0.90, 0.95, 0.99 };

    ofstream archivo2("resultados_ts_t4.txt"); // Abre el archivo en modo lectura

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

