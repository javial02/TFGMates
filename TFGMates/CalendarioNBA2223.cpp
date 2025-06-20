#include <vector>
#include <string>
#include <iostream>
#include <fstream>
#include <cstdlib>
#include <ctime>
#include <cmath>
#include <algorithm>
#include <unordered_map>
#include "gurobi_c++.h"
#include "datosNBA.h"


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

void actualiza_insertando(vector<vector<int>>& viajes, vector<vector<int>>& rivales, int jornada, int j_prev, int local, int visitante) {
    rivales[local][j_prev] = -1;
    rivales[visitante][j_prev] = -1;
    viajes[local][j_prev] = -1;
    viajes[visitante][j_prev] = -1;

    for (int i = 0; i < viajes.size(); i++) {
        rivales[i].push_back(-1);
        viajes[i].push_back(-1);
    }
    for (int i = 0; i < viajes.size(); i++) {
        for (int k = viajes[i].size() - 1; k > jornada + 1; k--) {
            rivales[i][k] = rivales[i][k - 1];
            viajes[i][k] = viajes[i][k - 1];
        }
    }
    for (int i = 0; i < N; i++) {
        rivales[i][jornada + 1] = -1;
        viajes[i][jornada + 1] = -1;
    }


    rivales[local][jornada + 1] = visitante;
    rivales[visitante][jornada + 1] = local;


    viajes[local][jornada + 1] = local;
    viajes[visitante][jornada + 1] = local;



}


bool insertaPartido(const vector<vector<int>>& viajes, int i, int rival, int k, int jornada, double distancia_inicial_jornada, double distancia_hipotetica_jornada, double& distancia) {

    int ipost = partido_posterior(viajes, i, k);
    int rpost = partido_posterior(viajes, rival, k);
    int iant = partido_anterior(viajes, i, k);
    int rant = partido_anterior(viajes, rival, k);

    //cout << iant << " " << rant << " " << ipost << " " << rpost << endl;

    if (ipost < viajes[0].size() && iant >= 0) {
        //cout << "aqui1" << endl;
        distancia_inicial_jornada += distanciasNBA[viajes[i][iant]][viajes[i][ipost]];
        distancia_hipotetica_jornada += distanciasNBA[viajes[i][iant]][viajes[i][jornada]] + distanciasNBA[viajes[i][jornada]][viajes[i][ipost]];
    }
    else if (ipost >= viajes[0].size()) {
        distancia_inicial_jornada += distanciasNBA[viajes[i][iant]][i];
        distancia_hipotetica_jornada += distanciasNBA[viajes[i][iant]][viajes[i][jornada]] + distanciasNBA[viajes[i][jornada]][i];
    }
    else if (iant < 0) {
        distancia_inicial_jornada += distanciasNBA[i][viajes[i][ipost]];
        distancia_hipotetica_jornada += distanciasNBA[i][viajes[i][jornada]] + distanciasNBA[viajes[i][jornada]][viajes[i][ipost]];
    }


    if (rpost < viajes[0].size() && rant >= 0) {
        distancia_inicial_jornada += distanciasNBA[viajes[rival][rant]][viajes[rival][rpost]];
        distancia_hipotetica_jornada += distanciasNBA[viajes[rival][rant]][viajes[rival][jornada]] + distanciasNBA[viajes[rival][jornada]][viajes[rival][rpost]];
    }
    else if (rpost >= viajes[0].size()) {
        distancia_inicial_jornada += distanciasNBA[viajes[rival][rant]][rival];
        distancia_hipotetica_jornada += distanciasNBA[viajes[rival][rant]][viajes[rival][jornada]] + distanciasNBA[viajes[rival][jornada]][rival];
    }
    else if (rant < 0) {
        distancia_inicial_jornada += distanciasNBA[rival][viajes[rival][rpost]];
        distancia_hipotetica_jornada += distanciasNBA[rival][viajes[rival][jornada]] + distanciasNBA[viajes[rival][jornada]][viajes[rival][rpost]];
    }



    if (distancia_hipotetica_jornada < distancia_inicial_jornada) {
        cout << distancia_hipotetica_jornada - distancia_inicial_jornada << endl;
        distancia += distancia_hipotetica_jornada - distancia_inicial_jornada;
        return true;
    }

    return false;


}

