#pragma once
#include <list>
#include "proceso.h"

class maquina
{
public:
    maquina();
    ~maquina();
    void init(int recursos_num, int maquinas_num);
    //setters
    void setLocation(int nLocation);
    void setNeighborhood(int nNeighborhood);
    void setCapacidadMax(int id, int valor);
    void setCapacidadSafe(int id, int valor);
    void setCosteMovimiento(int id, int valor);
    //getters
    int getNeighborhood();
    int getLocation();
    
private:
    std::list<proceso> procesos;
    int location, neighborhood;
    int* capacidad_max;
    int* capacidad_safe;
    int* costos_movimiento;
};