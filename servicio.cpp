#include "servicio.h"

servicio::servicio()
{
    //do nothing
}

servicio::~servicio()
{
    delete[] dependencias;
}

void servicio::initDependencias(int dependencias_num)
{
    dependencias = new int[dependencias_num];
}

void servicio::setDependencia(int id, int valor)
{
    dependencias[id] = valor;
}

void servicio::setSpreadmin(int nspreadmin)
{
    spreadmin = nspreadmin;
}

int servicio::getDependencia(int id)
{
    return dependencias[id];
}

int servicio::getSpreadmin()
{
    return spreadmin;
}

void servicio::pushProceso(int idProceso)
{
    idProcesos.push_back(idProceso);
}