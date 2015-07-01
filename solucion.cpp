#include "solucion.h"

solucion::solucion()
{
    //do nothing
}

/*esto tira segfault, no sé por qué
solucion::~solucion()
{
    for (int i = 0; i < balances_num; ++i)
    {
        delete[] balances[i];
    }
    delete[] balances;
}*/

//esta es la parte que se pone interesante
int solucion::getCostoSolucion()
{
    costo_solucion = 0;
    
    //do stuff
    
    return costo_solucion;
}

//balances[idx][triple]
void solucion::initBalanceTriple(int nBalances)
{
    balances_num = nBalances;
    balances = new int* [balances_num];
    for (int i = 0; i < balances_num; ++i)
    {
        balances[i] = new int[3];
    }
}

void solucion::setBalanceTriple(int id, int rec1, int rec2, int ratio)
{
    balances[id][0] = rec1;
    balances[id][1] = rec2;
    balances[id][3] = ratio;
}

void solucion::setPesoBalance(int peso)
{
    peso_balance = peso;
}

void solucion::setPesoMoverMaquina(int peso)
{
    peso_maquina = peso;
}

void solucion::setPesoMoverProceso(int peso)
{
    peso_proceso = peso;
}

void solucion::setPesoMoverServicio(int peso)
{
    peso_servicio = peso;
}