#include <vector>
#include <string>
#include <iostream>
#include <fstream>  // Biblioteca para manejar archivos
#include "gurobi_c++.h"
#include "datosNBA_8eq.h"

int main() {

    try {
        /* ofstream archivo("Calendario_modelo_4_equipos_con_numeros.txt"); // Abre el archivo (lo crea si no existe)

         if (!archivo) {  // Verifica si se abri� correctamente
             cerr << "Error al abrir el archivo" << std::endl;
             return 1;
         }*/


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

        /*
        // L�mite m�ximo de diferencia acumulada entre partidos en casa y fuera por equipo
        const int MAX_DIF_LOCAL_VISITANTE = 7;

        // Variables auxiliares para la diferencia absoluta en cada jornada
        GRBVar diff[N][TOTAL_JORNADAS];

        for (int i = 0; i < N; ++i) {
            for (int k = 0; k < TOTAL_JORNADAS; ++k) {
                diff[i][k] = model.addVar(0.0, GRB_INFINITY, 0.0, GRB_INTEGER,"diff_" + to_string(i) + "_k_" + to_string(k));
            }
        }*/


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



        // Restricci�n: Partidos dentro de la divisi�n
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

        // Restricci�n: Partidos contra los equipos de la conferencia contraria
        for (int i = 0; i < N; ++i) {
            for (int j = 0; j < 4; j++) {

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



        // Funci�n objetivo vac�a (solo generar el calendario)
        model.setObjective(GRBLinExpr(5), GRB_MINIMIZE);

        // Optimizar el modelo
        model.optimize();

        if (model.get(GRB_IntAttr_Status) == GRB_OPTIMAL) {
            cout << "Soluci�n �ptima encontrada:" << std::endl;

            // Imprimir el calendario
            for (int k = 0; k < TOTAL_JORNADAS; ++k) {
                cout << "Jornada " << k + 1 << ":" << endl;
                for (int i = 0; i < N; ++i) {
                    for (int j = 0; j < N; ++j) {
                        if (i != j && x[i][j][k].get(GRB_DoubleAttr_X) > 0.5) {
                            cout << equipos[i].nombre << " vs " << equipos[j].nombre << endl;
                        }
                    }
                }

                int contador = 0;
                for (int i = 0; i < N; ++i) {
                    for (int j = 0; j < N; ++j) {
                        if (i != j && x[i][j][k].get(GRB_DoubleAttr_X) > 0.5) {
                            contador++;
                        }
                    }
                }

                cout << contador << endl;

                cout << "-----------------------------" << endl;
            }

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

            /*for (int i = 0; i < N; i++) {
                for (int j = 0; j < TOTAL_JORNADAS; j++) {
                    if (j != TOTAL_JORNADAS - 1) {
                        archivo << viajes[i][j] << " ";
                    }
                    else {
                        archivo << viajes[i][j] << "\n";
                    }

                }

            }


            archivo.close(); // Cierra el archivo*/

        }
        else {
            cout << "No se encontr� una soluci�n �ptima." << std::endl;
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

