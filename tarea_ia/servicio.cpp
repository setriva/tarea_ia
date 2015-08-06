#include "servicio.h"

servicio::servicio()
{
    id = -1;
    spreadmin = -1;
    dependencias = vector<int>();
    dependencias_num = 0;
    idProcesos = vector<int>();
    prioridad = 0;
}

void servicio::initDependencias(int dnum)
{
    dependencias_num = dnum;
    dependencias.resize(dnum);
}

void servicio::setId(int valor)
{
    id = valor;
}

void servicio::setDependencia(int id, int valor)
{
    dependencias.at(id) = valor;
}

void servicio::setDependencia(vector<int> vec)
{
    dependencias = vec;
}

void servicio::setSpreadmin(int nspreadmin)
{
    spreadmin = nspreadmin;
}

int servicio::getDependencia(int id)
{
    return dependencias.at(id);
}

int servicio::getDependenciaNum()
{
    return dependencias_num;
}

int servicio::getId()
{
	return id;
}

int servicio::getSpreadmin()
{
    return spreadmin;
}

void servicio::pushProceso(int idProceso)
{
    idProcesos.push_back(idProceso);
}

vector<int> servicio::getListaProcesos()
{
	return idProcesos;
}

bool servicio::operator<(const servicio& srv) const
{
    return prioridad < srv.prioridad;
}

void servicio::calcularPrioridad()
{
	prioridad = idProcesos.size();
	if (prioridad < dependencias_num) prioridad = dependencias_num;
	if (prioridad < spreadmin) prioridad = spreadmin;
}









