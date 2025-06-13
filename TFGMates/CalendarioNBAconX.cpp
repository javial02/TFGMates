#include <vector>
#include <string>
#include <iostream>
#include <fstream>  // Biblioteca para manejar archivos
#include "gurobi_c++.h"

using namespace std;

const int N = 30;								//N�mero de franquicias en total
const int CONFERENCIAS = 2;						//N�mero de conferencias
const int DIVISIONES_POR_CONFERENCIA = 3;		//N�mero de divisiones por conferencia
const int EQUIPOS_POR_DIVISION = 5;				//N�mero de equipos por cada divisi�n
const int NUM_RIVALES_CONF_1 = 6;               //N�mero de rivales fuera de la divisi�n, en la misma conferencia, con los que se juegan 4 partidos
const int NUM_RIVALES_CONF_2 = 2;               //N�mero de rivales fuera de la divisi�n, en la misma conferencia, con los que se juegan 3 partidos (2c y 1f)
const int NUM_RIVALES_CONF_3 = 2;               //N�mero de rivales fuera de la divisi�n, en la misma conferencia, con los que se juegan 3 partidos (1c y 2f)
const int NUM_EQUIPOS_CONFERENCIA = 15;         //N�mero de equipos por conferencia
const int TOTAL_JORNADAS = 82;                  //N�mero de jornadas

struct InfoEquipo {
    int id;                                     //Numeraci�n de equipos
    string nombre;								//Nombre del equipo
    string conferencia;							//Conferencia a la que pertenece el equipo
    string division;							//Divisi�n a la que pertenece el equipo
    vector<int> rivales_division;			    //Lista de rivales de su misma divisi�n
    vector<int> rivales_conf1;				    //Lista de rivales de su misma conferencia pero distinta division con los que juega 4 partidos
    vector<int> rivales_conf3;				    //Lista de rivales de su misma conferencia pero distinta divisi�n con los que juega 3 partidos (1c y 2f)
    vector<int> rivales_conf2;				    //Lista de rivales de su misma conferencia pero distinta divisi�n con los que juega 3 partidos (2c y 1f)
    vector<int> rivales_interconf;			    //Lista de rivales que no son de su conferencia
};

struct InfoUltimosPartidos {
    int ultimo_rival;                                     //id del ultimo rival
    bool en_casa;                               //Booleano para guardar si el �ltimo partido ha sido en casa o no
};

