#include "proceso.h"

proceso::proceso()
{
    //do nothing
}

proceso::~proceso()
{
    delete[] usoRecursos;
}

void proceso::initRecursos(int recursos_num)
{
    usoRecursos = new int[recursos_num];
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
    usoRecursos[id] = valor;
}

void proceso::setCosteMovimiento(int nCoste)
{
    coste_movimiento = nCoste;
}

int proceso::getUsoRecursos(int recurso)
{
    return usoRecursos[recurso];
}

int proceso::getId()
{
    return id_proceso;
}

int proceso::getServicio()
{
    return id_servicio;
}
