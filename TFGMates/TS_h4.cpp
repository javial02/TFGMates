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
        int k = 0;
        int jornada = viajes[i][k];
        double distancia_eq = 0;

        while (jornada == -1 && k < TOTAL_JORNADAS) {
            k++;
            jornada = viajes[i][k];
        }

        int prev = k;

        distancia += distanciasNBA[i][viajes[i][prev]];
        distancia_eq += distanciasNBA[i][viajes[i][prev]];
        jornada = partido_posterior(viajes, i, prev);

        while (jornada < viajes[i].size()) {
            distancia += distanciasNBA[viajes[i][prev]][viajes[i][jornada]];
            distancia_eq += distanciasNBA[viajes[i][prev]][viajes[i][jornada]];
            prev = jornada;

            jornada = partido_posterior(viajes, i, prev);

        }

        distancia += distanciasNBA[viajes[i][prev]][i];
        distancia_eq += distanciasNBA[viajes[i][prev]][i];

        dist_eq.push_back(distancia_eq);

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


// Funci�n que calcula la posible mejora en t�rminos de distancias con respecto a una reubicaci�n hipot�tica de partido 
double distancias(const vector<vector<int>>& viajes, int i, int rival, int k, int k2) {
    int ipost = partido_posterior(viajes, i, k);
    int rpost = partido_posterior(viajes, rival, k);
    int iant = partido_anterior(viajes, i, k);
    int rant = partido_anterior(viajes, rival, k);
    int ipost2 = partido_posterior(viajes, i, k2);
    int rpost2 = partido_posterior(viajes, rival, k2);
    int iant2 = partido_anterior(viajes, i, k2);
    int rant2 = partido_anterior(viajes, rival, k2);


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


void temple_simulado(vector<vector<int>>& viajes, vector<vector<int>>& rivales, double& distancia, double t_inicial, double t_minima, int M, double alpha, vector<int>& minimo_dia, vector<int>& maximo_dia, vector<vector<int>> & partidos_dia) {
    double distancia_mejor = distancia;
    vector<vector<int>> mejor_calendario;

    mejor_calendario = copiar_calendario(viajes);

    double distancia_nueva = 0;
    double distancia_inicial;
    double distancia_final;
    while (t_inicial > t_minima) {
        int M_act = M;
        if (t_inicial < 60 && t_inicial > 25) {
            M_act = M / 2;
        }
        else if (t_inicial < 25) {
            M_act = 1;
        }
        for (int i = 0; i < M_act; i++) {
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


                        
                        for (int k2 = 0; k2 < viajes[0].size(); k2++) {     
                            if (k2 != k) {
                                if (rivales[local][k2] == -1 && rivales[visitante][k2] == -1) {
                                    
                                    double dist = distancias(viajes, local, visitante, k, k2);
                                    
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
            if (((dist_ini - distancia) / dist_ini) * 100 < 3) {
                break;
            }

        }


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

    ifstream archivo("calendario_balanceado_lyv.txt"); // Abre el archivo en modo lectura

    if (!archivo) { // Verifica si el archivo se abri� correctamente
        cerr << "Error al abrir el archivo" << std::endl;
        return 1;
    }


    // Lectura del calendario inicial
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


    // Creaci�n de la matriz de rivales
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
        int jornada = viajes[i][k];

        while (jornada == -1 && k < NUM_JORNADAS) {
            k++;
            jornada = viajes[i][k];
        }

        int prev = k;
        distanciaTotal += distanciasNBA[i][viajes[i][prev]];
        jornada = partido_posterior(viajes, i, prev);
        while (jornada < viajes[i].size()) {
            distanciaTotal += distanciasNBA[viajes[i][prev]][viajes[i][jornada]];
            prev = jornada;
            jornada = partido_posterior(viajes, i, prev);
        }
        distanciaTotal += distanciasNBA[viajes[i][prev]][i];
    }

    cout << distanciaTotal << endl;


    // Creaci�n de estructuras de datos para llevar el conteo de partidos

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

    ofstream archivo2("resultados_ts_h4.txt"); // Abre el archivo en modo lectura

    if (!archivo2) { // Verifica si el archivo se abri� correctamente
        cerr << "Error al abrir el archivo" << std::endl;
        return 1;
    }

    vector<vector<int>> calendario_inicial = copiar_calendario(viajes);
    double dist_inicial = distanciaTotal;

    vector<double> t = { 50, 100 };
    double t_minimo = 0.1;
    vector<int> Ms = { 2, 5, 8 };
    vector<double> alphas = { 0.90, 0.95};


    double dist_mejor = numeric_limits<double>::max();
    vector<vector<int>> mejor_calend;
    vector<int> max_dia_ini = maximo_dia;
    vector<int> min_dia_ini = minimo_dia;
    vector<vector<int>> partidos_dia_ini = partidos_dia;
    vector<vector<int>> rivales_ini = rivales;

    for (double t_inicial : t) {
        for (int M : Ms) {
            for (double alpha : alphas) {
                temple_simulado(viajes, rivales, distanciaTotal, t_inicial,t_minimo , M, alpha, minimo_dia, maximo_dia, partidos_dia);
                archivo2 << "T = " << t_inicial << ", M = " << M << ", alpha = " << alpha << " -> Mejor Distancia: " << distanciaTotal << endl;
                cout << "T = " << t_inicial << ", M = " << M << ", alpha = " << alpha << " -> Mejor Distancia: " << distanciaTotal << endl;
                if (distanciaTotal < dist_mejor) {
                    dist_mejor = distanciaTotal;
                    mejor_calend = copiar_calendario(viajes);
                }
                viajes = copiar_calendario(calendario_inicial);
                distanciaTotal = dist_inicial;
                maximo_dia = max_dia_ini;
                minimo_dia = min_dia_ini;
                partidos_dia = partidos_dia_ini;
                rivales = rivales_ini;
            }
        }
    }

    archivo2.close();

    cout << distanciaTotal << endl;
    

    imprimeCalendario(mejor_calend, "TS_h4.txt");



    return 0;
}



