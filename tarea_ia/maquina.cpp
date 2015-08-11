#include "maquina.h"
#include <limits>

//Funciones de la clase máquina
maquina::maquina()
{
    id = -1;
    recursos_num = 0;
    location = neighborhood = -1;
    capacidad_max = vector<int>();
    capacidad_safe = vector<int>();
    capacidad_utilizada = vector<int>();
    capacidad_disponible = 0.0f;
    balances = 0;
}


void maquina::init(int nid, int nrecs)
{
    id = nid;
    recursos_num = nrecs;
    capacidad_utilizada.resize(recursos_num, 0);
}

void maquina::setId(int id)
{
	this->id = id;
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
    capacidad_max.at(id) = valor;
}

void maquina::setCapacidadMax(vector<int> vec)
{
    capacidad_max = vec;
}

void maquina::setCapacidadSafe(int id, int valor)
{
    capacidad_safe.at(id) = valor;
}

void maquina::setCapacidadSafe(vector<int> vec)
{
    capacidad_safe = vec;
}

void maquina::setBalances(vector<vector<int>>* bals)
{
	balances = bals;
}

void maquina::setCapacidadUtilizada(int id, int valor)
{
    capacidad_utilizada.at(id) = valor;
    //requiere recalcular la capacidad disponible
    calcular_capacidad();
}

void maquina::setCapacidadUtilizada(vector<int> vec)
{
    capacidad_utilizada = vec;
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
    return capacidad_max.at(recurso);
}

vector<int> maquina::getEspacioMaxVector()
{
	return capacidad_max;
}

vector<int> maquina::getEspacioSafeVector()
{
	return capacidad_safe;
}

void maquina::calcular_capacidad()
{
	//máquinas con menor capacidad disponible se van al final
    capacidad_disponible = 0.0f;
    //opción 1: ponderar recursos y dar el promedio
    /*for(int r = 0; r < recursos_num; ++r)
    {
        //sumar recurso_disponible / recurso_total para todos los recursos
        capacidad_disponible += (((float)capacidad_max.at(r) - (float)capacidad_utilizada.at(r)) / (float)capacidad_max.at(r));
    }

    //promedio (suma de razones / cantidad de recursos)
    capacidad_disponible = capacidad_disponible / recursos_num;
    */

    //opción 2: usar como índice el recurso con numéricamente menor capacidad disponible
    int disp;
    capacidad_disponible = numeric_limits<int>::max();
    for (int r = 0; r < recursos_num; ++r)
    {
    	disp = capacidad_max.at(r) - capacidad_utilizada.at(r);
    	if (disp < capacidad_disponible) capacidad_disponible = disp;
    }

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
        if (proc.getUsoRecursos(r) <= (capacidad_max.at(r) - capacidad_utilizada.at(r)))
            continue;
        //capacidad sí se pasa, abortar
        else
            return false;
    }
    //commit
    for (int r = 0; r < recursos_num; ++r)
    {
        capacidad_utilizada.at(r) += proc.getUsoRecursos(r);
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
        capacidad_utilizada.at(r) -= proc.getUsoRecursos(r);
        if (capacidad_utilizada.at(r) < 0) status = false;
    }
    return status;
}
