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



int main() {

    ifstream archivo("mejor_calendario_82.txt"); // Abre el archivo en modo lectura

    if (!archivo) { // Verifica si el archivo se abrió correctamente
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



    //temple_simulado(viajes, rivales, distanciaTotal, 100, 0.1, 4, 0.9);

    //cout << distanciaTotal << endl;
    bool mejorado = true;
    while (mejorado) {
        mejorado = false;
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
                                    if (!tieneDescansoExcesivo(viajes, local) && !tieneDescansoExcesivo(viajes, visitante) && !tieneTresPartidosSeguidos(viajes, local) && !tieneTresPartidosSeguidos(viajes, visitante)) {
                                        distanciaTotal += dist;
                                        //cout << "Distancia heur: " << distanciaTotal << "Intercambio: " << k << " - " << k2 << endl;
                                        double distanciaTotal2 = 0;

                                        /*for (int i = 0; i < N; i++) {
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

                                        }*/

                                        //cout << distanciaTotal2 << endl;
                                        mejorado2 = true;
                                        mejorado = true;
                                        break;
                                    }
                                    else {
                                        //cout << "aqui" << endl;
                                        actualiza_moviendo(viajes, rivales, k, k2, local, visitante);
                                    }
                                }
                            }

                        }

                    }
                }

            }
        }
    }



    cout << distanciaTotal << endl;

    imprimeCalendario(viajes, "Insercion_descansos.txt");


    return 0;
}



