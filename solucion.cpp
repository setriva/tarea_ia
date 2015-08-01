#include "solucion.h"
#include <list>

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

//constructor de copia de solución
//sol es la solución inicial
//no es necesaria para la solución del greedy
solucion::solucion(const solucion& sol, bool mantener_inicial)
{
    //Referencia a la solución inicial
    if (mantener_inicial) sol_inicial = &sol;
    //costos
    costoMovimientoMaquinas = sol.costoMovimientoMaquinas;
    initBalanceTriple(sol.balances_num);

    //pesos
    peso_balance = sol.peso_balance;
    peso_proceso = sol.peso_proceso;
    peso_servicio = sol.peso_servicio;
    peso_maquina = sol.peso_maquina;

    //recursos
    recursos_num = sol.recursos_num;
    peso_recursos = sol.peso_recursos;
    recursos_trans = sol.recursos_trans;

    //cantidades
    setCantidades(sol.procesos_num, sol.maquinas_num, sol.servicios_num, sol.localizaciones_num, sol.vecindarios_num);

    //objetos
    setObjetos(sol.procesos, sol.maquinas);
}

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

void solucion::setParametrosRecursos(int num, int* peso, bool* trans)
{
    recursos_num = num;
    peso_recursos = peso;
    recursos_trans = trans;
}

void solucion::setCMM(int** cmm)
{
    costoMovimientoMaquinas = cmm;
}

void solucion::setCantidades(int procs, int maqs, int srvcs, int locs, int vecs)
{
    //asignar números
    procesos_num = procs;
    maquinas_num = maqs;
    servicios_num = srvcs;
    localizaciones_num = locs;
    vecindarios_num = vecs;

    //inicializar matrices y vectores
    asignacion_procesos = new int[procesos_num];
    for (int i = 0; i < procesos_num; ++i)      //init a -1
        asignacion_procesos[i] = -1;
    spread_servicios = new int[servicios_num];
    for (int i = 0; i < servicios_num; ++i)     //init a 0
        spread_servicios[i] = 0;

    //espacio_disponible[maquina][recurso]
    espacio_disponible = new int*[maquinas_num];
    for (int i = 0; i < maquinas_num; ++i)
    {
        espacio_disponible[i] = new int[recursos_num];
        for (int j = 0; j < recursos_num; ++j)   //init a 0
            espacio_disponible[i][j] = 0;
    }

    //servicio_maquina[servicio][maquina]
    servicio_maquina = new bool*[servicios_num];
    for (int i = 0; i < servicios_num; ++i)
    {
        servicio_maquina[i] = new bool[maquinas_num];
        for (int j = 0; j < maquinas_num; ++j)  //init to false
            servicio_maquina[i][j] = false;
    }

    //servicio_localizacion[servicio][localizacion]
    servicio_localizacion = new int*[servicios_num];
    for (int i = 0; i < servicios_num; ++i)
    {
        servicio_localizacion[i] = new int[localizaciones_num];
        for (int j = 0; j < localizaciones_num; ++j)    //init to 0
            servicio_localizacion[i][j] = 0;
    }

    //servicio_vecindario[servicio][vecindario]
    servicio_vecindario = new int*[servicios_num];
    for (int i = 0; i < servicios_num; ++i)
    {
        servicio_vecindario[i] = new int[vecindarios_num];
        for (int j = 0; j < vecindarios_num; ++j)   //init to 0
            servicio_vecindario[i][j] = 0;
    }

}

void solucion::setObjetos(proceso* procs, maquina* maqs)
{
    procesos = procs;
    maquinas = maqs;
}

int solucion::llenarSolucion(char* entrada)
{
    int maq, srv, loc, vec;
    string line;
    ifstream fsol(entrada, ifstream::in);

    if (!fsol.good())
    {
        cout << "Solución inicial no existe!" << endl;
        return -1;
    }

    //obteniendo espacio de las máquinas
    for(int m = 0; m < maquinas_num; ++m)
        for (int r = 0; r < recursos_num; ++r)
            espacio_disponible[m][r] = maquinas[m].getEspacioMax(r);

    //leyendo la solución inicial
    for(int p = 0; p < procesos_num; ++p)
    {
        fsol >> line; maq = stoi(line);

        //cada número es la máquina a la que está asignado un proceso (maq)
        asignacion_procesos[p] = maq;

        //restar al espacio disponible
        for (int r = 0; r < recursos_num; ++r)
            espacio_disponible[maq][r] -= procesos[p].getUsoRecursos(r);

        //servicio en la máquina
        srv = procesos[p].getServicio();
        servicio_maquina[srv][maq] = true;

        //sumar servicio en la localización y en el vecindario
        loc = maquinas[maq].getLocation();
        vec = maquinas[maq].getNeighborhood();
        servicio_localizacion[srv][loc] += 1;
        servicio_vecindario[srv][vec] += 1;

        //spread factor
        spread_servicios[srv] += 1;

    }

    //done
    return 0;
}

int solucion::llenarSolucion()
{
    //copiar máquinas
    list<maquina> maquinas_sort;
    for (int m = 0; m < maquinas_num; ++m)
    {
        maquinas_sort.push_back(maquinas[m]);
    }
    list<maquina>::iterator maq_i;

    //llenar matriz de espacio disponible en las máquinas
    for(int m = 0; m < maquinas_num; ++m)
        for(int r = 0; r < recursos_num; ++r)
            espacio_disponible[m][r] = maquinas[m].getEspacioMax(r);

    //en este punto las máquinas están vacías, hay que empezar a llenarlas
    //la lista se ordena después de cada inserción
    //agregar a la primera máquina que se pueda

    bool insert_ok;
    int m_id;
    int srv;

    //loop principal
    for(int p = 0; p < procesos_num; ++p)
    {
        for(maq_i = maquinas_sort.begin(); maq_i != maquinas_sort.end(); ++maq_i)
        {
            m_id = maq_i->getId();
            //Revisión de restricciones:
            insert_ok = true;
            //Capacidad (no cuenta el uso transitorio)
            for(int r = 0; r < recursos_num; ++r)
            {
                //para cada recurso, ver si hay suficiente espacio en la máquina
                if(espacio_disponible[m_id][r] < procesos[p].getUsoRecursos(r)) insert_ok = false;
            }

            //Conflicto de servicios
            srv = procesos[p].getServicio();
            if (servicio_maquina[srv][m_id]) insert_ok = false;

            //Conflicto de localizaciones (según spreadmin)


            //Conflicto de vecindarios (según dependencia)
        }

    }


}





