//EN ESTE MODELO NO SE ESTÁ CONTANDO CON LA DISTANCIA RECORRIDA DE LOS EQUIPOS VISITANTES EN LA PRIMERA JORNADA DE LA TEMPORADA.
//SE DA POR HECHO DE QUE SE EMPIEZA A CONTAR A PARTIR DE LA JORNADA 1

#include "datosNBA.h"

double calculaDistancias(int k, int kantes, int kdespues, const vector<vector<int>>& viajes, int local, int visitante) {
    double total_dist = 0;
    if (kantes >= 0 && kdespues <= TOTAL_JORNADAS - 1) {
        total_dist += distanciasNBA[viajes[local][kantes]][local] + distanciasNBA[local][viajes[local][kdespues]];
        total_dist += distanciasNBA[viajes[visitante][kantes]][local] + distanciasNBA[local][viajes[visitante][kdespues]];
    }
    else if (kantes < 0) {
        total_dist += distanciasNBA[local][viajes[local][kdespues]];
        total_dist += distanciasNBA[local][viajes[visitante][kdespues]] + distanciasNBA[local][visitante];
    }
    else if (kdespues > TOTAL_JORNADAS - 1) {
        total_dist += distanciasNBA[viajes[local][kantes]][local];
        total_dist += distanciasNBA[viajes[visitante][kantes]][local] + distanciasNBA[local][visitante];
    }

    return total_dist;
}

void cambiaJornadas(int k1, int k2, vector<vector<int>>& viajes, int local, int visitante) {
    viajes[local][k1] = visitante;
    viajes[local][k2] = local;
    viajes[visitante][k1] = visitante;
    viajes[visitante][k2] = local;
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

double calculaDistancias2(int k, int kantes, int kdespues, const vector<vector<int>>& viajes) {
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

void cambiaJornadas2(int k1, int k2, vector<vector<int>>& viajes) {
    for (int i = 0; i < N; i++) {
        int aux = viajes[i][k1];
        viajes[i][k1] = viajes[i][k2];
        viajes[i][k2] = aux;
    }
}

double buscaPartido_tabu(const vector<vector<int>>& viajes, int local, int visitante, int jornada, int& j_partido) {
    j_partido = -1;
    double dist_nueva = 0;
    double dist_actual = 0;
    double max = -numeric_limits<double>::max();
    for (int k = 0; k < TOTAL_JORNADAS; k++) {
        if (viajes[visitante][k] == visitante && viajes[local][k] == visitante) {
            if (k - jornada == 1) {
                dist_actual = calculaDistancias(jornada, jornada - 1, jornada + 1, viajes, local, visitante) + calculaDistancias(k, k - 1, k + 1, viajes, visitante, local);
                dist_nueva = calculaDistancias(k, k, k + 1, viajes, local, visitante) + calculaDistancias(jornada, jornada - 1, jornada, viajes, visitante, local);   //aqui el local pasa a ser visitante y viceversa
            }
            else if (k - jornada == -1) {
                dist_actual = calculaDistancias(jornada, jornada - 1, jornada + 1, viajes, local, visitante) + calculaDistancias(k, k - 1, k + 1, viajes, visitante, local);
                dist_nueva = calculaDistancias(k, k - 1, k, viajes, local, visitante) + calculaDistancias(jornada, jornada, jornada + 1, viajes, visitante, local);   //aqui el local pasa a ser visitante y viceversa
            }
            else {
                dist_actual = calculaDistancias(jornada, jornada - 1, jornada + 1, viajes, local, visitante) + calculaDistancias(k, k - 1, k + 1, viajes, visitante, local);
                dist_nueva = calculaDistancias(k, k - 1, k + 1, viajes, local, visitante) + calculaDistancias(jornada, jornada - 1, jornada + 1, viajes, visitante, local);   //aqui el local pasa a ser visitante y viceversa
            }

            if (dist_actual - dist_nueva > max) {
                max = dist_actual - dist_nueva;
                j_partido = k;
            }

        }
    }

    return max;
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

        for (int i = 0; i < N; i++) {
            for (int j = 0; j < N && j != i; j++) {
                if (lista_tabu.count({ 2, i, j }) || lista_tabu.count({ 2, j, i })) {
                    continue;
                }
                for (int k = 0; k < TOTAL_JORNADAS - 1; k++) {
                    if (viajes[i][k] == i && viajes[j][k] == i) {
                        int cambio;
                        int diferencia = buscaPartido_tabu(viajes, i, j, k, cambio);
                        //cout << "dif " << i << " " << j << " " << k  << endl;
                        if (cambio != -1) {
                            cambiaJornadas(k, cambio, viajes, i, j);

                            if (diferencia > mejor_dist_vec) {
                                //cout << "cambio: " << cambio << endl;
                                mejor_dist_vec = diferencia;
                                mejor_vecino = copiar_calendario(viajes);
                                mejor_mov = { 2,i,j };

                            }
                            cambiaJornadas(k, cambio, viajes, j, i);
                            break;
                        }
                    }
                }
            }
        }

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



        //cout << "aqui" << endl;
        viajes = copiar_calendario(mejor_vecino);
        // cout << "hola2" << endl;
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

        //cout << "hola" << endl;

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

    busqueda_tabu(viajes, distancia, 1000, 50);

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


    for (int i = 0; i < N; i++) {
        cout << "Equipo: " << i << endl;
        int counttotal = 0;
        int countlocal = 0;
        for (int k = 0; k < TOTAL_JORNADAS; k++) {
            if (viajes[i][k] != -1) {
                counttotal++;
            }
            if (viajes[i][k] == i) {
                countlocal++;
            }
            cout << viajes[i][k] << " ";
        }

        cout << "Partidos totales: " << counttotal << "------ Partidos como local: " << countlocal << endl;
        cout << endl;
    }

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

