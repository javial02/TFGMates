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
    for (int k = jornada -1; k >= 0; k--) {
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
    else if(iant < 0){
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

bool hazHueco(const vector<vector<int>>& viajes, int i, int rival, int k, int jornada, double distancia_inicial_jornada, double distancia_hipotetica_jornada, double & distancia) {
    //cout << "aqui" << endl;
    int ipost = partido_posterior(viajes, i, k);
    int rpost = partido_posterior(viajes, rival, k);
    //cout << i << " " << rival << " " << k << " " << jornada << " " << ipost << " " << rpost <<  endl;

    if (ipost < viajes[0].size()) {
        distancia_inicial_jornada += distanciasNBA[viajes[i][k]][viajes[i][ipost]];
        distancia_hipotetica_jornada += distanciasNBA[viajes[i][k]][viajes[i][jornada]] + distanciasNBA[viajes[i][jornada]][viajes[i][ipost]];
    }
    else if(ipost >= viajes[0].size()){
        distancia_inicial_jornada += distanciasNBA[viajes[i][k]][i];
        distancia_hipotetica_jornada += distanciasNBA[viajes[i][k]][viajes[i][jornada]] + distanciasNBA[viajes[i][jornada]][i];
    }
    
    if (rpost < viajes[0].size()) {
        distancia_inicial_jornada += distanciasNBA[viajes[rival][k]][viajes[rival][rpost]];
        distancia_hipotetica_jornada += distanciasNBA[viajes[rival][k]][viajes[rival][jornada]] + distanciasNBA[viajes[rival][jornada]][viajes[rival][rpost]];

    }
    else if(rpost >= viajes[0].size()){
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


bool hayPartido(const vector<vector<int>>& viajes, int k2, int k, int local, int visitante){

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


int main() {

    ifstream archivo("mejorado_calen.txt"); // Abre el archivo en modo lectura

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

    /*for (int i = 0; i < N; i++) {
        cout << "Equipo: " << i << endl;
        int counttotal = 0;
        int countlocal = 0;
        for (int k = 0; k < viajes[i].size(); k++) {
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
    }*/

    
    
    
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

    
    int NUM_JORNADAS = 82;
    
    for (int k = 0; k < NUM_JORNADAS; k++) {
        for (int i = 0; i < N; i++) {
            bool mejorado = false;
            if (viajes[i][k] != -1) {                           //si juega partido en esa jornada
                
                int local = viajes[i][k];
                int visitante = rivales[local][k];
                double distancia_inicial = 0;
                double distancia_hipotetica = 0;
                
                distancias_iniciales(viajes, local, visitante, k, distancia_inicial, distancia_hipotetica, NUM_JORNADAS);
                for (int k2 = 0; k2 < NUM_JORNADAS; k2++) {     //recorremos todas las jornadas posibles. hay que comprobar que esos mismos equipos no jueguen en esa jornada, y en caso de que no haya ninguna opcion, crear una nueva
                    
                    if (k2 != k) {
                        if (rivales[local][k2] == -1 && rivales[visitante][k2] == -1 && hayPartido(viajes, k2, k, local, visitante))  {
                            double distancia_antigua = nuevaDist(viajes, local, visitante);
                            actualiza_moviendo(viajes, rivales, k2, k, local, visitante);
                            double distancia_nueva = nuevaDist(viajes,local,visitante);
                            if (distancia_nueva < distancia_antigua) {
                                distancia += distancia_nueva - distancia_antigua;
                                mejorado = true;
                                break;
                            }
                            else {
                                actualiza_moviendo(viajes, rivales, k, k2, local, visitante);
                            }
                            
                        }
                        if (rivales[local][k2] != -1 && rivales[visitante][k2] != -1 && NUM_JORNADAS < 163) {
                            if (hazHueco(viajes, local, visitante, k2, k, distancia_inicial, distancia_hipotetica, distancia)) {
                                cout << "------------------------------------------------" << endl;
                                muestraDist(viajes);
                                cout << "----insertando---" << endl;

                                NUM_JORNADAS++;
                                mejorado = true;
                                actualiza_insertando(viajes, rivales, k2, k, local, visitante);
                                muestraDist(viajes);
                                break;
                            }
                            
                        }

                    }
                    
                }
            }
            if (mejorado) {
                cout << "Distancia: " << distancia << endl;
                //cout << "--------------------------------------------------------" << endl;
            }
            
        }
    }

   double distanciaTotal = 0;

    for (int i = 0; i < N; i++) {
        int k = 0;
        int jornada = partido_posterior(viajes, i, k);
        while (jornada == -1 && k < NUM_JORNADAS) {
            k++;
            jornada = partido_posterior(viajes, i, k);
        }
        
        int prev = jornada;
        //cout << prev << endl;
        distanciaTotal += distanciasNBA[i][viajes[i][prev]];
        jornada = partido_posterior(viajes, i, prev);
        while (jornada < viajes[i].size() + 1) {
            distanciaTotal += distanciasNBA[viajes[i][prev]][viajes[i][jornada]];
            prev = jornada;
            //cout << prev << " " << jornada << endl;
            jornada = partido_posterior(viajes, i, prev);
            //cout << jornada << endl;
        }
       //cout << "aqui" << endl;
        distanciaTotal += distanciasNBA[viajes[i][prev]][i];

    }

    cout << distanciaTotal << endl;

    for (int i = 0; i < N; i++) {
        cout << "Equipo: " << i << endl;
        int counttotal = 0;
        int countlocal = 0;
        for (int k = 0; k < NUM_JORNADAS; k++) {
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

    vector<vector<int>> viajes_final;
    for (int i = 0; i < N; i++) {
        vector<int> v;
        for (int k = 0; k < NUM_JORNADAS; k++) {
            if (viajes[i][k] != -1) {
                v.push_back(viajes[i][k]);
            }
        }
        viajes_final.push_back(v);
        cout << v.size() << endl;
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

    ofstream archivo2("calendario_impreso_por_dias.txt");

    if (!archivo2) { // Verifica si el archivo se abrió correctamente
        cerr << "Error al abrir el archivo" << std::endl;
        return 1;
    }

    for (int k = 0; k < viajes[0].size(); k++) {
        archivo2 << "Día " << k + 1 << ":" << endl;
        for (int i = 0; i < N; i++) {
            if (viajes[i][k] == i) {
                int rival = calculaRival(viajes, k, i);
                archivo2 << equipos[i].nombre << " vs " << equipos[rival].nombre << endl;
            }
        }

        archivo2 << "----------------------" << endl;
    }


    archivo2.close(); // Cierra el archivo

    /*ofstream archivo2("resultados_insercion.txt"); // Abre el archivo en modo lectura

    if (!archivo2) { // Verifica si el archivo se abrió correctamente
        cerr << "Error al abrir el archivo" << std::endl;
        return 1;
    }


    vector<vector<int>> sol = muestraDist(viajes);
    for (int i = 0; i < N; i++) {
        for (int k = 0; k < TOTAL_JORNADAS; k++) {
            archivo2 << sol[i][k] << " ";
        }
        archivo2 << endl;
    }




    archivo2.close(); // Cierra el archivo
    */

    
    return 0;
}



