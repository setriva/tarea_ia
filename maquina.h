#pragma once
#include <list>
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
    ~maquina();
    void init(int id, int recursos_num, int maquinas_num);

    //setters
    void setLocation(int nLocation);
    void setNeighborhood(int nNeighborhood);
    void setCapacidadMax(int id, int valor);
    void setCapacidadSafe(int id, int valor);
    void setCapacidadUtilizada(int id, int valor);
    void setCosteMovimiento(int id, int valor);

    //getters
    int getId();
    int getNeighborhood();
    int getLocation();

    int getEspacioMax(int recurso);

    //funciones para el greedy
    void calcular_capacidad();
    bool operator<(const maquina& maq) const;
    //ojo: la máquina lleva un costo nomás, se obtienen los costos del proceso y se agregan al total
    bool agregar_proceso(proceso proc);
    bool quitar_proceso(proceso proc);

private:
    int id;
    int recursos_num;
    int location, neighborhood;
    int* capacidad_max;
    int* capacidad_safe;
    int* costos_movimiento;

    //esto para calcular el greedy
    int* capacidad_utilizada;
    float capacidad_disponible;   //porcentaje, esto va entre 0 y 1
};
