#include "datosNBA163.h"


vector<vector<int>> copiar_calendario(const vector<vector<int>> nuevo) {
    return nuevo;
}


int partido_posterior(const vector<vector<int>>& viajes, int i, int jornada) {
    for (int k = jornada + 1; k < viajes[0].size(); k++) {
        if (viajes[i][k] != -1) {
            return k;
        }
    }

    return viajes[0].size() + 100;
}

int partido_anterior(const vector<vector<int>>& viajes, int i, int jornada) {
    for (int k = jornada - 1; k >= 0; k--) {
        if (viajes[i][k] != -1) {
            return k;
        }
    }
    return -1;
}

void actualiza_moviendo(vector<vector<int>>& viajes, vector<vector<int>>& rivales, int jornada, int j_prev, int local, int visitante) {

    rivales[local][j_prev] = -1;
    rivales[visitante][j_prev] = -1;
    rivales[local][jornada] = visitante;
    rivales[visitante][jornada] = local;

    viajes[local][j_prev] = -1;
    viajes[visitante][j_prev] = -1;
    viajes[local][jornada] = local;
    viajes[visitante][jornada] = local;

}




void distancias_iniciales(const vector<vector<int>>& viajes, int i, int rival, int k, double& distancia_inicial, double& distancia_hipotetica, int jornadas) {
    int ipost = partido_posterior(viajes, i, k);
    int rpost = partido_posterior(viajes, rival, k);
    int iant = partido_anterior(viajes, i, k);
    int rant = partido_anterior(viajes, rival, k);

    if (iant >= 0 && ipost < viajes[0].size()) {
        distancia_inicial += distanciasNBA[viajes[i][iant]][viajes[i][k]] + distanciasNBA[viajes[i][k]][viajes[i][ipost]];
        distancia_hipotetica += distanciasNBA[viajes[i][iant]][viajes[i][ipost]];
    }
    else if (iant < 0) {
        distancia_inicial += distanciasNBA[i][viajes[i][k]] + distanciasNBA[viajes[i][k]][viajes[i][ipost]];
        distancia_hipotetica += distanciasNBA[i][viajes[i][ipost]];
    }
    else if (ipost >= viajes[0].size()) {
        distancia_inicial += distanciasNBA[viajes[i][iant]][viajes[i][k]] + distanciasNBA[viajes[i][k]][i];
        distancia_hipotetica += distanciasNBA[viajes[i][iant]][i];
    }


    if (rant >= 0 && rpost < viajes[0].size()) {
        distancia_inicial += distanciasNBA[viajes[rival][rant]][viajes[rival][k]] + distanciasNBA[viajes[rival][k]][viajes[rival][rpost]];
        distancia_hipotetica += distanciasNBA[viajes[rival][rant]][viajes[rival][rpost]];
    }
    else if (rant < 0) {
        distancia_inicial += distanciasNBA[rival][viajes[rival][k]] + distanciasNBA[viajes[rival][k]][viajes[rival][rpost]];
        distancia_hipotetica += distanciasNBA[rival][viajes[rival][rpost]];
    }
    else if (rpost >= viajes[0].size()) {
        distancia_inicial += distanciasNBA[viajes[rival][rant]][viajes[rival][k]] + distanciasNBA[viajes[rival][k]][rival];
        distancia_hipotetica += distanciasNBA[viajes[rival][rant]][rival];
    }

}

int buscaRival(int jornada, int rival, const vector<vector<int>>& viajes) {
    for (int i = 0; i < viajes.size(); i++) {
        if (viajes[i][jornada] == rival && i != rival) {
            return i;
        }
    }

    return -1;
}

