#include "datosNBA_4eq.h"

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

void cambiaJornadast2(vector<vector<int>>& viajes, double& distancia) {

    int k1 = rand() % TOTAL_JORNADAS;
    int k2 = rand() % TOTAL_JORNADAS;
    while (k1 == k2) {
        k2 = rand() % TOTAL_JORNADAS;
    }

    if (k1 > k2)
        swap(k1, k2);

    double distancia_inicial = calculaDistancias_h2(k1, k1 - 1, k1 + 1, viajes) + calculaDistancias_h2(k2, k2 - 1, k2 + 1, viajes);
    double distancia_final;
    if (k2 - k1 == 1) {
        distancia_final = calculaDistancias_h2(k2, k1 - 1, k1, viajes) + calculaDistancias_h2(k1, k2, k2 + 1, viajes);
    }
    else {
        distancia_final = calculaDistancias_h2(k2, k1 - 1, k1 + 1, viajes) + calculaDistancias_h2(k1, k2 - 1, k2 + 1, viajes);
    }


    cambiaJornadas_h2(k1, k2, viajes);
    distancia += (distancia_final - distancia_inicial);

}

void VNS(vector<vector<int>>& viajes, double& distancia, int max_iter, int limite_sin_mej, int k1, int k2, int k3) {
    double distancia_mejor = distancia;
    vector<vector<int>> mejor_calendario;

    mejor_calendario = copiar_calendario(viajes);

    int iter_sin_mej = 0;

    for (int iter = 0; iter < max_iter && iter_sin_mej < limite_sin_mej; iter++) {
        
        int k = 1;
        while (k <= 3) {
            vector<vector<int>> nuevoCalendario = copiar_calendario(mejor_calendario);
            distancia = distancia_mejor;

            int cambios = (k == 1) ? k1 : (k == 2) ? k2 : k3;
            for (int i = 0; i < cambios; i++) {
                cambiaJornadast2(nuevoCalendario, distancia);
            }

            int j1 = 0;
            double diferencia = 0;
            double distancia_inicial = 0;
            double distancia_final = 0;
            while (j1 < TOTAL_JORNADAS) {
                int j2 = j1 + 1;
                while (j2 < TOTAL_JORNADAS) {
                    if (j2 - j1 == 1) {
                        distancia_inicial = calculaDistancias_h2(j1, j1 - 1, j1 + 1, nuevoCalendario) + calculaDistancias_h2(j2, j2 - 1, j2 + 1, nuevoCalendario);
                        distancia_final = calculaDistancias_h2(j2, j1 - 1, j1, nuevoCalendario) + calculaDistancias_h2(j1, j2, j2 + 1, nuevoCalendario);
                    }
                    else {
                        distancia_inicial = calculaDistancias_h2(j1, j1 - 1, j1 + 1, nuevoCalendario) + calculaDistancias_h2(j2, j2 - 1, j2 + 1, nuevoCalendario);
                        distancia_final = calculaDistancias_h2(j2, j1 - 1, j1 + 1, nuevoCalendario) + calculaDistancias_h2(j1, j2 - 1, j2 + 1, nuevoCalendario);
                    }


                    if (distancia_final < distancia_inicial) {
                        diferencia = distancia_inicial - distancia_final;

                        distancia -= diferencia;
                        cambiaJornadas_h2(j1, j2, nuevoCalendario);
                        //cout << "He cambiado la jornada " << j1 + 1 << " por la jornada " << j2 + 1 << " recortando " << diferencia << " millas" << endl;
                        j1 = -1;
                        break;
                    }
                    j2++;
                }
                j1++;
            }

            if (distancia < distancia_mejor) {
                distancia_mejor = distancia;
                mejor_calendario = copiar_calendario(nuevoCalendario);
                iter_sin_mej = 0;
                k = 1;
            }
            else {
                k++;
            }
        }
        iter_sin_mej++;

    }
    viajes = copiar_calendario(mejor_calendario);
    distancia = distancia_mejor;
}





int main() {

    ifstream archivo("Calendario_4eq.txt"); // Abre el archivo en modo lectura

    if (!archivo) { // Verifica si el archivo se abri� correctamente
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
        distancia += distanciasNBA[i][viajes[i][0]] + distanciasNBA[i][viajes[i][TOTAL_JORNADAS - 1]];
    }

    cout << "Distancia inicial dada por el modelo: " << distancia << endl;

    vector<vector<int>> calendario_inicial = copiar_calendario(viajes);
    double dist_inicial = distancia;

    vector<double> t = { 50, 100 };
    //double t_inicial = 100;
    double t_minimo = 0.01;
    vector<int> Ms = { 2, 3, 4, 5, 6, 8, 10 };
    vector<double> alphas = { 0.90, 0.95, 0.99 };

    ofstream archivo2("resultados_vns_h2_4eq.txt"); // Abre el archivo en modo lectura

    if (!archivo2) { // Verifica si el archivo se abri� correctamente
        cerr << "Error al abrir el archivo" << std::endl;
        return 1;
    }

    int max_iter = 100;
    int lim_sin_mej = 10;
    int k1 = 2;
    int k2 = 3;
    int k3 = 4;

    vector<int> iteraciones = { 100,200,400 };
    vector<int> limites = { 10,50 };



    archivo2 << distancia << endl;
    cout << distancia << endl;

    double dist_mejor = numeric_limits<double>::max();
    vector<vector<int>> mejor_calend;

    for (int max_iter : iteraciones) {
        for (int lim_sin_mej : limites) {
            VNS(viajes, distancia, max_iter, lim_sin_mej, k1, k2, k3);
            archivo2 << "Max iter = " << max_iter << ", limite sin mej = " << lim_sin_mej << ", Distancia -> " << distancia << endl;
            cout << "Max iter = " << max_iter << ", limite sin mej = " << lim_sin_mej << ", Distancia -> " << distancia << endl;
            if (distancia < dist_mejor) {
                dist_mejor = distancia;
                mejor_calend = copiar_calendario(viajes);
            }
            viajes = copiar_calendario(calendario_inicial);
            distancia = dist_inicial;
        }
    }




    archivo2.close(); // Cierra el archivo

  
    imprimeCalendario(mejor_calend, "VNS_h2_4eq.txt");


    return 0;
}

