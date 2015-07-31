#include "maquina.h"

//Funciones de la clase máquina
maquina::maquina()
{
    recalcular_capacidad = true;
}

maquina::~maquina()
{
    delete[] capacidad_max;
    delete[] capacidad_safe;
    delete[] costos_movimiento;
}

void maquina::init(int id, int recursos_num, int maquinas_num)
{
    id = this->id;
    capacidad_max = new int[recursos_num];
    capacidad_safe = new int[recursos_num];
    capacidad_utilizada = new int[recursos_num]
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

int maquina::getEspacioMax(int recurso)
{
    return capacidad_max[recurso];
}

void maquina::calcular_capacidad()
{
    capacidad_disponible = 0;
    for(int r = 0; r < recursos_num; ++r)
    {
        //sumar recurso_disponible / recurso_total para todos los recursos
        capacidad_disponible += ((capacidad_max[r] - capacidad_utilizada[r]) / capacidad_max[r]);
    }
    //promedio (suma de razones / cantidad de recursos)
    capacidad_disponible = capacidad_disponible / recursos_num;
}

bool maquina::agregar_proceso(proceso proc)
{
    //revisar si alguna capacidad se pasa
    for (int r = 0; r < recursos_num; ++r)
    {

    }
    //commit

}
