#pragma once

/* Clase proceso:
 * El proceso es la unidad básica del problema. Representa cuánto
 * gasta de la máquina que lo contiene.
 * Es necesario que sepa quién es, para poder calcular después su coste
 * de movimiento.
 */

class proceso
{
public:
    //Init
    proceso();
    ~proceso();
    void initRecursos(int recursos_num);
    //setters
    void setId(int id);
    void setServicio(int nServicio);
    void setUsoRecursos(int id, int valor);
    void setCosteMovimiento(int nCoste);
    //getters
    int getId();
    int getServicio();
    int getUsoRecursos(int recurso);
    int getCosteMovimiento();

private:
    int id_proceso;
    int id_servicio;
    int* usoRecursos;
    int coste_movimiento;
};

