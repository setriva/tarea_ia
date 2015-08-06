#pragma once
#include <vector>

/* Clase proceso:
 * El proceso es la unidad básica del problema. Representa cuánto
 * gasta de la máquina que lo contiene.
 * Es necesario que sepa quién es, para poder calcular después su coste
 * de movimiento.
 */

using namespace std;

class proceso
{
public:
    //Init
    proceso();
    //~proceso();
    void initRecursos(int recursos_num);
    //setters
    void setId(int id);
    void setServicio(int nServicio);
    void setUsoRecursos(int id, int valor);
    void setUsoRecursos(vector<int> vec);
    void setCosteMovimiento(int nCoste);
    void setPrioridad(int prior);
    //getters
    int getId();
    int getServicio();
    int getUsoRecursos(int recurso);
    vector<int> getUsoRecursosVector();
    int getCosteMovimiento();

    void calcularPeso();
    int getPeso() const;
    int getPrioridad() const;

private:
    int id_proceso;
    int id_servicio;
    vector<int> usoRecursos;
    int coste_movimiento;
    //ordenamiento
    int prioridad;
    int peso;
};