vector<InfoEquipo> equipos = {
    {0,"Boston Celtics", "Este", "Atlantico", {1, 2, 3, 4}, {8, 7, 5, 14, 10, 12}, {9, 13}, {6, 11} , {15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29} },
    {1,"Brooklyn Nets", "Este", "Atlantico", {0, 2, 3, 4}, {13, 6, 9, 10, 5, 11}, {8, 14}, {7, 12} , {15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29}  },
    {2,"Philadelphia 76ers", "Este", "Atlantico", {0, 1, 3, 4}, {6, 9, 5, 13, 12, 11}, {7, 10}, {8, 14} , {15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29}  },
    {3,"New York Knicks", "Este", "Atlantico", {0, 1, 2, 4}, {7, 8, 11, 13, 6, 14}, {5, 12}, {10, 9} , {15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29}  },
    {4,"Toronto Raptors", "Este", "Atlantico", {0, 1, 2, 3}, {7, 8, 14, 12, 10, 9}, {6, 11}, {5, 13} , {15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29}  },
    {5,"Chicago Bulls", "Este", "Central", {6, 7, 8, 9}, {0, 2, 1, 13, 11, 10}, {4, 14}, {3, 12} , {15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29} },
    {6,"Indiana Pacers", "Este", "Central", {5, 7, 8, 9}, {2, 3, 1, 14, 12, 11}, {0, 10}, {4, 13} , {15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29}  },
    {7,"Cleveland Cavaliers", "Este", "Central", {5, 6, 8, 9}, {4, 0, 3, 10, 12, 14}, {1, 13}, {2, 11} , {15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29}  },
    {8,"Detroit Pistons", "Este", "Central", {5, 6, 7, 9}, {4, 0, 3, 10, 12, 13}, {2, 11}, {1, 14} , {15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29}  },
    {9,"Milwaukee Bucks", "Este", "Central", {5, 6, 7, 8}, {4, 2, 1, 14, 11, 13}, {3, 12}, {0, 10} , {15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29}  },
    {10,"Charlotte Hornets", "Este", "Sudeste", {11, 12, 13, 14}, {4, 0, 1, 7, 8, 5}, {3, 9}, {2, 6} , {15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29}  },
    {11,"Washington Wizards", "Este", "Sudeste", {10, 12, 13, 14}, {2, 3, 1, 6, 9, 5}, {0, 7}, {4, 8} , {15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29}  },
    {12,"Orlando Magic", "Este", "Sudeste", {10, 11, 13, 14}, {4, 0, 2, 6, 7, 8}, {1, 5}, {3, 9} , {15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29}  },
    {13,"Atlanta Hawks", "Este", "Sudeste", {10, 11, 12, 14}, {2, 3, 1, 9, 8, 5}, {4, 6}, {0, 7} , {15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29}  },
    {14,"Miami Heat", "Este", "Sudeste", {10, 11, 12, 13}, {4, 0, 3, 6, 7, 9}, {2, 8}, {1, 5} , {15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29}  },
    {15,"Oklahoma City Thunder", "Oeste", "Noroeste", {16, 17, 18, 19}, {28, 26, 29, 21, 24, 20}, {23, 25}, {27, 22} , {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14} },
    {16,"Minnesota Timberwolves", "Oeste", "Noroeste", {15, 17, 18, 19}, {28, 27, 29, 21, 24, 22}, {26, 20}, {25, 23} , {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14} },
    {17,"Utah Jazz", "Oeste", "Noroeste", {15, 16, 18, 19}, {26, 25, 27, 21, 22, 23}, {28, 24}, {29, 20} , {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14} },
    {18,"Denver Nuggets", "Oeste", "Noroeste", {15, 16, 17, 19}, {25, 26, 29, 24, 23, 20}, {27, 22}, {28, 21} , {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14} },
    {19,"Portland Trail Blazers", "Oeste", "Noroeste", {15, 16, 17, 18}, {28, 25, 27, 22, 23, 20}, {29, 21}, {26, 24} , {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14} },
    {20,"New Orleans Pelicans", "Oeste", "Sudoeste", {21, 22, 23, 24}, {19, 15, 18, 28, 25, 29}, {17, 26}, {16, 27} , {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14} },
    {21,"Memphis Grizzlies", "Oeste", "Sudoeste", {20, 22, 23, 24}, {15, 16, 17, 28, 27, 29}, {18, 25}, {19, 26} , {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14} },
    {22,"San Antonio Spurs", "Oeste", "Sudoeste", {20, 21, 23, 24}, {19, 16, 17, 25, 26, 27}, {15, 28}, {18, 29} , {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14} },
    {23,"Dallas Mavericks", "Oeste", "Sudoeste", {20, 21, 22, 24}, {19, 17, 18, 25, 26, 29}, {16, 27}, {15, 28} , {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14} },
    {24,"Houston Rockets", "Oeste", "Sudoeste", {20, 21, 22, 23}, {15, 16, 18, 28, 26, 27}, {19, 29}, {17, 25} , {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14} },
    {25,"Los Angeles Lakers", "Oeste", "Pacifico", {26, 27, 28, 29}, {19, 17, 18, 22, 23, 20}, {16, 24}, {15, 21} , {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14} },
    {26,"Los Angeles Clippers", "Oeste", "Pacifico", {25, 27, 28, 29}, {15, 17, 18, 24, 23, 22}, {19, 21}, {16, 20} , {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14} },
    {27,"Sacramento Kings", "Oeste", "Pacifico", {25, 26, 28, 29}, {19, 16, 17, 24, 22, 21}, {15, 20}, {18, 23} , {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14} },
    {28,"Golden State Warriors", "Oeste", "Pacifico", {25, 26, 27, 29}, {19, 15, 16, 21, 24, 20}, {18, 23}, {17, 22} , {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14} },
    {29,"Phoenix Suns", "Oeste", "Pacifico", {25, 26, 27, 28}, {15, 16, 18, 23, 20, 21}, {17, 22}, {19, 24} , {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14} }
};

