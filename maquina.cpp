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
    delete[] capacidad_utilizada;
    delete[] costos_movimiento;
}

void maquina::init(int id, int recursos_num, int maquinas_num)
{
    id = this->id;
    capacidad_max = new int[recursos_num];
    capacidad_safe = new int[recursos_num];
    capacidad_utilizada = new int[recursos_num];
    for (int r = 0; r < recursos_num; ++r)
    {
        capacidad_max[r] = 0;
        capacidad_safe[r] = 0;
        capacidad_utilizada[r] = 0;
    }
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

void maquina::setCapacidadUtilizada(int id, int valor)
{
    capacidad_utilizada[id] = valor;
    //requiere recalcular la capacidad disponible
    calcular_capacidad();
}

int maquina::getId()
{
    return id;
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
    capacidad_disponible = 0.0f;
    for(int r = 0; r < recursos_num; ++r)
    {
        //sumar recurso_disponible / recurso_total para todos los recursos
        capacidad_disponible += (((float)capacidad_max[r] - (float)capacidad_utilizada[r]) / (float)capacidad_max[r]);
    }

    //promedio (suma de razones / cantidad de recursos)
    capacidad_disponible = capacidad_disponible / recursos_num;
}

bool maquina::operator<(const maquina& maq) const
{
    return capacidad_disponible < maq.capacidad_disponible;
}

bool maquina::agregar_proceso(proceso proc)
{
    //revisar si alguna capacidad se pasa
    for (int r = 0; r < recursos_num; ++r)
    {
        //capacidad no se pasa
        if (proc.getUsoRecursos(r) <= (capacidad_max[r] - capacidad_utilizada[r]))
            continue;
        //capacidad sí se pasa, abortar
        else
            return false;
    }
    //commit
    for (int r = 0; r < recursos_num; ++r)
    {
        capacidad_utilizada[r] += proc.getUsoRecursos(r);
    }
    //calcular
    calcular_capacidad();
    return true;
}

bool maquina::quitar_proceso(proceso proc)
{
    bool status = true;
    for(int r = 0; r < recursos_num; ++r)
    {
        capacidad_utilizada[r] -= proc.getUsoRecursos(r);
        if (capacidad_utilizada[r] < 0) status = false;
    }
    return status;
}
