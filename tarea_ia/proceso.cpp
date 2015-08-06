#include "proceso.h"

proceso::proceso()
{
    id_proceso = -1;
    id_servicio = -1;
    usoRecursos = vector<int>();
    coste_movimiento = 0;
}

void proceso::initRecursos(int recursos_num)
{
    usoRecursos.resize(recursos_num);
}

void proceso::setId(int id)
{
    id_proceso = id;
}

void proceso::setServicio(int nServicio)
{
    id_servicio = nServicio;
}

void proceso::setUsoRecursos(int id, int valor)
{
    usoRecursos.at(id) = valor;
}

void proceso::setUsoRecursos(vector<int> vec)
{
	usoRecursos = vec;
}

void proceso::setCosteMovimiento(int nCoste)
{
    coste_movimiento = nCoste;
}

int proceso::getUsoRecursos(int recurso)
{
    return usoRecursos.at(recurso);
}

vector<int> proceso::getUsoRecursosVector()
{
	return usoRecursos;
}

int proceso::getId()
{
    return id_proceso;
}

int proceso::getServicio()
{
    return id_servicio;
}
