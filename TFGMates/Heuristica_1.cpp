#include <vector>
#include <string>
#include <iostream>
#include <limits>
#include "gurobi_c++.h"
#include "datosNBA.h"

// ----------------------Funciones Heur�stica 2---------------------------------
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


//--------------------Funciones Heur�stica 3----------------------------
double calculaDistancias_h3(int k, int kantes, int kdespues, const vector<vector<int>>& viajes, int local, int visitante) {
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

double buscaPartido_h3(const vector<vector<int>>& viajes, int local, int visitante, int jornada, int& j_partido) {
    j_partido = -1;
    double dist_nueva = 0;
    double dist_actual = 0;
    double max = 0;
    for (int k = 0; k < TOTAL_JORNADAS; k++) {
        if (viajes[visitante][k] == visitante && viajes[local][k] == visitante) {
            if (k - jornada == 1) {
                dist_actual = calculaDistancias_h3(jornada, jornada - 1, jornada + 1, viajes, local, visitante) + calculaDistancias_h3(k, k - 1, k + 1, viajes, visitante, local);
                dist_nueva = calculaDistancias_h3(k, k, k + 1, viajes, local, visitante) + calculaDistancias_h3(jornada, jornada - 1, jornada, viajes, visitante, local);   //aqui el local pasa a ser visitante y viceversa
            }
            else if (k - jornada == -1) {
                dist_actual = calculaDistancias_h3(jornada, jornada - 1, jornada + 1, viajes, local, visitante) + calculaDistancias_h3(k, k - 1, k + 1, viajes, visitante, local);
                dist_nueva = calculaDistancias_h3(k, k - 1, k, viajes, local, visitante) + calculaDistancias_h3(jornada, jornada, jornada + 1, viajes, visitante, local);   //aqui el local pasa a ser visitante y viceversa
            }
            else {
                dist_actual = calculaDistancias_h3(jornada, jornada - 1, jornada + 1, viajes, local, visitante) + calculaDistancias_h3(k, k - 1, k + 1, viajes, visitante, local);
                dist_nueva = calculaDistancias_h3(k, k - 1, k + 1, viajes, local, visitante) + calculaDistancias_h3(jornada, jornada - 1, jornada + 1, viajes, visitante, local);   //aqui el local pasa a ser visitante y viceversa
            }

            if (dist_actual - dist_nueva > max) {
                max = dist_actual - dist_nueva;
                j_partido = k;
            }
        }
    }

    return max;
}


void cambiaJornadas_h3(int k1, int k2, vector<vector<int>>& viajes, int local, int visitante) {
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


void imprimeCalendario(const vector<vector<int>> viajes, string n_archivo) {
    ofstream archivo(n_archivo);

    if (!archivo) {
        cerr << "Error al abrir el archivo" << std::endl;
    }
    for (int k = 0; k < TOTAL_JORNADAS; k++) {
        archivo << "Jornada " << k + 1 << ":" << endl;
        for (int i = 0; i < N; i++) {
            if (viajes[i][k] == i) {
                int rival = calculaRival(viajes, k, i);
                archivo << equipos[i].nombre << " vs " << equipos[rival].nombre << endl;
            }
        }
        archivo << "----------------------" << endl;
    }

    double distancia = 0;
    vector<double> dist_eq;
    for (int i = 0; i < N; i++) {
        double distancia_eq = 0;
        for (int k = 0; k < TOTAL_JORNADAS - 1; k++) {
            distancia += distanciasNBA[viajes[i][k]][viajes[i][k + 1]];
            distancia_eq += distanciasNBA[viajes[i][k]][viajes[i][k + 1]];
        }
        dist_eq.push_back(distancia_eq);
    }

    for (int i = 0; i < N; i++) {
        distancia += distanciasNBA[i][viajes[i][0]] + distanciasNBA[i][viajes[i][TOTAL_JORNADAS - 1]];
        dist_eq[i] += distanciasNBA[i][viajes[i][0]] + distanciasNBA[i][viajes[i][TOTAL_JORNADAS - 1]];
    }

    archivo << "Distancia final: " << distancia << endl;

    double maximo = -numeric_limits<double>::max();
    double minimo = numeric_limits<double>::max();
    int idxmin = -1;
    int idxmax = -1;

    for (int i = 0; i < N; i++) {
        archivo << equipos[i].nombre << ": " << dist_eq[i] << " millas recorridas" << endl;
        if (dist_eq[i] > maximo) {
            maximo = dist_eq[i];
            idxmax = i;
        }
        if (dist_eq[i] < minimo) {
            minimo = dist_eq[i];
            idxmin = i;
        }
    }

    archivo << "Equipo con menor distancia recorrida: " << equipos[idxmin].nombre << " con " << dist_eq[idxmin] << " millas" << endl;
    archivo << "Equipo con mayor distancia recorrida: " << equipos[idxmax].nombre << " con " << dist_eq[idxmax] << " millas" << endl;


    double disteste = 0;
    double distoeste = 0;

    for (int i = 0; i < N; i++) {
        if (i < 15) {
            disteste += dist_eq[i];
        }
        else {
            distoeste += dist_eq[i];
        }
    }

    archivo << "Media de millas recorridas Conferencia Este: " << disteste / 15 << " Millas Totales: " << disteste << endl;
    archivo << "Media de millas recorridas Conferencia Oeste: " << distoeste / 15 << " Millas Totales: " << distoeste << endl;


    archivo.close();
}

vector<vector<int>> copiar_calendario(const vector<vector<int>>& nuevo) {
    return nuevo;
}


int main() {

    int cont = 0;
    int Nu = 5;

    vector<vector<int>> mejor_calen;
    double mejor_dist = numeric_limits<double>::max();

    while (cont < Nu) {

        try {
            // Inicializar el entorno de Gurobi
            GRBEnv env = GRBEnv(true);
            env.set("LogFile", "nba_schedule.log");
            env.start();

            env.set(GRB_IntParam_Seed, cont + time(NULL)); // semilla variable

            // Crear el modelo
            GRBModel model = GRBModel(env);


            // Variables binarias: x[i][j][k] donde i es el equipo local, j es el visitante y k es la jornada
            GRBVar x[N][N][TOTAL_JORNADAS];
            for (int i = 0; i < N; ++i) {
                for (int j = 0; j < N; ++j) {
                    if (i != j) {
                        for (int k = 0; k < TOTAL_JORNADAS; ++k) {
                            x[i][j][k] = model.addVar(0.0, 1.0, 0.0, GRB_BINARY, "x_" + to_string(i) + "_" + to_string(j) + "_" + to_string(k));
                        }
                    }
                }
            }



            //Restricci�n de partidos de la divisi�n
            for (int i = 0; i < N; ++i) {
                for (int j = 0; j < EQUIPOS_POR_DIVISION - 1; j++) {

                    // Inicializar contadores para ver la cantidad de partidos que juegan en casa y fuera contra cada equipo de la divisi�n
                    GRBLinExpr partidosEnCasa = 0;
                    GRBLinExpr partidosFuera = 0;
                    int rival = equipos[i].rivales_division[j];

                    for (int k = 0; k < TOTAL_JORNADAS; ++k) {
                        partidosEnCasa += x[i][rival][k]; // partidos de i en casa contra j
                        partidosFuera += x[rival][i][k];  // partidos de i fuera de casa contra j
                    }

                    // Restricci�n para que cada equipo juegue 2 partidos en casa y 2 fuera de casa contra un rival de la misma divisi�n que ellos
                    model.addConstr(partidosEnCasa == 2, "PartidosEnCasa_" + to_string(i) + "_" + to_string(rival));
                    model.addConstr(partidosFuera == 2, "PartidosFuera_" + to_string(i) + "_" + to_string(rival));
                }
            }

            //---------------PARTIDOS FUERA DE LA DIVISI�N-----------------
// Restricci�n: Partidos dentro de la misma conferencia contra los que juegan 4 veces
            for (int i = 0; i < N; ++i) {
                for (int j = 0; j < NUM_RIVALES_CONF_1; j++) {

                    // Inicializar contadores para ver la cantidad de partidos que juegan en casa y fuera contra cada equipo de la divisi�n
                    GRBLinExpr partidosEnCasa = 0;
                    GRBLinExpr partidosFuera = 0;
                    int rival = equipos[i].rivales_conf1[j];

                    for (int k = 0; k < TOTAL_JORNADAS; ++k) {
                        partidosEnCasa += x[i][rival][k]; // partidos de i en casa contra j
                        partidosFuera += x[rival][i][k];  // partidos de i fuera de casa contra j
                    }

                    // Restricci�n para que cada equipo juegue 2 partidos en casa y 2 fuera de casa contra  cada uno de los 6 rivales a los que se enfrentan de este tipo en su misma conferencia
                    model.addConstr(partidosEnCasa == 2, "PartidosEnCasa_" + to_string(i) + "_" + to_string(rival));
                    model.addConstr(partidosFuera == 2, "PartidosFuera_" + to_string(i) + "_" + to_string(rival));
                }
            }



            // Restricci�n: Partidos dentro de la misma conferencia contra los que juegan 3 veces (2c y 1f)
            for (int i = 0; i < N; ++i) {
                for (int j = 0; j < NUM_RIVALES_CONF_2; j++) {

                    // Inicializar contadores para ver la cantidad de partidos que juegan en casa y fuera contra cada equipo de la divisi�n
                    GRBLinExpr partidosEnCasa = 0;
                    GRBLinExpr partidosFuera = 0;
                    int rival = equipos[i].rivales_conf2[j];

                    for (int k = 0; k < TOTAL_JORNADAS; ++k) {
                        partidosEnCasa += x[i][rival][k]; // partidos de i en casa contra j
                        partidosFuera += x[rival][i][k];  // partidos de i fuera de casa contra j
                    }

                    // Restricci�n para que cada equipo juegue 2 partidos en casa y 1 fuera de casa contra cada uno de los 2 rivales a los que se enfrentan de este tipo en su misma conferencia
                    model.addConstr(partidosEnCasa == 2, "PartidosEnCasa_" + to_string(i) + "_" + to_string(rival));
                    model.addConstr(partidosFuera == 1, "PartidosFuera_" + to_string(i) + "_" + to_string(rival));
                }
            }



            // Restricci�n: Partidos dentro de la misma conferencia contra los que juegan 3 veces (1c y 2f)
            for (int i = 0; i < N; ++i) {
                for (int j = 0; j < NUM_RIVALES_CONF_3; j++) {

                    // Inicializar contadores para ver la cantidad de partidos que juegan en casa y fuera contra cada equipo de la divisi�n
                    GRBLinExpr partidosEnCasa = 0;
                    GRBLinExpr partidosFuera = 0;
                    int rival = equipos[i].rivales_conf3[j];

                    for (int k = 0; k < TOTAL_JORNADAS; ++k) {
                        partidosEnCasa += x[i][rival][k]; // partidos de i en casa contra j
                        partidosFuera += x[rival][i][k];  // partidos de i fuera de casa contra j
                    }

                    // Restricci�n para que cada equipo juegue 1 partido en casa y 2 fuera de casa contra cada uno de los 2 rivales a los que se enfrentan de este tipo en su misma conferencia
                    model.addConstr(partidosEnCasa == 1, "PartidosEnCasa_" + to_string(i) + "_" + to_string(rival));
                    model.addConstr(partidosFuera == 2, "PartidosFuera_" + to_string(i) + "_" + to_string(rival));
                }
            }


            // Restricci�n: Partidos contra los equipos de la conferencia contraria
            for (int i = 0; i < N; ++i) {
                for (int j = 0; j < NUM_EQUIPOS_CONFERENCIA; j++) {

                    // Inicializar contadores para ver la cantidad de partidos que juegan en casa y fuera contra cada equipo de la divisi�n
                    GRBLinExpr partidosEnCasa = 0;
                    GRBLinExpr partidosFuera = 0;
                    int rival = equipos[i].rivales_interconf[j];

                    for (int k = 0; k < TOTAL_JORNADAS; ++k) {
                        partidosEnCasa += x[i][rival][k]; // partidos de i en casa contra j
                        partidosFuera += x[rival][i][k];  // partidos de i fuera de casa contra j
                    }

                    // Restricci�n para que cada equipo juegue 2 partidos en casa y 2 fuera de casa contra cada uno de los 15 rivales a los que se enfrentan de la otra conferencia
                    model.addConstr(partidosEnCasa == 1, "PartidosEnCasa_" + to_string(i) + "_" + to_string(rival));
                    model.addConstr(partidosFuera == 1, "PartidosFuera_" + to_string(i) + "_" + to_string(rival));
                }
            }






            // Restricci�n: Cada equipo solo puede jugar un partido por jornada
            for (int i = 0; i < N; ++i) {
                for (int k = 0; k < TOTAL_JORNADAS; k++) {

                    // Inicializar contadores para ver la cantidad de partidos que juega cada equipo por jornada
                    GRBLinExpr partidosPorJornada = 0;

                    for (int j = 0; j < N; ++j) {
                        if (i != j) {
                            partidosPorJornada += x[i][j][k]; // partidos de i en casa contra j 
                            partidosPorJornada += x[j][i][k];  // partidos de i fuera de casa contra j
                        }

                    }

                    // Restricci�n para que cada equipo juegue 1 partido por jornada
                    model.addConstr(partidosPorJornada == 1, "UnPartidoPorJornada_" + to_string(i) + "_Jornada_" + to_string(k));
                }
            }

            // Funci�n objetivo vac�a (solo generar el calendario)
            model.setObjective(GRBLinExpr(5), GRB_MINIMIZE);

            // Optimizar el modelo
            model.optimize();

            if (model.get(GRB_IntAttr_Status) == GRB_OPTIMAL) {

                vector<vector<int>> viajes;
                bool encontrado;


                for (int i = 0; i < N; i++) {
                    vector<int> recorrido;
                    for (int k = 0; k < TOTAL_JORNADAS; k++) {
                        encontrado = false;
                        for (int j = 0; j < N && !encontrado; j++) {
                            if (i != j && x[i][j][k].get(GRB_DoubleAttr_X) > 0.5) {
                                recorrido.push_back(i);
                                encontrado = true;
                            }
                            else if (i != j && x[j][i][k].get(GRB_DoubleAttr_X) > 0.5) {
                                recorrido.push_back(j);
                                encontrado = true;
                            }
                        }
                    }

                    viajes.push_back(recorrido);

                }

                double distancia = 0;
                for (int i = 0; i < N; i++) {
                    for (int k = 0; k < TOTAL_JORNADAS - 1; k++) {
                        distancia += distanciasNBA[viajes[i][k]][viajes[i][k + 1]];
                    }
                }

                for (int i = 0; i < N; i++) {
                    distancia += distanciasNBA[i][viajes[i][0]] + distanciasNBA[i][viajes[i][TOTAL_JORNADAS - 1]];
                }


                if (distancia < mejor_dist) {
                    mejor_dist = distancia;
                    mejor_calen = copiar_calendario(viajes);
                }


            }
            else {
                cout << "No se encontr� una soluci�n �ptima." << std::endl;
            }

            cont++;
        }

        catch (GRBException e) {
            cout << "Error: " << e.getMessage() << endl;
        }
        catch (...) {
            cout << "Error desconocido" << endl;
        }

    }

    imprimeCalendario(mejor_calen, "Heuristica_1.txt");



    return 0;
}
