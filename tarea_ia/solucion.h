#pragma once

#include <iostream>
#include <fstream>
#include <string>
#include <vector>

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
    //~solucion();

    void initBalanceTriple(int balances_num);

    //setters
    void setBalanceTriple(vector<vector<int>>* bals);

    void setPesoBalance(int peso);
    void setPesoMoverProceso(int peso);
    void setPesoMoverServicio(int peso);
    void setPesoMoverMaquina(int peso);

    void setCMM(vector<vector<int>>* cmm); //costeMovimientoMaquinas

    void setParametrosRecursos(int num, vector<int>* peso, vector<bool>* trans);

    void setCantidades(int procs, int maqs, int srvcs, int locs, int vecs);

    void setObjetos(vector<proceso>* procs, vector<maquina>* maqs, vector<servicio>* srvs);

    //getters
    int getCostoSolucion();

    //fillers
    bool llenarSolucion();              //aplicar greedy
    int llenarSolucion(ifstream& fsol);  //leer archivo entrada

    bool ordenProcesoPrioridad(const proceso& left, const proceso& right) const;
    bool ordenProcesoPeso(const proceso& left, const proceso& right) const;

    void calcularCostoSolucion();

    void descartarGreedy();

    //exploradores
    bool move();
    bool swap();
    bool bigmove();

private:
//Parámetros del modelo
    //Costos
    vector<vector<int>>* costoMovimientoMaquinas;      //machinemovecost
    int balances_num;
    vector<vector<int>>* balances;                     //balancecost

    //Pesos
    int peso_proceso;
    int peso_servicio;
    int peso_maquina;

    //Recursos
    int recursos_num;
    vector<int>* peso_recursos;
    vector<bool>* recursos_trans;

    //Cantidades
    int procesos_num;
    int servicios_num;
    int maquinas_num;
    int localizaciones_num;
    int vecindarios_num;

    //Objetos lógicos
    vector<proceso>* procesos;
    vector<maquina>* maquinas;
    vector<servicio>* servicios;

    //ifstream fsol;

//Parámetros de la solución
    //Estructuras de datos
    vector<int> asignacion_procesos;       //máquina a la que está asignado cada proceso
    vector<vector<int>> espacio_disponible;       //espacio disponible por máquina, por recurso
    vector<vector<bool>> servicio_maquina;        //indica si proceso de un servicio está asignado a una máquina
    vector<vector<int>> servicio_localizacion;    //indica procesos de servicio en localizacion
    vector<vector<int>> servicio_vecindario;      //indica procesos de servicio en vecindario
    vector<int> spread_servicios;         //cantidad de localizaciones con procesos del servicio

    //Fitness
    int costo_solucion;

    //Sol_inicial, si es que la hay
    const solucion* sol_inicial;
};
