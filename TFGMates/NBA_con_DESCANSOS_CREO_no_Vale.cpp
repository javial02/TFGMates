//EN ESTE MODELO NO SE ESTÁ CONTANDO CON LA DISTANCIA RECORRIDA DE LOS EQUIPOS VISITANTES EN LA PRIMERA JORNADA DE LA TEMPORADA.
//SE DA POR HECHO DE QUE SE EMPIEZA A CONTAR A PARTIR DE LA JORNADA 1

#include <vector>
#include <string>
#include <iostream>
#include "gurobi_c++.h"

using namespace std;

const int N = 5;								//Número de franquicias en total
const int CONFERENCIAS = 2;						//Número de conferencias
const int DIVISIONES_POR_CONFERENCIA = 3;		//Número de divisiones por conferencia
const int EQUIPOS_POR_DIVISION = 5;				//Número de equipos por cada división
const int NUM_RIVALES_CONF_1 = 6;               //Número de rivales fuera de la división, en la misma conferencia, con los que se juegan 4 partidos
const int NUM_RIVALES_CONF_2 = 2;               //Número de rivales fuera de la división, en la misma conferencia, con los que se juegan 3 partidos (2c y 1f)
const int NUM_RIVALES_CONF_3 = 2;               //Número de rivales fuera de la división, en la misma conferencia, con los que se juegan 3 partidos (1c y 2f)
const int NUM_EQUIPOS_CONFERENCIA = 15;         //Número de equipos por conferencia
const int TOTAL_JORNADAS = 20;                  //Número de jornadas
const int NUM_DIVISIONES = 6;                   //Número de divisiones

struct InfoEquipo {
    int id;                                     //Numeración de equipos
    string nombre;								//Nombre del equipo
    string conferencia;							//Conferencia a la que pertenece el equipo
    string division;							//División a la que pertenece el equipo
    vector<int> rivales_division;			    //Lista de rivales de su misma división
    vector<int> rivales_conf1;				    //Lista de rivales de su misma conferencia pero distinta division con los que juega 4 partidos
    vector<int> rivales_conf2;				    //Lista de rivales de su misma conferencia pero distinta división con los que juega 3 partidos (2c y 1f)
    vector<int> rivales_conf3;				    //Lista de rivales de su misma conferencia pero distinta división con los que juega 3 partidos (1c y 2f)
    vector<int> rivales_interconf;			    //Lista de rivales que no son de su conferencia
};



vector<InfoEquipo> equipos = {
    {0, "Boston Celtics", "Este", "Atlántico", {1, 2, 3}, {8, 7, 5, 10, 12}, {9, 13}, {6, 11}, {4,5,6,7}},
    {1, "Brooklyn Nets", "Este", "Atlántico", {0, 2, 3}, {13, 6, 9, 10, 5, 11}, {8}, {7, 12}, {4,5,6,7}},
    {2, "Philadelphia 76ers", "Este", "Atlántico", {0, 1, 3}, {6, 9, 5, 13, 12, 11}, {7, 10}, {8}, {4,5,6,7}},
    {3, "New York Knicks", "Este", "Atlántico", {0, 1, 2}, {7, 8, 11, 13, 6}, {5, 12}, {10, 9}, {4,5,6,7}},
    {4,"Oklahoma City Thunder", "Oeste", "Noroeste", {5, 6, 7}, {28, 26, 29, 21, 24, 20}, {23, 25}, {27, 22} , {0, 1, 2, 3} },
    {5,"Minnesota Timberwolves", "Oeste", "Noroeste", {4, 6, 7}, {28, 27, 29, 21, 24, 22}, {26, 20}, {25, 23} , {0, 1, 2, 3} },
    {6,"Utah Jazz", "Oeste", "Noroeste", {4, 5, 7}, {26, 25, 27, 21, 22, 23}, {28, 24}, {29, 20} , {0, 1, 2, 3} },
    {7,"Denver Nuggets", "Oeste", "Noroeste", {4, 5, 6}, {25, 26, 29, 24, 23, 20}, {27, 22}, {28, 21} , {0, 1, 2, 3} }
};

vector<vector<double>> distanciasNBA = {
    {0, 217.9, 309.3, 213.78, 1689.49, 1399.98, 2365.09, 1969.76},
    {217.9, 0, 98.12, 5.45, 1467.56, 1209, 2174.12, 1778.79},
    {309.3, 98.12, 0, 96.67, 1383.31, 1175.94, 2141.06, 1726.81},
    {213.78, 5.45, 96.67, 0, 1451.06, 1208.36, 2173.39, 1777.98},
    {1689.49, 1467.56, 1383.31, 1451.06, 0, 789.32, 1188.33, 677.57},
    {1399.98, 1209, 1175.94, 1208.36, 789.32, 0, 1247.22, 914.03},
    {2365.09, 2174.12, 2141.06, 2173.39, 1188.33, 1247.22, 0, 519.27},
    {1969.76, 1778.79, 1726.81, 1777.98, 677.57, 914.03, 519.27, 0},
};