int calculaRival(const vector<vector<int>>& viajes, int jornada, int rival) {
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

vector<vector<int>> muestraDist(const vector<vector<int>>& viajes) {
    vector<vector<int>> viajes_final;
    for (int i = 0; i < N; i++) {
        vector<int> v;
        for (int k = 0; k < viajes[0].size(); k++) {
            if (viajes[i][k] != -1) {
                v.push_back(viajes[i][k]);
            }
        }
        viajes_final.push_back(v);
    }


    double distancia_final = 0;
    for (int i = 0; i < N; i++) {
        for (int k = 0; k < TOTAL_JORNADAS - 1; k++) {
            distancia_final += distanciasNBA[viajes_final[i][k]][viajes_final[i][k + 1]];
        }
    }

    for (int i = 0; i < N; i++) {
        distancia_final += distanciasNBA[i][viajes_final[i][0]] + distanciasNBA[i][viajes_final[i][81]];
    }

    cout << "Distancia final: " << distancia_final << endl;

    return viajes_final;
}



double nuevaDist(const vector<vector<int>>& viajes, int local, int visitante) {
    vector<vector<int>> viajes_final;
    vector<int> v;
    for (int k = 0; k < viajes[local].size(); k++) {
        if (viajes[local][k] != -1) {
            v.push_back(viajes[local][k]);
        }
    }
    viajes_final.push_back(v);
    vector<int> v2;
    for (int k = 0; k < viajes[visitante].size(); k++) {
        if (viajes[visitante][k] != -1) {
            v2.push_back(viajes[visitante][k]);
        }
    }
    viajes_final.push_back(v2);


    double distancia_final = 0;
    for (int i = 0; i < 2; i++) {
        for (int k = 0; k < viajes_final[0].size() - 1; k++) {
            distancia_final += distanciasNBA[viajes_final[i][k]][viajes_final[i][k + 1]];
        }
    }

    double x = 0;
    distancia_final += distanciasNBA[local][viajes_final[0][0]] + distanciasNBA[local][viajes_final[0][81]];
    distancia_final += distanciasNBA[visitante][viajes_final[1][0]] + distanciasNBA[visitante][viajes_final[1][81]];


    //cout << "Distancia final: " << distancia_final << endl;

    return distancia_final;
}


bool hayPartido(const vector<vector<int>>& viajes, int k2, int k, int local, int visitante) {

    if (k2 > k) {
        for (int i = k + 1; i < k2; i++) {
            if (viajes[local][i] != -1 && viajes[local][i] != local) {
                return true;
            }
            if (viajes[visitante][i] != -1 && viajes[visitante][i] != local) {
                return true;
            }
        }
    }
    else {
        for (int i = k2 + 1; i < k; i++) {
            if (viajes[local][i] != -1 && viajes[local][i] != local) {
                return true;
            }
            if (viajes[visitante][i] != -1 && viajes[visitante][i] != local) {
                return true;
            }
        }

    }

    return false;

}


bool tieneTresPartidosSeguidos(const vector<vector<int>>& viajes, int i) {
    int consecutivos = 0;
    for (int d = 0; d < viajes[0].size(); ++d) {
        if (viajes[i][d] != -1) {
            consecutivos++;
            if (consecutivos >= 3) return true;
        }
        else {
            consecutivos = 0;
        }
    }
    return false;
}

bool tieneDescansoExcesivo(const vector<vector<int>>& viajes, int i) {
    int descanso = 0;
    for (int d = 0; d < viajes[i].size(); ++d) {
        if (viajes[i][d] == -1) {
            descanso++;
            if (descanso > 4) return true;
        }
        else {
            descanso = 0;
        }
    }
    return false;
}

double distancias(const vector<vector<int>>& viajes, int i, int rival, int k, int k2) {
    int ipost = partido_posterior(viajes, i, k);
    int rpost = partido_posterior(viajes, rival, k);
    int iant = partido_anterior(viajes, i, k);
    int rant = partido_anterior(viajes, rival, k);
    int ipost2 = partido_posterior(viajes, i, k2);
    int rpost2 = partido_posterior(viajes, rival, k2);
    int iant2 = partido_anterior(viajes, i, k2);
    int rant2 = partido_anterior(viajes, rival, k2);

    //cout << iant <<" " <<  ipost << " " << iant2 << " " << ipost2 << endl;
     //cout << rant << " " << rpost << " " << rant2 << " " << rpost2 << endl;

    double distancia_inicial = 0;
    double distancia_hipotetica = 0;

    if (k < k2) {
        if (ipost <= k2) {
            if (iant >= 0 && ipost < viajes[0].size()) {
                distancia_inicial += distanciasNBA[viajes[i][iant]][viajes[i][k]] + distanciasNBA[viajes[i][k]][viajes[i][ipost]];
                distancia_hipotetica += distanciasNBA[viajes[i][iant]][viajes[i][ipost]];
            }
            else if (iant < 0) {
                distancia_inicial += distanciasNBA[i][viajes[i][k]] + distanciasNBA[viajes[i][k]][viajes[i][ipost]];
                distancia_hipotetica += distanciasNBA[i][viajes[i][ipost]];
            }
            else if (ipost >= viajes[0].size()) {
                distancia_inicial += distanciasNBA[viajes[i][iant]][viajes[i][k]] + distanciasNBA[viajes[i][k]][i];
                distancia_hipotetica += distanciasNBA[viajes[i][iant]][i];
            }

            if (iant2 >= 0 && ipost2 < viajes[0].size()) {
                distancia_hipotetica += distanciasNBA[viajes[i][iant2]][viajes[i][k]] + distanciasNBA[viajes[i][k]][viajes[i][ipost2]];
                distancia_inicial += distanciasNBA[viajes[i][iant2]][viajes[i][ipost2]];
            }
            else if (iant2 < 0) {
                distancia_hipotetica += distanciasNBA[i][viajes[i][k]] + distanciasNBA[viajes[i][k]][viajes[i][ipost2]];
                distancia_inicial += distanciasNBA[i][viajes[i][ipost2]];
            }
            else if (ipost2 >= viajes[0].size()) {
                distancia_hipotetica += distanciasNBA[viajes[i][iant2]][viajes[i][k]] + distanciasNBA[viajes[i][k]][i];
                distancia_inicial += distanciasNBA[viajes[i][iant2]][i];
            }
        }

        if (rpost <= k2) {
            if (rant >= 0 && rpost < viajes[0].size()) {
                distancia_inicial += distanciasNBA[viajes[rival][rant]][viajes[rival][k]] + distanciasNBA[viajes[rival][k]][viajes[rival][rpost]];
                distancia_hipotetica += distanciasNBA[viajes[rival][rant]][viajes[rival][rpost]];
            }
            else if (rant < 0) {
                distancia_inicial += distanciasNBA[rival][viajes[rival][k]] + distanciasNBA[viajes[rival][k]][viajes[rival][rpost]];
                distancia_hipotetica += distanciasNBA[rival][viajes[rival][rpost]];
            }
            else if (rpost >= viajes[0].size()) {
                distancia_inicial += distanciasNBA[viajes[rival][rant]][viajes[rival][k]] + distanciasNBA[viajes[rival][k]][rival];
                distancia_hipotetica += distanciasNBA[viajes[rival][rant]][rival];
            }

            if (rant2 >= 0 && rpost2 < viajes[0].size()) {
                distancia_hipotetica += distanciasNBA[viajes[rival][rant2]][viajes[rival][k]] + distanciasNBA[viajes[rival][k]][viajes[rival][rpost2]];
                distancia_inicial += distanciasNBA[viajes[rival][rant2]][viajes[rival][rpost2]];
            }
            else if (rant2 < 0) {
                distancia_hipotetica += distanciasNBA[rival][viajes[rival][k]] + distanciasNBA[viajes[rival][k]][viajes[rival][rpost2]];
                distancia_inicial += distanciasNBA[rival][viajes[rival][rpost2]];
            }
            else if (rpost2 >= viajes[0].size()) {
                distancia_hipotetica += distanciasNBA[viajes[rival][rant2]][viajes[rival][k]] + distanciasNBA[viajes[rival][k]][rival];
                distancia_inicial += distanciasNBA[viajes[rival][rant2]][rival];
            }
        }
    }

    if (k > k2) {
        if (iant >= k2) {
            if (iant >= 0 && ipost < viajes[0].size()) {
                distancia_inicial += distanciasNBA[viajes[i][iant]][viajes[i][k]] + distanciasNBA[viajes[i][k]][viajes[i][ipost]];
                distancia_hipotetica += distanciasNBA[viajes[i][iant]][viajes[i][ipost]];
            }
            else if (iant < 0) {
                distancia_inicial += distanciasNBA[i][viajes[i][k]] + distanciasNBA[viajes[i][k]][viajes[i][ipost]];
                distancia_hipotetica += distanciasNBA[i][viajes[i][ipost]];
            }
            else if (ipost >= viajes[0].size()) {
                distancia_inicial += distanciasNBA[viajes[i][iant]][viajes[i][k]] + distanciasNBA[viajes[i][k]][i];
                distancia_hipotetica += distanciasNBA[viajes[i][iant]][i];
            }

            if (iant2 >= 0 && ipost2 < viajes[0].size()) {
                distancia_hipotetica += distanciasNBA[viajes[i][iant2]][viajes[i][k]] + distanciasNBA[viajes[i][k]][viajes[i][ipost2]];
                distancia_inicial += distanciasNBA[viajes[i][iant2]][viajes[i][ipost2]];
            }
            else if (iant2 < 0) {
                distancia_hipotetica += distanciasNBA[i][viajes[i][k]] + distanciasNBA[viajes[i][k]][viajes[i][ipost2]];
                distancia_inicial += distanciasNBA[i][viajes[i][ipost2]];
            }
            else if (ipost2 >= viajes[0].size()) {
                distancia_hipotetica += distanciasNBA[viajes[i][iant2]][viajes[i][k]] + distanciasNBA[viajes[i][k]][i];
                distancia_inicial += distanciasNBA[viajes[i][iant2]][i];
            }
        }

        if (rant >= k2) {
            if (rant >= 0 && rpost < viajes[0].size()) {
                distancia_inicial += distanciasNBA[viajes[rival][rant]][viajes[rival][k]] + distanciasNBA[viajes[rival][k]][viajes[rival][rpost]];
                distancia_hipotetica += distanciasNBA[viajes[rival][rant]][viajes[rival][rpost]];
            }
            else if (rant < 0) {
                distancia_inicial += distanciasNBA[rival][viajes[rival][k]] + distanciasNBA[viajes[rival][k]][viajes[rival][rpost]];
                distancia_hipotetica += distanciasNBA[rival][viajes[rival][rpost]];
            }
            else if (rpost >= viajes[0].size()) {
                distancia_inicial += distanciasNBA[viajes[rival][rant]][viajes[rival][k]] + distanciasNBA[viajes[rival][k]][rival];
                distancia_hipotetica += distanciasNBA[viajes[rival][rant]][rival];
            }

            if (rant2 >= 0 && rpost2 < viajes[0].size()) {
                distancia_hipotetica += distanciasNBA[viajes[rival][rant2]][viajes[rival][k]] + distanciasNBA[viajes[rival][k]][viajes[rival][rpost2]];
                distancia_inicial += distanciasNBA[viajes[rival][rant2]][viajes[rival][rpost2]];
            }
            else if (rant2 < 0) {
                distancia_hipotetica += distanciasNBA[rival][viajes[rival][k]] + distanciasNBA[viajes[rival][k]][viajes[rival][rpost2]];
                distancia_inicial += distanciasNBA[rival][viajes[rival][rpost2]];
            }
            else if (rpost2 >= viajes[0].size()) {
                distancia_hipotetica += distanciasNBA[viajes[rival][rant2]][viajes[rival][k]] + distanciasNBA[viajes[rival][k]][rival];
                distancia_inicial += distanciasNBA[viajes[rival][rant2]][rival];
            }
        }
    }





    return distancia_hipotetica - distancia_inicial;

}

void actualiza_balance(vector<vector<int>>& partidos_dia, vector<int>& minimo_dia, vector<int>& maximo_dia, int i, int jprev, int jnueva) {
    if (jprev < jnueva) {
        for (int k = jprev; k < jnueva; k++) {
            partidos_dia[i][k]--;
            if (partidos_dia[i][k] < minimo_dia[k]) {
                minimo_dia[k] = partidos_dia[i][k];
            }
        }
    }
    else {
        for (int k = jprev - 1; k >= jnueva; k--) {
            partidos_dia[i][k]++;
            if (partidos_dia[i][k] > maximo_dia[k]) {
                maximo_dia[k] = partidos_dia[i][k];
            }
        }
    }
}

bool comprueba_balance(const vector<int>& minimo_dia, const vector<int>& maximo_dia) {
    for (int k = 0; k < minimo_dia.size(); k++) {
        if (maximo_dia[k] > minimo_dia[k] + 4) {
            return false;
        }
    }

    return true;
}

bool comprueba_balance_lyv(const vector<vector<int>>& viajes, int i) {
    int local = 0;
    int visitante = 0;
    for (int k = 0; k < viajes[i].size(); k++) {
        if (viajes[i][k] != -1) {
            if (viajes[i][k] == i) {
                local++;
            }
            else {
                visitante++;
            }
        }

        if (abs(visitante - local) > 10) {
            return false;
        }
    }

    return true;
}


void VNS(vector<vector<int>>& viajes, vector<vector<int>>& rivales,  double& distancia, int max_iter, int limite_sin_mej, int k1, int k2, int k3, vector<int>& minimo_dia, vector<int>& maximo_dia, vector<vector<int>>& partidos_dia) {
    double distancia_mejor = distancia;
    vector<vector<int>> mejor_calendario;
    vector<int> max_dia_mejor = maximo_dia;
    vector<int> min_dia_mejor = minimo_dia;
    vector<vector<int>> partidos_dia_mejor = partidos_dia;
    vector<vector<int>> rivales_mejor = rivales;

    mejor_calendario = copiar_calendario(viajes);

    int iter_sin_mej = 0;

    for (int iter = 0; iter < max_iter && iter_sin_mej < limite_sin_mej; iter++) {

        int k = 1;
        while (k <= 3) {
            viajes = copiar_calendario(mejor_calendario);
            distancia = distancia_mejor;
            maximo_dia = max_dia_mejor;
            minimo_dia = min_dia_mejor;
            partidos_dia = partidos_dia_mejor;
            rivales = rivales_mejor;

            int cambios = (k == 1) ? k1 : (k == 2) ? k2 : k3;
            for (int i = 0; i < cambios; i++) {
                while (true) {
                    int equipo = rand() % N;
                    int k1 = rand() % TOTAL_JORNADAS;
                    int k2 = rand() % TOTAL_JORNADAS;

                    while (viajes[equipo][k1] == -1) {
                        k1 = rand() % TOTAL_JORNADAS;
                    }

                    while (viajes[equipo][k2] != -1 || viajes[rivales[equipo][k1]][k2] != -1) {
                        k2 = rand() % TOTAL_JORNADAS;
                    }
                    int local = viajes[equipo][k1];
                    int visitante = rivales[local][k1];


                    double dist = distancias(viajes, local, visitante, k1, k2);


                    actualiza_moviendo(viajes, rivales, k2, k1, local, visitante);
                    vector<int> old_min = minimo_dia;
                    vector<int> old_max = maximo_dia;
                    actualiza_balance(partidos_dia, minimo_dia, maximo_dia, local, k1, k2);
                    actualiza_balance(partidos_dia, minimo_dia, maximo_dia, visitante, k1, k2);


                    if (!tieneDescansoExcesivo(viajes, local) && !tieneDescansoExcesivo(viajes, visitante) && !tieneTresPartidosSeguidos(viajes, local) && !tieneTresPartidosSeguidos(viajes, visitante) && comprueba_balance(minimo_dia, maximo_dia) && comprueba_balance_lyv(viajes, local) && comprueba_balance_lyv(viajes, visitante)) {
                        distancia += dist;
                        break;
                    }
                    else {
                        //cout << "aqui" << endl;
                        actualiza_moviendo(viajes, rivales, k1, k2, local, visitante);
                        actualiza_balance(partidos_dia, minimo_dia, maximo_dia, local, k2, k1);
                        actualiza_balance(partidos_dia, minimo_dia, maximo_dia, visitante, k2, k1);
                        minimo_dia = old_min;
                        maximo_dia = old_max;
                        continue;
                    }
                }

            }

            bool mejorado = true;

            while (mejorado) {
                mejorado = false;
                double dist_ini = distancia;
                for (int k = 0; k < viajes[0].size(); k++) {

                    for (int i = 0; i < N; i++) {
                        bool mejorado2 = false;
                        if (viajes[i][k] != -1) {                           //si juega partido en esa jornada
                            int local = viajes[i][k];
                            int visitante = rivales[local][k];
                            double distancia_inicial = 0;
                            double distancia_hipotetica = 0;


                            //distancias_iniciales(viajes, local, visitante, k, distancia_inicial, distancia_hipotetica, 163);
                            for (int k2 = 0; k2 < viajes[0].size(); k2++) {     //recorremos todas las jornadas posibles. hay que comprobar que esos mismos equipos no jueguen en esa jornada, y en caso de que no haya ninguna opcion, crear una nueva
                                if (k2 != k) {
                                    if (rivales[local][k2] == -1 && rivales[visitante][k2] == -1) {
                                        //cout << "hola" << endl;
                                        double dist = distancias(viajes, local, visitante, k, k2);
                                        //cout << "adios" << endl;
                                        if (dist < 0) {
                                            actualiza_moviendo(viajes, rivales, k2, k, local, visitante);
                                            vector<int> old_min = minimo_dia;
                                            vector<int> old_max = maximo_dia;
                                            actualiza_balance(partidos_dia, minimo_dia, maximo_dia, local, k, k2);
                                            actualiza_balance(partidos_dia, minimo_dia, maximo_dia, visitante, k, k2);
                                            if (!tieneDescansoExcesivo(viajes, local) && !tieneDescansoExcesivo(viajes, visitante) && !tieneTresPartidosSeguidos(viajes, local) && !tieneTresPartidosSeguidos(viajes, visitante) && comprueba_balance(minimo_dia, maximo_dia) && comprueba_balance_lyv(viajes, local) && comprueba_balance_lyv(viajes, visitante)) {
                                                distancia += dist;
                                                mejorado2 = true;
                                                mejorado = true;
                                                break;
                                            }
                                            else {
                                                //cout << "aqui" << endl;
                                                actualiza_moviendo(viajes, rivales, k, k2, local, visitante);
                                                actualiza_balance(partidos_dia, minimo_dia, maximo_dia, local, k2, k);
                                                actualiza_balance(partidos_dia, minimo_dia, maximo_dia, visitante, k2, k);
                                                minimo_dia = old_min;
                                                maximo_dia = old_max;
                                            }
                                        }
                                    }

                                }

                            }
                        }

                    }
                }
                if (((dist_ini - distancia) / dist_ini) * 100 < 5) {
                    break;
                }

            }

            if (distancia < distancia_mejor) {
                distancia_mejor = distancia;
                cout << distancia_mejor << endl;
                mejor_calendario = copiar_calendario(viajes);
                max_dia_mejor = maximo_dia;
                min_dia_mejor = minimo_dia;
                partidos_dia_mejor = partidos_dia;
                rivales_mejor = rivales;
                iter_sin_mej = 0;
                k = 1;
            }
            else {
                k++;
            }
        }
        iter_sin_mej++;
        cout << iter << endl;

    }
    viajes = copiar_calendario(mejor_calendario);
    distancia = distancia_mejor;
}

int main() {

    ifstream archivo("calendario_balanceado_lyv.txt"); // Abre el archivo en modo lectura

    if (!archivo) { // Verifica si el archivo se abri� correctamente
        cerr << "Error al abrir el archivo" << std::endl;
        return 1;
    }

    vector<vector<int>> viajes;
    for (int i = 0; i < N; i++) {
        vector<int> recorrido;
        int n;
        bool ok = true;
        int count = 0;
        for (int k = 0; k < 82; k++) {
            archivo >> n;
            recorrido.push_back(n);
            count++;
            if (count == 2 && ok) {
                recorrido.push_back(-1);
                ok = !ok;
                count = 0;
            }
            else if (count == 2 && !ok) {
                recorrido.push_back(-1);
                recorrido.push_back(-1);
                recorrido.push_back(-1);

                ok = !ok;
                count = 0;
            }
        }
        viajes.push_back(recorrido);
    }

    archivo.close(); // Cierra el archivo


    vector<vector<int>> rivales(viajes.size(), vector<int>(viajes[0].size(), 0));
    for (int i = 0; i < viajes.size(); i++) {
        for (int k = 0; k < viajes[0].size(); k++) {
            if (viajes[i][k] == i) {
                int rival = buscaRival(k, i, viajes);
                rivales[i][k] = rival;
            }
            else {
                rivales[i][k] = viajes[i][k];
            }
        }
    }

    int NUM_JORNADAS = 163;
    double distanciaTotal = 0;

    for (int i = 0; i < N; i++) {
        int k = 0;
        //int jornada = partido_posterior(viajes, i, k);
        int jornada = viajes[i][k];

        while (jornada == -1 && k < NUM_JORNADAS) {
            k++;
            jornada = viajes[i][k];
        }

        int prev = k;
        //cout << prev << viajes[i][prev] << endl;
        distanciaTotal += distanciasNBA[i][viajes[i][prev]];
        jornada = partido_posterior(viajes, i, prev);
        //cout << "aqui2" << endl;
        while (jornada < viajes[i].size()) {
            distanciaTotal += distanciasNBA[viajes[i][prev]][viajes[i][jornada]];
            prev = jornada;
            //cout << prev << " " << jornada << endl;
            jornada = partido_posterior(viajes, i, prev);
            //cout << jornada << endl;
        }
        //cout << "aqui" << endl;
        distanciaTotal += distanciasNBA[viajes[i][prev]][i];
        //cout << "Equipo " << i << ": " << distanciaTotal << endl;

    }

    cout << distanciaTotal << endl;

    vector<vector<int>> partidos_dia(viajes.size(), vector<int>(viajes[0].size(), 0));

    for (int i = 0; i < viajes.size(); i++) {
        int cont = 0;
        for (int k = 0; k < viajes[0].size(); k++) {
            if (viajes[i][k] != -1) {
                cont++;
            }
            partidos_dia[i][k] = cont;
        }
    }


    vector<int> minimo_dia(viajes[0].size(), 0);
    for (int k = 0; k < viajes[0].size(); k++) {
        int minimo = INT_MAX;
        for (int i = 0; i < viajes.size(); i++) {
            minimo = min(minimo, partidos_dia[i][k]);
        }
        minimo_dia[k] = minimo;
    }

    vector<int> maximo_dia(viajes[0].size(), 0);
    for (int k = 0; k < viajes[0].size(); k++) {
        int maximo = INT_MIN;
        for (int i = 0; i < viajes.size(); i++) {
            maximo = max(maximo, partidos_dia[i][k]);
        }
        maximo_dia[k] = maximo;
    }

    vector<vector<int>> calendario_inicial = copiar_calendario(viajes);
    double dist_inicial = distanciaTotal;

    vector<double> t = { 20 };
    //double t_inicial = 100;
    double t_minimo = 0.1;
    //vector<int> Ms = { 2, 3, 4, 5, 6, 8, 10 };
    //vector<double> alphas = { 0.90, 0.95, 0.99 };

    vector<int> Ms = { 2 };
    vector<double> alphas = { 0.90 };

    double dist_mejor = numeric_limits<double>::max();
    vector<vector<int>> mejor_calend;
    vector<int> max_dia_ini = maximo_dia;
    vector<int> min_dia_ini = minimo_dia;
    vector<vector<int>> partidos_dia_ini = partidos_dia;
    vector<vector<int>> rivales_ini = rivales;

    for (double t_inicial : t) {
        for (int M : Ms) {
            for (double alpha : alphas) {
                VNS(viajes, rivales, distanciaTotal, 50, 20, 2, 3, 5, minimo_dia, maximo_dia, partidos_dia);
                //archivo2 << "T = " << t_inicial << ", M = " << M << ", alpha = " << alpha << " -> Mejor Distancia: " << distancia << endl;
                cout << "T = " << t_inicial << ", M = " << M << ", alpha = " << alpha << " -> Mejor Distancia: " << distanciaTotal << endl;
                if (distanciaTotal < dist_mejor) {
                    dist_mejor = distanciaTotal;
                    mejor_calend = copiar_calendario(viajes);
                }
                //viajes = copiar_calendario(calendario_inicial);
                //distanciaTotal = dist_inicial;
                maximo_dia = max_dia_ini;
                minimo_dia = min_dia_ini;
                partidos_dia = partidos_dia_ini;
                rivales = rivales_ini;
            }
        }
    }



    //cout << distanciaTotal << endl;
    int count = 0;




    cout << distanciaTotal << endl;

    double distanciaTotal2 = 0;

    for (int i = 0; i < N; i++) {
        int k = 0;
        //int jornada = partido_posterior(viajes, i, k);
        int jornada = viajes[i][k];

        while (jornada == -1 && k < NUM_JORNADAS) {
            k++;
            jornada = viajes[i][k];
        }

        int prev = k;
        //cout << prev << viajes[i][prev] << endl;
        distanciaTotal2 += distanciasNBA[i][viajes[i][prev]];
        jornada = partido_posterior(viajes, i, prev);
        //cout << "aqui2" << endl;
        while (jornada < viajes[i].size()) {
            distanciaTotal2 += distanciasNBA[viajes[i][prev]][viajes[i][jornada]];
            prev = jornada;
            //cout << prev << " " << jornada << endl;
            jornada = partido_posterior(viajes, i, prev);
            //cout << jornada << endl;
        }
        //cout << "aqui" << endl;
        distanciaTotal2 += distanciasNBA[viajes[i][prev]][i];
        //cout << "Equipo " << i << ": " << distanciaTotal << endl;

    }

    cout << "Distancia Final: " << distanciaTotal2 << endl;



    for (int i = 0; i < N; i++) {
        int max_sin_jugar = 0;
        cout << "Equipo: " << i << endl;
        int counttotal = 0;
        int countlocal = 0;
        int countsinjug = 0;
        for (int k = 0; k < NUM_JORNADAS; k++) {
            if (viajes[i][k] != -1) {
                counttotal++;
                countsinjug = 0;
            }
            if (viajes[i][k] == i) {
                countlocal++;
            }
            if (viajes[i][k] == -1) {
                countsinjug++;
                if (countsinjug > max_sin_jugar) {
                    max_sin_jugar = countsinjug;
                }
            }
            cout << viajes[i][k] << " ";
        }

        cout << "Partidos totales: " << counttotal << "------ Partidos como local: " << countlocal << "-------" << "M�ximo de d�as de descanso: " << max_sin_jugar << endl;
        cout << endl;
    }

    int max_dif = 0;
    int maximo_part = 0;
    for (int k = 0; k < viajes[0].size(); k++) {
        int countsinjug = 0;
        for (int i = 0; i < N; i++) {
            if (viajes[i][k] != -1) {
                equipos[i].partidos_jug++;
                maximo_part = max(maximo_part, equipos[i].partidos_jug);
            }
            max_dif = max(max_dif, maximo_part - equipos[i].partidos_jug);
            cout << max_dif << " k: " << k << " i: " << i << " partidos jugados: " << equipos[i].partidos_jug << endl;
        }
    }

    cout << "Maxima diferencia de partidos " << max_dif << endl;
    cout << endl;

    for (int i = 0; i < N; i++) {
        if (!tieneDescansoExcesivo(viajes, i) && !tieneTresPartidosSeguidos(viajes, i)) {
            cout << i << endl;
        }
    }

    vector<vector<int>> viajes_final;
    for (int i = 0; i < N; i++) {
        vector<int> v;
        for (int k = 0; k < viajes[i].size(); k++) {
            if (viajes[i][k] != -1) {
                v.push_back(viajes[i][k]);
            }
        }
        viajes_final.push_back(v);
    }

    double distancia_final = 0;
    for (int i = 0; i < N; i++) {
        distancia_final += distanciasNBA[i][viajes_final[i][0]] + distanciasNBA[i][viajes_final[i][81]];
        for (int k = 0; k < viajes_final[0].size() - 1; k++) {
            distancia_final += distanciasNBA[viajes_final[i][k]][viajes_final[i][k + 1]];
        }
    }

    cout << distancia_final << endl;

    for (int i = 0; i < N; i++) {
        if (comprueba_balance_lyv(viajes, i))
            cout << i << endl;
    }



    return 0;
}