vector<vector<double>> distanciasNBA = {
    {0, 217.9, 309.3, 213.78, 549.49, 984.73, 949.7, 639.99, 718.75, 1074.63, 848.51, 439.8, 1294.98, 1086.22, 1498.74, 1689.49, 1399.98, 2365.09, 1969.76, 3086.27, 1524.67, 1317.43, 2037.81, 1770.72, 1847.59, 2986.21, 2986.21, 3011.62, 3095.38, 2651.26},
    {217.9, 0, 98.12, 5.45, 473.07, 794.52, 727.77, 463.8, 616.45, 883.66, 634.7, 225.99, 1081.16, 872.39, 1284.93, 1467.56, 1209, 2174.12, 1778.79, 2895.3, 1311.08, 1103.84, 1824.24, 1551.85, 1634.01, 2795.25, 2795.25, 2820.64, 2904.41, 2429.33},
    {309.3, 98.12, 0, 96.67, 477.35, 760.7, 643.53, 430.74, 583.4, 850.6, 547.15, 138.44, 993.62, 784.85, 1197.38, 1383.31, 1175.94, 2141.06, 1726.81, 2862.24, 1223.54, 1016.3, 1736.68, 1469.58, 1546.46, 2714.24, 2714.24, 2787.59, 2871.35, 2345.09},
    {213.78, 5.45, 96.67, 0, 472.34, 792.92, 711.47, 462.99, 615.78, 883.08, 635.88, 227.17, 1082.35, 873.58, 1286.28, 1451.06, 1208.36, 2173.39, 1777.98, 2894.56, 1306.79, 1099.55, 1819.93, 1552.83, 1629.71, 2794.59, 2794.59, 2819.91, 2903.68, 2410.02},
    {549.49, 473.07, 477.35, 472.34, 0, 520.26, 536.48, 290.73, 241.91, 488.25, 757.77, 483.89, 1280.09, 968.86, 1483.86, 1284.51, 822.57, 1899.71, 1504.38, 2620.89, 1313.78, 992.65, 1721.69, 1454.37, 1550.24, 2520.82, 2520.82, 2546.23, 2630, 2243.44},
    {984.73, 794.52, 760.7, 792.92, 520.26, 0, 186.43, 345.63, 282.86, 91.97, 759.31, 704.15, 1156.67, 718.13, 1380.31, 796.77, 408.29, 1395.08, 999.74, 2116.25, 926.65, 535.3, 1239.12, 971.8, 1083.74, 2016.18, 2016.18, 2041.6, 2125.37, 1754.12},
    {949.7, 727.77, 643.53, 711.47, 536.48, 186.43, 0, 315.27, 297.86, 283.6, 575.29, 578.52, 970.33, 531.78, 1193.96, 747.37, 599.19, 1541.34, 1090.87, 2262.52, 819.36, 472.91, 1176.95, 909.47, 1021.35, 2078.3, 2078.3, 2187.86, 2271.63, 1706.17},
    {639.99, 463.8, 430.74, 462.99, 290.73, 345.63, 315.27, 0, 167.93, 435.08, 514.45, 374.96, 1036.77, 707.78, 1240.53, 1055.06, 760.43, 1725.6, 1330.27, 2446.78, 1052.7, 731.57, 1452.18, 1184.86, 1296.8, 2346.66, 2346.66, 2372.12, 2455.89, 2013.85},
    {718.75, 616.45, 583.4, 615.78, 241.91, 282.86, 297.86, 167.93, 0, 381.6, 637.84, 536.53, 1168.79, 730.24, 1392.42, 1047.1, 687.18, 1662.3, 1266.97, 2383.48, 1075.16, 754.03, 1484.28, 1216.97, 1328.91, 2283.41, 2283.41, 2308.82, 2392.59, 2006.03},
    {1074.63, 883.66, 850.6, 883.08, 488.25, 91.97, 283.6, 435.08, 381.6, 0, 852.37, 793.6, 1249.73, 811.19, 1473.37, 870.22, 336.98, 1437.61, 1042.28, 2063.52, 1016.2, 623.16, 1326.98, 1000.42, 1171.6, 2058.72, 2058.72, 2085.18, 2167.89, 1829.15},
    {848.51, 634.7, 547.15, 635.88, 757.77, 759.31, 575.29, 514.45, 637.84, 852.37, 0, 399.9, 524.78, 243.69, 728.54, 1084.84, 1180.93, 2032.98, 1561.09, 2754.16, 714.3, 629.88, 1226.39, 1029.09, 1036.17, 2423.84, 2423.84, 2679.5, 2701.77, 2090.87},
    {439.8, 225.99, 138.44, 227.17, 483.89, 704.15, 578.52, 374.96, 536.53, 793.6, 399.9, 0, 846.9, 636.35, 1050.51, 1342.69, 1118.03, 2083.15, 1661.54, 2804.32, 1085.4, 878.16, 1600.02, 1332.92, 1408.32, 2672.72, 2672.72, 2729.67, 2813.44, 2302.2},
    {1294.98, 1081.16, 993.62, 1082.35, 1280.09, 1156.67, 970.33, 1036.77, 1168.79, 1249.73, 524.78, 846.9, 0, 438.74, 233.63, 1285.54, 1567.61, 2310.21, 1839.74, 3031.39, 638.79, 821.61, 1152.3, 1086.67, 962.08, 2508.22, 2508.22, 2889.4, 2870.28, 2134.7},
    {1086.22, 872.39, 784.85, 873.58, 968.86, 718.13, 531.78, 707.78, 730.24, 811.19, 243.69, 636.35, 438.74, 0, 662.37, 848.94, 1129.4, 1872.01, 1401.54, 2593.19, 470.13, 385.01, 983.64, 784.23, 793.42, 2178.97, 2178.97, 2468.83, 2456.9, 1846},
    {1498.74, 1284.93, 1197.38, 1286.28, 1483.86, 1380.31, 1193.96, 1240.53, 1392.42, 1473.37, 728.54, 1050.51, 233.63, 662.37, 0, 1509.2, 1791.27, 2533.87, 2063.4, 3255.05, 862.45, 1045.27, 1375.96, 1310.33, 1185.74, 2731.88, 2731.88, 3113.06, 3093.94, 2358.36},
    {1689.49, 1467.56, 1383.31, 1451.06, 1284.51, 796.77, 747.37, 1055.06, 1047.1, 870.22, 1084.84, 1342.69, 1285.54, 848.94, 1509.2, 0, 789.32, 1188.33, 677.57, 1909.5, 708.01, 467.42, 462.68, 203.63, 445.71, 1331.81, 1331.81, 1621.66, 1609.74, 959.81},
    {1399.98, 1209, 1175.94, 1208.36, 822.57, 408.29, 599.19, 760.43, 687.18, 336.98, 1180.93, 1118.03, 1567.61, 1129.4, 1791.27, 789.32, 0, 1247.22, 914.03, 1728.14, 1222.13, 830.78, 1248.81, 940.77, 1180.22, 1930.83, 1930.83, 1893.74, 1977.51, 1645.74},
    {2365.09, 2174.12, 2141.06, 2173.39, 1899.71, 1395.08, 1541.34, 1725.6, 1662.3, 1437.61, 2032.98, 2083.15, 2310.21, 1872.01, 2533.87, 1188.33, 1247.22, 0, 519.27, 767.35, 1745.06, 1524.47, 1306.75, 1240.61, 1451, 691.96, 691.96, 652.12, 735.88, 659.75},
    {1969.76, 1778.79, 1726.81, 1777.98, 1504.38, 999.74, 1090.87, 1330.27, 1266.97, 1042.28, 1561.09, 1661.54, 1839.74, 1401.54, 2063.4, 677.57, 914.03, 519.27, 0, 1240.44, 1298.45, 1094.46, 936.22, 794, 1038.61, 1019.3, 1019.3, 1125.27, 1209.03, 863.56},
    {3086.27, 2895.3, 2862.24, 2894.56, 2620.89, 2116.25, 2262.52, 2446.78, 2383.48, 2063.52, 2754.16, 2804.32, 3031.39, 2593.19, 3255.05, 1909.5, 1728.14, 767.35, 1240.44, 0, 2511.46, 2249.05, 2080.23, 2007.01, 2251.62, 965.76, 965.76, 580.02, 635.81, 1334.76},
    {1524.67, 1311.08, 1223.54, 1306.79, 1313.78, 926.65, 819.36, 1052.7, 1075.16, 1016.2, 714.3, 1085.4, 638.79, 470.13, 862.45, 708.01, 1222.13, 1745.06, 1298.45, 2511.46, 0, 393.78, 537.12, 506.25, 346.79, 1895.66, 1895.66, 2276.23, 2258.13, 1521.16},
    {1317.43, 1103.84, 1016.3, 1099.55, 992.65, 535.3, 472.91, 731.57, 754.03, 623.16, 629.88, 878.16, 821.61, 385.01, 1045.27, 467.42, 830.78, 1524.47, 1094.46, 2249.05, 393.78, 0, 723.98, 454.46, 568.28, 1796.45, 1796.45, 2090.11, 2076.58, 1424.6},
    {2037.81, 1824.24, 1736.68, 1819.93, 1721.69, 1239.12, 1176.95, 1452.18, 1484.28, 1326.98, 1226.39, 1600.02, 1152.3, 983.64, 1375.96, 462.68, 1248.81, 1306.75, 936.22, 2080.23, 537.12, 723.98, 0, 271.27, 192.91, 1358.02, 1358.02, 1739.51, 1721.42, 984.45},
    {1770.72, 1551.85, 1469.58, 1552.83, 1454.37, 971.8, 909.47, 1184.86, 1216.97, 1000.42, 1029.09, 1332.92, 1086.67, 784.23, 1310.33, 203.63, 940.77, 1240.61, 794, 2007.01, 506.25, 454.46, 271.27, 0, 241.89, 1439.08, 1439.08, 1727.71, 1714.18, 1064.58},
    {1847.59, 1634.01, 1546.46, 1629.71, 1550.24, 1083.74, 1021.35, 1296.8, 1328.91, 1171.6, 1036.17, 1408.32, 962.08, 793.42, 1185.74, 445.71, 1180.22, 1451, 1038.61, 2251.62, 346.79, 568.28, 192.91, 241.89, 0, 1551.55, 1551.55, 1932.11, 1913.57, 1177.05},
    {2986.21, 2795.25, 2714.24, 2794.59, 2520.82, 2016.18, 2078.3, 2346.66, 2283.41, 2058.72, 2423.84, 2672.72, 2508.22, 2178.97, 2731.88, 1331.81, 1930.83, 691.96, 1019.3, 965.76, 1895.66, 1796.45, 1358.02, 1439.08, 1551.55, 0, 0, 391.58, 367.92, 373.52},
    {2986.21, 2795.25, 2714.24, 2794.59, 2520.82, 2016.18, 2078.3, 2346.66, 2283.41, 2058.72, 2423.84, 2672.72, 2508.22, 2178.97, 2731.88, 1331.81, 1930.83, 691.96, 1019.3, 965.76, 1895.66, 1796.45, 1358.02, 1439.08, 1551.55, 0, 0, 391.58, 367.92, 373.52},
    {3011.62, 2820.64, 2787.59, 2819.91, 2546.23, 2041.6, 2187.86, 2372.12, 2308.82, 2085.18, 2679.5, 2729.67, 2889.4, 2468.83, 3113.06, 1621.66, 1893.74, 652.12, 1125.27, 580.02, 2276.23, 2090.11, 1739.51, 1727.71, 1932.11, 391.58, 391.58, 0, 91.84, 755.3},
    {3095.38, 2904.41, 2871.35, 2903.68, 2630, 2125.37, 2271.63, 2455.89, 2392.59, 2167.89, 2701.77, 2813.44, 2870.28, 2456.9, 3093.94, 1609.74, 1977.51, 735.88, 1209.03, 635.81, 2258.13, 2076.58, 1721.42, 1714.18, 1913.57, 367.92, 367.92, 91.84, 0, 736.92},
    {2651.26, 2429.33, 2345.09, 2410.02, 2243.44, 1754.12, 1706.17, 2013.85, 2006.03, 1829.15, 2090.87, 2302.2, 2134.7, 1846, 2358.36, 959.81, 1645.74, 659.75, 863.56, 1334.76, 1521.16, 1424.6, 984.45, 1064.58, 1177.05, 373.52, 373.52, 755.3, 736.92, 0}
};

