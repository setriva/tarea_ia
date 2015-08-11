#include "proceso.h"

proceso::proceso()
{
    id_proceso = -1;
    id_servicio = -1;
    usoRecursos = vector<int>();
    coste_movimiento = 0;
    prioridad = 0;
    peso = 0;
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
    calcularPeso();
}

void proceso::setUsoRecursos(vector<int> vec)
{
	usoRecursos = vec;
	calcularPeso();
}

void proceso::setCosteMovimiento(int nCoste)
{
    coste_movimiento = nCoste;
}

void proceso::setPrioridad(int prior)
{
	prioridad = prior;
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

int proceso::getPeso() const
{
	return peso;
}

int proceso::getPrioridad() const
{
	return prioridad;
}

int proceso::getCosteMovimiento()
{
	return coste_movimiento;
}

void proceso::calcularPeso()
{
	//opción 1: promedio
	/*try
	{
		for (size_t r = 0; r < usoRecursos.size(); ++r)
				peso += usoRecursos.at(r);

		peso = peso / usoRecursos.size();
	}
	catch (...)
	{

	}*/

	//opción 2: máximo uso numérico de recursos
	peso = 0;
	for (size_t r = 0; r < usoRecursos.size(); ++r)
		if (peso < usoRecursos.at(r)) peso = usoRecursos.at(r);
}
