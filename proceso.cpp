#include "proceso.h"

//Funciones de la clase proceso
proceso::proceso()
{
    //do nothing
}

proceso::~proceso()
{
    delete[] recursos;
}

void proceso::initRecursos(int recursos_num)
{
    recursos = new int[recursos_num];
}

void proceso::setServicio(int nServicio)
{
    id_servicio = nServicio;
}

void proceso::setUsoRecursos(int id, int valor)
{
    recursos[id] = valor;
}

void proceso::setCosteMovimiento(int nCoste)
{
    coste_movimiento = nCoste;
}