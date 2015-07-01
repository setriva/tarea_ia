#include "maquina.h"

//Funciones de la clase máquina
maquina::maquina()
{
    //do nothing
}

maquina::~maquina()
{
    delete[] capacidad_max;
    delete[] capacidad_safe;
    delete[] costos_movimiento;
}

void maquina::init(int recursos_num, int maquinas_num)
{
    capacidad_max = new int[recursos_num];
    capacidad_safe = new int[recursos_num];
    costos_movimiento = new int[maquinas_num];
}

void maquina::setLocation(int nLocation)
{
    location = nLocation;
}

void maquina::setNeighborhood(int nNeighborhood)
{
    neighborhood = nNeighborhood;
}

void maquina::setCapacidadMax(int id, int valor)
{
    capacidad_max[id] = valor;
}

void maquina::setCapacidadSafe(int id, int valor)
{
    capacidad_safe[id] = valor;
}

void maquina::setCosteMovimiento(int id, int valor)
{
    costos_movimiento[id] = valor;
}

int maquina::getNeighborhood()
{
    return neighborhood;
}

int maquina::getLocation()
{
    return location;
}