int main() {

    try {
        /*ofstream archivo("calendario_balanceado_lyv.txt"); // Abre el archivo (lo crea si no existe)

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
            
        /*
        // Restricciones: limitar diferencia acumulada entre partidos en casa y fuera
        for (int i = 0; i < N; ++i) {
            GRBLinExpr partidosEnCasaAcumulados = 0;
            GRBLinExpr partidosFueraAcumulados = 0;

            for (int k = 0; k < TOTAL_JORNADAS; ++k) {
                // Actualizar acumulados hasta jornada k
                for (int j = 0; j < N; ++j) {
                    if (i != j) {
                        partidosEnCasaAcumulados += x[i][j][k];     // Partido en casa
                        partidosFueraAcumulados += x[j][i][k];      // Partido fuera
                    }
                }

                // Definir la diferencia absoluta con variables auxiliares
                model.addConstr(partidosEnCasaAcumulados - partidosFueraAcumulados <= diff[i][k],"diff_pos_" + to_string(i) + "_" + to_string(k));
                model.addConstr(partidosFueraAcumulados - partidosEnCasaAcumulados <= diff[i][k],"diff_neg_" + to_string(i) + "_" + to_string(k));

                // L�mite de desequilibrio m�ximo permitido
                model.addConstr(diff[i][k] <= MAX_DIF_LOCAL_VISITANTE,"max_diff_" + to_string(i) + "_" + to_string(k));
            }
        }*/



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
                int rival = equipos[i].rivales_conf3[j];

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
                int rival = equipos[i].rivales_conf2[j];

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