int main() {

    try {
        // Inicializar el entorno de Gurobi
        GRBEnv env = GRBEnv(true);
        env.set("LogFile", "nba_schedule.log");
        env.start();

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


        // Variables z[i][j][j'][k]: Indica si el equipo i viaja de j a j' entre jornadas k-1 y k
        GRBVar z[N][N][N][TOTAL_JORNADAS - 1];
        for (int i = 0; i < N; ++i) {
            for (int j = 0; j < N; ++j) {
                for (int j2 = 0; j2 < N; ++j2) {
                    for (int k = 1; k < TOTAL_JORNADAS; ++k) {
                        z[i][j][j2][k - 1] = model.addVar(0.0, 1.0, 0.0, GRB_BINARY, "z_" + to_string(i) + "_" + to_string(j) + "_" + to_string(j2) + "_Jornada_" + to_string(k));
                    }
                }
            }
        }


        /*// Variables d[i][k]: Indica si el equipo i está descansando en la jornada k
        GRBVar d[N][TOTAL_JORNADAS];
        for (int i = 0; i < N; ++i) {
            for (int k = 0; k < TOTAL_JORNADAS; ++k) {
                d[i][k] = model.addVar(0.0, 1.0, 0.0, GRB_BINARY, "d_" + to_string(i) + "_" + to_string(k));
            }
        }*/

        //Restricción de partidos de la división
        for (int i = 0; i < N; ++i) {
            for (int j = 0; j < EQUIPOS_POR_DIVISION - 1; j++) {

                // Inicializar contadores para ver la cantidad de partidos que juegan en casa y fuera contra cada equipo de la división
                GRBLinExpr partidosEnCasa = 0;
                GRBLinExpr partidosFuera = 0;
                int rival = equipos[i].rivales_division[j];

                for (int k = 0; k < TOTAL_JORNADAS; ++k) {
                    partidosEnCasa += x[i][rival][k]; // partidos de i en casa contra j
                    partidosFuera += x[rival][i][k];  // partidos de i fuera de casa contra j
                }

                // Restricción para que cada equipo juegue 2 partidos en casa y 2 fuera de casa contra un rival de la misma división que ellos
                model.addConstr(partidosEnCasa == 2, "PartidosEnCasa_" + to_string(i) + "_" + to_string(rival));
                model.addConstr(partidosFuera == 2, "PartidosFuera_" + to_string(i) + "_" + to_string(rival));
            }
        }

        //---------------PARTIDOS FUERA DE LA DIVISIÓN-----------------
       /* // Restricción: Partidos dentro de la misma conferencia contra los que juegan 4 veces
        for (int i = 0; i < N; ++i) {
            for (int j = 0; j < NUM_RIVALES_CONF_1; j++) {

                // Inicializar contadores para ver la cantidad de partidos que juegan en casa y fuera contra cada equipo de la división
                GRBLinExpr partidosEnCasa = 0;
                GRBLinExpr partidosFuera = 0;
                int rival = equipos[i].rivales_conf1[j];

                for (int k = 0; k < TOTAL_JORNADAS; ++k) {
                    partidosEnCasa += x[i][rival][k]; // partidos de i en casa contra j
                    partidosFuera += x[rival][i][k];  // partidos de i fuera de casa contra j
                }

                // Restricción para que cada equipo juegue 2 partidos en casa y 2 fuera de casa contra  cada uno de los 6 rivales a los que se enfrentan de este tipo en su misma conferencia
                model.addConstr(partidosEnCasa == 2, "PartidosEnCasa_" + to_string(i) + "_" + to_string(rival));
                model.addConstr(partidosFuera == 2, "PartidosFuera_" + to_string(i) + "_" + to_string(rival));
            }
        }


        // Restricción: Partidos dentro de la misma conferencia contra los que juegan 3 veces (2c y 1f)
        for (int i = 0; i < N; ++i) {
            for (int j = 0; j < NUM_RIVALES_CONF_2; j++) {

                // Inicializar contadores para ver la cantidad de partidos que juegan en casa y fuera contra cada equipo de la división
                GRBLinExpr partidosEnCasa = 0;
                GRBLinExpr partidosFuera = 0;
                int rival = equipos[i].rivales_conf2[j];

                for (int k = 0; k < TOTAL_JORNADAS; ++k) {
                    partidosEnCasa += x[i][rival][k]; // partidos de i en casa contra j
                    partidosFuera += x[rival][i][k];  // partidos de i fuera de casa contra j
                }

                // Restricción para que cada equipo juegue 2 partidos en casa y 1 fuera de casa contra cada uno de los 2 rivales a los que se enfrentan de este tipo en su misma conferencia
                model.addConstr(partidosEnCasa == 2, "PartidosEnCasa_" + to_string(i) + "_" + to_string(rival));
                model.addConstr(partidosFuera == 1, "PartidosFuera_" + to_string(i) + "_" + to_string(rival));
            }
        }


        // Restricción: Partidos dentro de la misma conferencia contra los que juegan 3 veces (1c y 2f)
        for (int i = 0; i < N; ++i) {
            for (int j = 0; j < NUM_RIVALES_CONF_3; j++) {

                // Inicializar contadores para ver la cantidad de partidos que juegan en casa y fuera contra cada equipo de la división
                GRBLinExpr partidosEnCasa = 0;
                GRBLinExpr partidosFuera = 0;
                int rival = equipos[i].rivales_conf3[j];

                for (int k = 0; k < TOTAL_JORNADAS; ++k) {
                    partidosEnCasa += x[i][rival][k]; // partidos de i en casa contra j
                    partidosFuera += x[rival][i][k];  // partidos de i fuera de casa contra j
                }

                // Restricción para que cada equipo juegue 1 partido en casa y 2 fuera de casa contra cada uno de los 2 rivales a los que se enfrentan de este tipo en su misma conferencia
                model.addConstr(partidosEnCasa == 1, "PartidosEnCasa_" + to_string(i) + "_" + to_string(rival));
                model.addConstr(partidosFuera == 2, "PartidosFuera_" + to_string(i) + "_" + to_string(rival));
            }
        }
        */

        // Restricción: Partidos contra los equipos de la conferencia contraria
        for (int i = 0; i < N; ++i) {
            for (int j = 0; j < NUM_EQUIPOS_CONFERENCIA; j++) {

                // Inicializar contadores para ver la cantidad de partidos que juegan en casa y fuera contra cada equipo de la división
                GRBLinExpr partidosEnCasa = 0;
                GRBLinExpr partidosFuera = 0;
                int rival = equipos[i].rivales_interconf[j];

                for (int k = 0; k < TOTAL_JORNADAS; ++k) {
                    partidosEnCasa += x[i][rival][k]; // partidos de i en casa contra j
                    partidosFuera += x[rival][i][k];  // partidos de i fuera de casa contra j
                }

                // Restricción para que cada equipo juegue 2 partidos en casa y 2 fuera de casa contra cada uno de los 15 rivales a los que se enfrentan de la otra conferencia
                model.addConstr(partidosEnCasa == 1, "PartidosEnCasa_" + to_string(i) + "_" + to_string(rival));
                model.addConstr(partidosFuera == 1, "PartidosFuera_" + to_string(i) + "_" + to_string(rival));
            }
        }


        // Restricción: Cada equipo solo puede jugar un partido por jornada
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

                // Restricción para que cada equipo juegue 1 partido por jornada
                model.addConstr(partidosPorJornada <= 1, "UnPartidoPorJornada_" + to_string(i) + "_Jornada_" + to_string(k));
            }
        }

        // Restricción: Solo un equipo descansa por jornada
        for (int k = 0; k < TOTAL_JORNADAS; k++) {
            GRBLinExpr sum_descansos_por_jornada = 0;
            for (int i = 0; i < N; i++) {
                sum_descansos_por_jornada += d[i][k];
            }
            model.addConstr(sum_descansos_por_jornada == 1, "Descansos_jornada_" + to_string(k));
        }


        for (int k = 0; k < TOTAL_JORNADAS; k++) {
            for (int i = 0; i < N; i++) {
                GRBLinExpr sum_partidos = 0;
                for (int j = 0; j < EQUIPOS_POR_DIVISION; j++) {
                    if (i != j) {
                        sum_partidos += x[i][j][k];
                        sum_partidos += x[j][i][k];
                    }
                }
                model.addConstr(sum_partidos + d[i][k] == 1, "O se descansa o se juega");
            }
        }


        /*// Restricción: Cada equipo descansa 4 veces en la temporada      CREO QUE SOBRA!!!!!!!!!!!
        for (int i = 0; i < N; i++) {
            GRBLinExpr sum_descansos = 0;
            for (int k = 0; k < TOTAL_JORNADAS; k++) {
                sum_descansos += d[i][k];
            }
            model.addConstr(sum_descansos == 4, "Descansos del equipo_" + to_string(i));
        }*/





        // Relación entre y y z: Si viajan entre ciudades
        for (int i = 0; i < N; i++) {
            for (int j1 = 0; j1 < N; j1++) {
                for (int j2 = 0; j2 < N; j2++) {
                    for (int k = 1; k < TOTAL_JORNADAS; k++) {
                        if (j1 != i && j2 != i) {
                            model.addConstr(z[i][i][i][k - 1] >= (x[i][j1][k - 1] + x[i][j2][k] - 1), "El equipo i juega dos partidos seguidos en casa");
                            model.addConstr(z[i][i][j2][k - 1] >= x[i][j1][k - 1] + x[j2][i][k] - 1, "El equipo i juega un partido en casa y el siguiente fuera");
                            model.addConstr(z[i][j1][i][k - 1] >= x[j1][i][k - 1] + x[i][j2][k] - 1, "El equipo i juega un partido fuera y el sigueinte en casa");
                            model.addConstr(z[i][j1][j2][k - 1] >= x[j1][i][k - 1] + x[j2][i][k] - 1, "El equipo i juega dos partidos seguidos fuera de casa");
                            /*model.addConstr(z[i][i][i][k - 1] <= x[i][j1][k - 1], "RestriccionViaje1_");
                            model.addConstr(z[i][i][j2][k - 1] <= x[i][j1][k - 1], "RestriccionViaje1_");
                            model.addConstr(z[i][j1][i][k - 1] <= x[j1][i][k - 1], "RestriccionViaje1_");
                            model.addConstr(z[i][j1][j2][k - 1] <= x[j1][i][k - 1], "RestriccionViaje1_");
                            model.addConstr(z[i][i][i][k - 1] <= x[i][j2][k], "RestriccionViaje2_");
                            model.addConstr(z[i][j1][i][k - 1] <= x[i][j2][k], "RestriccionViaje2_");
                            model.addConstr(z[i][i][j2][k - 1] <= x[j2][i][k], "RestriccionViaje2_");
                            model.addConstr(z[i][j1][j2][k - 1] <= x[j2][i][k], "RestriccionViaje2_");*/

                        }
                       
                    }
                }
            }
        }

        /*for (int i = 0; i < N; i++) {
            for (int k = 1; k < TOTAL_JORNADAS; k++) {
                GRBLinExpr sum_viajes = 0;
                for (int j1 = 0; j1 < N; j1++) {
                    for (int j2 = 0; j2 < N; j2++) {
                        sum_viajes += z[i][j1][j2][k - 1];
                    }
                }
                model.addConstr(sum_viajes == 1, "Max_1_viaje_" + to_string(i) + "_J" + to_string(k));
            }
        }*/




        // Función objetivo: Minimizar distancias
        GRBLinExpr distanciaTotal = 0;
        for (int i = 0; i < N; ++i) {
            for (int j = 0; j < N; ++j) {
                for (int j2 = 0; j2 < N; ++j2) {
                    for (int k = 1; k < TOTAL_JORNADAS; ++k) {
                        distanciaTotal += z[i][j][j2][k - 1] * distanciasNBA[j][j2];
                    }
                }
            }
        }


        model.setObjective(distanciaTotal, GRB_MINIMIZE);






        // Función objetivo vacía (solo generar el calendario)
        //model.setObjective(GRBLinExpr(5), GRB_MINIMIZE);

        // Optimizar el modelo
        model.optimize();

        if (model.get(GRB_IntAttr_Status) == GRB_OPTIMAL) {
            cout << "Solución óptima encontrada:" << model.get(GRB_DoubleAttr_ObjVal) << std::endl;

            // Imprimir el calendario
            for (int k = 0; k < TOTAL_JORNADAS; ++k) {
                cout << "Jornada " << k + 1 << ":" << endl;
                for (int i = 0; i < N; ++i) {
                    int cont = 0;
                    for (int j = 0; j < N; ++j) {
                        
                        if (i != j && x[i][j][k].get(GRB_DoubleAttr_X) > 0.5) {
                            cout << equipos[i].nombre << " vs " << equipos[j].nombre << endl;
                        }
                 
                    }
                    if (d[i][k].get(GRB_DoubleAttr_X) > 0.5) {
                        cout << equipos[i].nombre << " descansa" << endl;
                    }
                }

                cout << "-----------------------------" << endl;
            }

        }
        else {
            cout << "No se encontró una solución óptima." << std::endl;
        }

    }
    catch (GRBException e) {
        cout << "Error: " << e.getMessage() << endl;
    }
    catch (...) {
        cout << "Error desconocido" << endl;
    }
    return 0;
}

