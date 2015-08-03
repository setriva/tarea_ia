#include "servicio.h"

servicio::servicio()
{
    //do nothing
    dependencias_num = 0;
}

servicio::~servicio()
{
    delete[] dependencias;
}

void servicio::initDependencias(int dnum)
{
    dependencias_num = dnum;
    dependencias = new int[dnum];
}

void servicio::setId(int valor)
{
    id = valor;
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

int servicio::getDependenciaNum()
{
    return dependencias_num;
}

int servicio::getSpreadmin()
{
    return spreadmin;
}

void servicio::pushProceso(int idProceso)
{
    idProcesos.push_back(idProceso);
}
