#pragma once

#include <iostream>
#include <fstream>
#include <string>

#include "proceso.h"
#include "maquina.h"
#include "servicio.h"

using namespace std;

/* Clase solución:
 * Esta clase es importante. La gracia es que reúne todo lo necesario para
 * calcular el costo de la solución, e implementa los movimientos de procesos
 * entre máquinas. Además, tiene que poder saber si es una solución válida.
 * Por lo tanto, requiere:
 * - lista propia de asignación de procesos
 * - referencia a lista de máquinas, servicios, etc
 * - referencia a los distintos costes
 * - operador de copia, para generar soluciones nuevas
 */

class solucion
{
public:
    solucion();
    solucion(const solucion& sol, bool mantener_inicial = true);
    // ~solucion();

    void initBalanceTriple(int balances_num);

    //setters
    void setBalanceTriple(int id, int rec1, int rec2, int ratio);
    void setPesoBalance(int peso);
    void setPesoMoverProceso(int peso);
    void setPesoMoverServicio(int peso);
    void setPesoMoverMaquina(int peso);
    void setCMM(int** cmm); //costeMovimientoMaquinas

    void setParametrosRecursos(int num, int* peso, bool* trans);

    void setCantidades(int procs, int maqs, int srvcs, int locs, int vecs);

    void setObjetos(proceso* procs, maquina* maqs, servicio* srvs);

    //getters
    int getCostoSolucion();

    //fillers
    int llenarSolucion();              //aplicar greedy
    int llenarSolucion(char* entrada);  //leer archivo entrada

private:
//Parámetros del modelo
    //Costos
    int** costoMovimientoMaquinas;      //machinemovecost
    int balances_num;
    int** balances;                     //balancecost

    //Pesos
    int peso_balance;
    int peso_proceso;
    int peso_servicio;
    int peso_maquina;

    //Recursos
    int recursos_num;
    int* peso_recursos;
    bool* recursos_trans;

    //Cantidades
    int procesos_num;
    int servicios_num;
    int maquinas_num;
    int localizaciones_num;
    int vecindarios_num;

    //Objetos lógicos
    proceso* procesos;
    maquina* maquinas;
    servicio* servicios;


//Parámetros de la solución
    //Estructuras de datos
    int* asignacion_procesos;       //máquina a la que está asignado cada proceso
    int** espacio_disponible;       //espacio disponible por máquina, por recurso
    bool** servicio_maquina;        //indica si proceso de un servicio está asignado a una máquina
    int** servicio_localizacion;    //indica procesos de servicio en localizacion
    int** servicio_vecindario;      //indica procesos de servicio en vecindario
    int* spread_servicios;         //cantidad de localizaciones con procesos del servicio

    //Fitness
    int costo_solucion;

    //Sol_inicial, si es que la hay
    const solucion* sol_inicial;
};
