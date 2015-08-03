#include "solucion.h"
#include <list>
#include <iterator>

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
//mantener_inicial es por si acaso no necesitara mantener
//la solución inicial para calcular algo
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
    setObjetos(sol.procesos, sol.maquinas, sol.servicios);
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

void solucion::setObjetos(proceso* procs, maquina* maqs, servicio* svcs)
{
    procesos = procs;
    maquinas = maqs;
    servicios = svcs;
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
    {
        for(int r = 0; r < recursos_num; ++r)
        {
            espacio_disponible[m][r] = maquinas[m].getEspacioMax(r);
            //manejo de recursos transitivos
            if (recursos_trans[r])
            {
                //si el recurso es transitivo, está ocupado por el movimiento anterior
                //basta con igualarlo en esta parte
                espacio_disponible[m][r] = sol_inicial.espacio_disponible[m][r];

                //hasta ahora, maquinas_sort está ordenado por id
                //así que se puede acceder directamente (en teoría)
                maq_i->setCapacidadUtilizada(r, (maquinas[m].getEspacioMax(r) - espacio_disponible[m][r]));
            }
        }
        //avanzar el iterador
        advance(maq_i, 1);
    }

    //ahora se ordena la lista
    //de ahora en adelante no se asegura que los id de las máquinas sean correlativos
    maquinas_sort.sort();

    //lista de procesos que no han podido agregarse aún
    list<proceso> procesos_cola;
    list<proceso>::iterator proc_i;

    //en este punto las máquinas están vacías, hay que empezar a llenarlas

    //la lista se ordena después de cada inserción (al final del loop principal)
    //agregar a la primera máquina que se pueda

    bool insert_ok;         //flag de inserción posible
    int m_id;               //id de la máquina
    int srv, dep, srv_dep;  //parámetros del proceso
    int loc, vec;           //parámetros de la máquina

    //loop principal
    for(int p = 0; p < procesos_num; ++p)
    {
        //servicio del proceso
        srv = procesos[p].getServicio();
        //dependencias del proceso
        dep = servicios[srv].getDependenciaNum();
        //revisar las máquinas ordenadas
        for(maq_i = maquinas_sort.begin(); maq_i != maquinas_sort.end(); ++maq_i)
        {
            //obtener id de la máquina, localización, vecindario
            m_id = maq_i->getId();
            loc = maquinas[m_id].getLocation();
            vec = maquinas[m_id].getNeighborhood();

            //Revisión de restricciones:
            insert_ok = true;
            //Capacidad (no cuenta el uso transitorio)
            for(int r = 0; r < recursos_num; ++r)
            {
                //para cada recurso, ver si hay suficiente espacio en la máquina
                if(espacio_disponible[m_id][r] < procesos[p].getUsoRecursos(r)) insert_ok = false;
            }

            //Conflicto de servicios
            if (servicio_maquina[srv][m_id]) insert_ok = false;

            //Conflicto de localizaciones (según spreadmin)
            if(spread_servicios[srv] < servicios[srv].getSpreadmin())
            {
                //servicio no cumple con spreadmin
                //si ya hay un proceso en la localización, buscar otra máquina
                if (servicio_localizacion[srv][loc] > 0) insert_ok = false;
            }//else, servicio cumple con spreadmin, carry on

            //Conflicto de vecindarios (según dependencia)
            if (dep > 0)
            {
                //Servicio tiene dependencias, revisarlas
                for (int d = 0; d < dep; ++d)
                {
                    srv_dep = servicios[srv].getDependencia(d);
                    //si una de las dependencias no se cumple en la máquina, descartar la máquina
                    if (servicio_vecindario[srv_dep][vec] == 0) insert_ok = false;
                }
            }//else, servicio no tiene dependencias, carry on

            //Fin de las comprobaciones
            if (insert_ok)
            {
                //nada impide la inserción, a insertar entonces
                asignacion_procesos[p] = m_id;

                //restar al espacio disponible, en estructura
                for (int r = 0; r < recursos_num; ++r)
                    espacio_disponible[m_id][r] -= procesos[p].getUsoRecursos(r);
                //y en máquina
                maq_i->agregar_proceso(procesos[p]);

                //set bit de servicio máquina
                servicio_maquina[srv][m_id] = true;

                //sumar loc, vec, spread
                servicio_localizacion[srv][loc] += 1;
                servicio_vecindario[srv][vec] += 1;
                spread_servicios[srv] += 1;

                //lista la agregación, pasar al siguiente proceso
                break;
            }
            //si no hay insert_ok, entonces probar con la siguiente máquina
            //continue;
        }//endfor(maquinas)

        //se revisaron todas las máquinas
        if (!insert_ok) //después de ver todas las máquinas no metí el proceso en ningún lado
        {
            procesos_cola.push_back(procesos[p]);
        }
        //seguir con el siguiente proceso; ordenar la lista de máquinas
        maquinas_sort.sort();
        //continue;
    }//endfor(procesos)

    //revisar cola de procesos no agregados
    int p;
    size_t size_cola = procesos_cola.size();
    while (!procesos_cola.empty())
    {
        for(proc_i = procesos_cola.begin(); proc_i != procesos_cola.end(); ++proc_i)
        {
            //obtener id del proceso
            p = proc_i->getId();
            //servicio del proceso (boilerplate, sorry)
            srv = procesos[p].getServicio();
            //dependencias del proceso
            dep = servicios[srv].getDependenciaNum();
            //revisar las máquinas ordenadas
            for(maq_i = maquinas_sort.begin(); maq_i != maquinas_sort.end(); ++maq_i)
            {
                //obtener id de la máquina, localización, vecindario
                m_id = maq_i->getId();
                loc = maquinas[m_id].getLocation();
                vec = maquinas[m_id].getNeighborhood();

                //Revisión de restricciones:
                insert_ok = true;
                //Capacidad (no cuenta el uso transitorio)
                for(int r = 0; r < recursos_num; ++r)
                {
                    //para cada recurso, ver si hay suficiente espacio en la máquina
                    if(espacio_disponible[m_id][r] < procesos[p].getUsoRecursos(r)) insert_ok = false;
                }

                //Conflicto de servicios
                if (servicio_maquina[srv][m_id]) insert_ok = false;

                //Conflicto de localizaciones (según spreadmin)
                if(spread_servicios[srv] < servicios[srv].getSpreadmin())
                {
                    //servicio no cumple con spreadmin
                    //si ya hay un proceso en la localización, buscar otra máquina
                    if (servicio_localizacion[srv][loc] > 0) insert_ok = false;
                }//else, servicio cumple con spreadmin, carry on

                //Conflicto de vecindarios (según dependencia)
                if (dep > 0)
                {
                    //Servicio tiene dependencias, revisarlas
                    for (int d = 0; d < dep; ++d)
                    {
                        srv_dep = servicios[srv].getDependencia(d);
                        //si una de las dependencias no se cumple en la máquina, descartar la máquina
                        if (servicio_vecindario[srv_dep][vec] == 0) insert_ok = false;
                    }
                }//else, servicio no tiene dependencias, carry on

                //Fin de las comprobaciones
                if (insert_ok)
                {
                    //nada impide la inserción, a insertar entonces
                    asignacion_procesos[p] = m_id;

                    //restar al espacio disponible, en estructura
                    for (int r = 0; r < recursos_num; ++r)
                        espacio_disponible[m_id][r] -= procesos[p].getUsoRecursos(r);
                    //y en máquina
                    maq_i->agregar_proceso(procesos[p]);

                    //set bit de servicio máquina
                    servicio_maquina[srv][m_id] = true;

                    //sumar loc, vec, spread
                    servicio_localizacion[srv][loc] += 1;
                    servicio_vecindario[srv][vec] += 1;
                    spread_servicios[srv] += 1;

                    //lista la agregación: eliminar de la cola y pasar al siguiente
                    procesos_cola.erase(proc_i);
                    break;
                }
                //si no hay insert_ok, entonces probar con la siguiente máquina
                //continue;
            }//endfor(maquinas)
            //si no se agregó el proceso, seguirá en la cola nomás, por ahora
        }//endfor(procesos)
        //para asegurarse que esto sirve para algo, hay que ver si disminuye
        //la cantidad de procesos en la cola
        if (size_cola == procesos_cola.size())
        {
            //esto es inútil
            break;
        }
        else
        {
            //intentamos nuevamente
            size_cola = procesos_cola.size();
        }
        //a estas alturas debiera no importar mucho el orden de las máquinas
        //si hay demoras en este paso, comentar el comando
        maquinas_sort.sort();
    }//endwhile(cola vacía)

    //fin del greedy
    if (!procesos_cola.empty()) insert_ok = false;
    else
    {
        insert_ok = true;
        //necesito asegurarme que asigné todo, más vale prevenir(?)
        for (int p = 0; p < procesos_num; ++p)
        {
            if (asignacion_procesos[p] == -1) insert_ok = false;
            break;
        }
    }

    //true si está todo hecho, false si no se pudo agregar algo
    return insert_ok;
}