bool hazHueco(const vector<vector<int>>& viajes, int i, int rival, int k, int jornada, double distancia_inicial_jornada, double distancia_hipotetica_jornada, double& distancia) {
    //cout << "aqui" << endl;
    int ipost = partido_posterior(viajes, i, k);
    int rpost = partido_posterior(viajes, rival, k);
    //cout << i << " " << rival << " " << k << " " << jornada << " " << ipost << " " << rpost <<  endl;

    if (ipost < viajes[0].size()) {
        distancia_inicial_jornada += distanciasNBA[viajes[i][k]][viajes[i][ipost]];
        distancia_hipotetica_jornada += distanciasNBA[viajes[i][k]][viajes[i][jornada]] + distanciasNBA[viajes[i][jornada]][viajes[i][ipost]];
    }
    else if (ipost >= viajes[0].size()) {
        distancia_inicial_jornada += distanciasNBA[viajes[i][k]][i];
        distancia_hipotetica_jornada += distanciasNBA[viajes[i][k]][viajes[i][jornada]] + distanciasNBA[viajes[i][jornada]][i];
    }

    if (rpost < viajes[0].size()) {
        distancia_inicial_jornada += distanciasNBA[viajes[rival][k]][viajes[rival][rpost]];
        distancia_hipotetica_jornada += distanciasNBA[viajes[rival][k]][viajes[rival][jornada]] + distanciasNBA[viajes[rival][jornada]][viajes[rival][rpost]];

    }
    else if (rpost >= viajes[0].size()) {
        distancia_inicial_jornada += distanciasNBA[viajes[rival][k]][rival];
        distancia_hipotetica_jornada += distanciasNBA[viajes[rival][k]][viajes[rival][jornada]] + distanciasNBA[viajes[rival][jornada]][rival];

    }

    if (distancia_hipotetica_jornada < distancia_inicial_jornada) {
        distancia += distancia_hipotetica_jornada - distancia_inicial_jornada;
        return true;
    }

    return false;
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
        for (int k = 0; k < TOTAL_JORNADAS - 1; k++) {
            distancia_final += distanciasNBA[viajes_final[i][k]][viajes_final[i][k + 1]];
        }
    }

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

int comprueba_si_partido(const vector<vector<int>>& viajes, int jornada, int equipo) {
    bool izda = false;
    bool dcha = false;
    int count = 1;
    for (int k = jornada + 1; k < viajes[equipo].size(); k++) {
        if (viajes[equipo][k] == -1) {
            count++;
        }
        else {
            break;
        }
    }

    for (int k = jornada - 1; k > 0; k--) {
        if (viajes[equipo][k] == -1) {
            count++;
        }
        else {
            break;
        }
    }

    return count;
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
            if (descanso > 5) return true;
        }
        else {
            descanso = 0;
        }
    }
    return false;
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

        if (abs(visitante - local) > 9) {
            return false;
        }
    }

    return true;
}

int main() {

    ifstream archivo("calendario22_23.txt"); // Abre el archivo en modo lectura

    if (!archivo) { // Verifica si el archivo se abrió correctamente
        cerr << "Error al abrir el archivo" << std::endl;
        return 1;
    }

    vector<vector<int>> viajes;
    
    for (int i = 0; i < N; i++) {
        vector<int> rec;
        int n;
        int k = 0;
        while (k < 82) {
            archivo >> n;
            if (n != -1) {
                rec.push_back(n);
                k++;
            }
        }
        viajes.push_back(rec);
    }

    archivo.close(); // Cierra el archivo

    //-----------------Distancia que genera el modelo--------------------------
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
        distancia += distanciasNBA[i][viajes[i][0]] + distanciasNBA[i][viajes[i][81]];
        dist_eq[i] += distanciasNBA[i][viajes[i][0]] + distanciasNBA[i][viajes[i][81]];
    }

    cout << "Distancia inicial dada por el modelo: " << distancia << endl;

    double maximo = -numeric_limits<double>::max();
    double minimo = numeric_limits<double>::max();
    int idxmin = -1;
    int idxmax = -1;

    for (int i = 0; i < N; i++) {
        cout << equipos[i].nombre << ": " << dist_eq[i] << " millas recorridas" << endl;
        if (dist_eq[i] > maximo) {
            maximo = dist_eq[i];
            idxmax = i;
        }
        if (dist_eq[i] < minimo) {
            minimo = dist_eq[i];
            idxmin = i;
        }
    }

    cout << "Equipo con menor distancia recorrida: " << equipos[idxmin].nombre << " con " << dist_eq[idxmin] << " millas" << endl;
    cout << "Equipo con mayor distancia recorrida: " << equipos[idxmax].nombre << " con " << dist_eq[idxmax] << " millas" << endl;


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

    cout << "Media de millas recorridas Conferencia Este: " << disteste / 15 << " Millas Totales: " << disteste << endl;
    cout << "Media de millas recorridas Conferencia Oeste: " << distoeste / 15 << " Millas Totales: " << distoeste << endl;



    int acum = 0;
    for (int i = 0; i < N; i++) {
        int maximo_diff_lyc = INT_MIN;
        int contlocal = 0;
        int contvisit = 0;
        for (int k = 0; k < viajes[i].size(); k++) {
            if (viajes[i][k] != -1) {
                if (viajes[i][k] == i) {
                    contlocal++;
                }
                else {
                    contvisit++;
                }
            }
            //cout << abs(contlocal - contvisit) << endl;
            maximo_diff_lyc = max(maximo_diff_lyc, abs(contlocal - contvisit));
            
        }
        cout << "Equipo: " << i << " maxima dif: " << maximo_diff_lyc << endl;
        acum += maximo_diff_lyc;
    }

    for (int i = 0; i < N; i++) {
        if(comprueba_balance_lyv(viajes, i))
            cout << i << endl;
    }

    cout << "Dif media: " << acum / N << endl;


    return 0;
}



