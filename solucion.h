#pragma once

using namespace std;

class solucion
{
public:
    solucion();
   // ~solucion();
    void initBalanceTriple(int balances_num);
    //setters
    void setBalanceTriple(int id, int rec1, int rec2, int ratio);
    void setPesoBalance(int peso);
    void setPesoMoverProceso(int peso);
    void setPesoMoverServicio(int peso);
    void setPesoMoverMaquina(int peso);
    
    //getters
    int getCostoSolucion();
    
private:
    int** balances; //balances[idx][triple]
    int balances_num;
    int peso_balance;
    int peso_proceso;
    int peso_servicio;
    int peso_maquina;
    
    int costo_solucion;
};