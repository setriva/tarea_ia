#pragma once
#include <vector>
#include "proceso.h"

using namespace std;

/* Clase máquina:
 * La máquina es la que ejecuta los procesos. Estos procesos ocupan recursos,
 * y conviene que la misma máquina sea capaz de reportar:
 * - cuánto le queda
 * - cuánto se ha pasado de su capacidad segura
 * La máquina se encuentra en un array fijo (porque no se agregarán más máquinas)
 * y sabe su ubicación y vecindario.
 */

class maquina
{
public:
    maquina();
    void init(int id, int recursos_num);

    //setters
    void setId(int id);
    void setLocation(int nLocation);
    void setNeighborhood(int nNeighborhood);
    void setCapacidadMax(int id, int valor);
    void setCapacidadMax(vector<int> vec);
    void setCapacidadSafe(int id, int valor);
    void setCapacidadSafe(vector<int> vec);
    void setCapacidadUtilizada(int id, int valor);
    void setCapacidadUtilizada(vector<int> vec);
    void setBalances(vector<vector<int>>* bals);

    //getters
    int getId();
    int getNeighborhood();
    int getLocation();

    int getEspacioMax(int recurso);
    vector<int> getEspacioMaxVector();
    vector<int> getEspacioSafeVector();

    //funciones para el greedy
    void calcular_capacidad();
    bool operator<(const maquina& maq) const;
    bool agregar_proceso(proceso proc);
    bool quitar_proceso(proceso proc);

private:
    int id;
    int recursos_num;
    int location, neighborhood;
    vector<int> capacidad_max;
    vector<int> capacidad_safe;

    //esto para calcular el greedy
    vector<int> capacidad_utilizada;
    vector<vector<int>>* balances;
    float capacidad_disponible;   //porcentaje, esto va entre 0 y 1
};
