#include "solucion.h"
#include <list>
#include <iterator>

solucion::solucion()
{
    costoMovimientoMaquinas = NULL;
    balances_num = 0;
    balances = NULL;

    peso_balance = 0;
    peso_proceso = 0;
    peso_servicio = 0;
    peso_maquina = 0;

    recursos_num = 0;
    peso_recursos = NULL;
    recursos_trans = NULL;

    procesos_num = 0;
    servicios_num = 0;
    maquinas_num = 0;
    localizaciones_num = 0;
    vecindarios_num = 0;

    procesos = NULL;
    maquinas = NULL;
    servicios = NULL;

    asignacion_procesos = vector<int>();
    espacio_disponible = vector<vector<int>>();
    servicio_maquina = vector<vector<bool>>();
    servicio_localizacion = vector<vector<int>>();
    servicio_vecindario = vector<vector<int>>();
    spread_servicios = vector<int>();

    costo_solucion = 0;

    //don't even try
    sol_inicial = NULL;
}



/* constructor de copia de solución
 * sol es la solución inicial
 * mantener_inicial es por si acaso no necesitara mantener
 * la solución inicial para calcular algo
 */
solucion::solucion(const solucion& sol, bool mantener_inicial)
{
    cout << "En constructor de solución inicial" << endl;
    //Referencia a la solución inicial
    if (mantener_inicial) sol_inicial = &sol;

    cout << "Copiando referencias" << endl;

    //costos
    costoMovimientoMaquinas = sol.costoMovimientoMaquinas;
    balances_num = sol.balances_num;
    balances = sol.balances;

    //pesos
    peso_balance = sol.peso_balance;
    peso_proceso = sol.peso_proceso;
    peso_servicio = sol.peso_servicio;
    peso_maquina = sol.peso_maquina;

    //recursos
    recursos_num = sol.recursos_num;
    peso_recursos = sol.peso_recursos;
    recursos_trans = sol.recursos_trans;

    cout << "Inicializando cantidades y objetos" << endl;

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

void solucion::setBalanceTriple(vector<vector<int>>* bals)
{
    balances = bals;
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

void solucion::setParametrosRecursos(int num, vector<int>* peso, vector<bool>* trans)
{
    recursos_num = num;
    peso_recursos = peso;
    recursos_trans = trans;
}

void solucion::setCMM(vector<vector<int>>* cmm)
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
}

void solucion::setObjetos(vector<proceso>* procs, vector<maquina>* maqs, vector<servicio>* srvs)
{
    procesos = procs;
    maquinas = maqs;
    servicios = srvs;
}

int solucion::llenarSolucion(ifstream& fsol)
{
    int maq, srv, loc, vec;
    string line;
    vector<int> auxvi;
    vector<bool> auxvb;

    vector<maquina>::iterator im;
    vector<proceso>::iterator ip;

    cout << "Rellenando espacio disponible en las máquinas" << endl;

    //obteniendo espacio de las máquinas
    auxvi.clear();
    for(im = maquinas->begin(); im != maquinas->end(); ++im)
    {
        espacio_disponible.push_back(im->getEspacioMaxVector());
    }

    cout << "Preparando estructuras de datos" << endl;
    servicio_maquina.resize(servicios_num, vector<bool>(maquinas_num, false));
    servicio_localizacion.resize(servicios_num, vector<int>(localizaciones_num, 0));
    servicio_vecindario.resize(servicios_num, vector<int>(vecindarios_num, 0));
    spread_servicios.resize(servicios_num, 0);

    cout << "Leyendo solución inicial" << endl;

    //leyendo la solución inicial
    for(ip = procesos->begin(); ip != procesos->end(); ++ip)
    {
        fsol >> line;
        maq = stoi(line);

        //cada número es la máquina a la que está asignado un proceso (maq)
        asignacion_procesos.push_back(maq);

        //restar al espacio disponible
        for (int r = 0; r < recursos_num; ++r)
        {
        	espacio_disponible.at(maq).at(r) -= ip->getUsoRecursosVector().at(r);
        }

        //servicio en la máquina
        srv = ip->getServicio();
        servicio_maquina.at(srv).at(maq) = true;

        //sumar servicio en la localización y en el vecindario
        loc = maquinas->at(maq).getLocation();
        vec = maquinas->at(maq).getNeighborhood();
        servicio_localizacion.at(srv).at(loc) += 1;
        servicio_vecindario.at(srv).at(vec) += 1;

        //spread factor
        spread_servicios.at(srv) += 1;

    }

    //Revisión
    cout << "Vector de procesos vs máqunas:" << endl;
    for (int p = 0; p < procesos_num; ++p) cout << asignacion_procesos.at(p) << ' ';
    cout << endl;
    cout << "Espacio disponible por máquina:" << endl;
    for (int m = 0; m < maquinas_num; ++m)
    {
        cout << "Maq " << m << ": ";
        for (int r = 0; r < recursos_num; ++r)
        {
            cout << espacio_disponible.at(m).at(r) << " ";
        }
        cout << endl;
    }
    cout << "Matriz srv/maq:" << endl;
    for (int s = 0; s < servicios_num; ++s)
    {
        for (int m = 0; m < maquinas_num; ++m)
        {
            cout << (int)servicio_maquina.at(s).at(m) << " ";
        }
        cout << endl;
    }
    cout << "Matriz srv/loc:" << endl;
    for (int s = 0; s < servicios_num; ++s)
    {
        for (int l = 0; l < localizaciones_num; ++l)
        {
            cout << servicio_localizacion.at(s).at(l) << " ";
        }
        cout << endl;
    }
    cout << "Matriz srv/vec:" << endl;
    for (int s = 0; s < servicios_num; ++s)
    {
        for (int v = 0; v < vecindarios_num; ++v)
        {
            cout << servicio_vecindario.at(s).at(v) << " ";
        }
        cout << endl;
    }
    cout << "Spread de los servicios:" << endl;
    for (int s = 0; s < servicios_num; ++s)
    {
        cout << spread_servicios.at(s) << " ";
    }
    cout << endl << endl;

    //done
    return 0;
}

//greedy
bool solucion::llenarSolucion()
{
	cout << "Preparando estructuras de datos" << endl;
	asignacion_procesos.resize(procesos_num, -1);
	servicio_maquina.resize(servicios_num, vector<bool>(maquinas_num, false));
	servicio_localizacion.resize(servicios_num, vector<int>(localizaciones_num, 0));
	servicio_vecindario.resize(servicios_num, vector<int>(vecindarios_num, 0));
	spread_servicios.resize(servicios_num, 0);

    cout << "Llenando lista ordenable de máquinas" << endl;

    //copiar máquinas
    list<maquina> maquinas_sort;
    for (int m = 0; m < maquinas_num; ++m)
    {
        maquinas_sort.push_back(maquinas->at(m));
    }
    list<maquina>::iterator maq_i;
    vector<int> auxvec;
    cout << "Obteniendo espacio disponible" << endl;

    //llenar matriz de espacio disponible en las máquinas
    maq_i = maquinas_sort.begin();
    for(vector<maquina>::iterator im = maquinas->begin(); im != maquinas->end(); ++im)
    {
    	auxvec = im->getEspacioMaxVector();
        for(int r = 0; r < recursos_num; ++r)
        {
            //manejo de recursos transitivos
            if (recursos_trans->at(r))
            {
                //si el recurso es transitivo, está ocupado por el movimiento anterior
                //basta con igualarlo en esta parte
            	auxvec.at(r) = sol_inicial->espacio_disponible.at(im->getId()).at(r);

                //hasta ahora, maquinas_sort está ordenado por id
                //así que se puede acceder directamente (en teoría)
                maq_i->setCapacidadUtilizada(r, (maquinas->at(im->getId()).getEspacioMax(r) - auxvec.at(r)));
            }
        }
        //ingresar espacio disponible en la máquina
        espacio_disponible.push_back(auxvec);
        //avanzar el iterador
        if (maq_i != maquinas_sort.end()) advance(maq_i, 1);
    }

    cout << "Primer ordenamiento lista de máquinas" << endl;

    //ahora se ordena la lista
    //de ahora en adelante no se asegura que los id de las máquinas sean correlativos
    maquinas_sort.sort();

    //Ordenando servicios
    cout << "Ordenamiento de servicios" << endl;
    list<servicio> servicios_sort;
    for (size_t s = 0; s < servicios->size(); ++s)
    {
    	servicios_sort.push_back(servicios->at(s));
    }
    //Servicios se ordenan en reversa (mayor a menor prioridad)
    servicios_sort.sort();
    servicios_sort.reverse();

    //Cola de procesos
    list<proceso> procesos_cola;
    list<proceso>::iterator proc_i;

    //llenando la cola de procesos por prioridad de servicios
    //(esto se encarga de restricciones de servicio, spreadmin y dependencias)
    for (vector<servicio>::iterator is = servicios->begin(); is != servicios->end(); ++is)
    {
    	auxvec.clear();
    	auxvec = is->getListaProcesos();
    	for (size_t p = 0; p < auxvec.size(); ++p)
    	{
    		procesos_cola.push_back(procesos->at(auxvec.at(p)));
    	}
    }

    //me aseguro que agregué todos (todos) los procesos a la cola
    size_t proc = procesos->size();
    size_t proc_c = procesos_cola.size();
    if (proc != proc_c)
    {
    	cout << "Error al agregar los procesos a la cola!" << endl;
    	return -1;
    }

    //en este punto las máquinas están vacías, hay que empezar a llenarlas
    //la lista se ordena después de cada inserción (al final del loop principal)
    //agregar a la primera máquina que se pueda (esto se encarga de restricciones de espacio)

    bool insert_ok;         //flag de inserción posible
    int m_id;               //id de la máquina
    int srv, dep, srv_dep;  //parámetros del proceso
    int loc, vec;           //parámetros de la máquina
    int p;					//id del proceso
    size_t size_cola = procesos_cola.size();	//tamaño de la cola

    cout << "Revisando procesos" << endl;

    //loop principal
    while (!procesos_cola.empty())
    {
    	//nota: el iterador no avanza con el for, porque es dependiente de si el objeto se borra
        for(proc_i = procesos_cola.begin(); proc_i != procesos_cola.end();)
        {
            //obtener id del proceso
            p = proc_i->getId();
            //servicio del proceso
            srv = procesos->at(p).getServicio();
            //dependencias del proceso
            dep = servicios->at(srv).getDependenciaNum();
            //revisar las máquinas ordenadas
            for(maq_i = maquinas_sort.begin(); maq_i != maquinas_sort.end(); ++maq_i)
            {
                //obtener id de la máquina, localización, vecindario
                m_id = maq_i->getId();
                loc = maquinas->at(m_id).getLocation();
                vec = maquinas->at(m_id).getNeighborhood();

                //Revisión de restricciones:
                insert_ok = true;
                //Capacidad (no cuenta el uso transitorio)
                for(int r = 0; r < recursos_num; ++r)
                {
                    //para cada recurso, ver si hay suficiente espacio en la máquina
                    if(espacio_disponible.at(m_id).at(r) < procesos->at(p).getUsoRecursos(r))
                    {
                        cout << "P" << p << "en M" << m_id << " rechazo por falta de recursos" << endl;
                        insert_ok = false;
                        break;
                    }
                }

                //Conflicto de servicios
                if (servicio_maquina.at(srv).at(m_id))
                {
                    cout << "P" << p << "en M" << m_id << " rechazo por colisión de servicio" << endl;
                    insert_ok = false;
                }

                //Conflicto de localizaciones (según spreadmin)
                if(spread_servicios.at(srv) < servicios->at(srv).getSpreadmin())
                {
                    //servicio no cumple con spreadmin
                    //si ya hay un proceso en la localización, buscar otra máquina
                    if (servicio_localizacion.at(srv).at(loc) > 0)
                    {
                        cout << "P" << p << "en M" << m_id << " rechazo por falta de spread" << endl;
                        insert_ok = false;
                    }
                }//else, servicio cumple con spreadmin, carry on

                //Conflicto de vecindarios (según dependencia)
                if (dep > 0)
                {
                    //Servicio tiene dependencias, revisarlas
                    for (int d = 0; d < dep; ++d)
                    {
                        srv_dep = servicios->at(srv).getDependencia(d);
                        //si una de las dependencias no se cumple en la máquina, descartar la máquina
                        if (servicio_vecindario.at(srv_dep).at(vec) == 0)
                        {
                            cout << "P" << p << "en M" << m_id << " rechazo por falta de dependencia" << endl;
                            insert_ok = false;
                        }
                    }
                }//else, servicio no tiene dependencias, carry on

                //Fin de las comprobaciones
                if (insert_ok)
                {
                    //nada impide la inserción, a insertar entonces
                    asignacion_procesos.at(p) = m_id;

                    //restar al espacio disponible, en estructura
                    for (int r = 0; r < recursos_num; ++r)
                        espacio_disponible.at(m_id).at(r) -= procesos->at(p).getUsoRecursos(r);
                    //y en máquina
                    maq_i->agregar_proceso(procesos->at(p));

                    //set bit de servicio máquina
                    servicio_maquina.at(srv).at(m_id) = true;

                    //sumar loc, vec, spread
                    servicio_localizacion.at(srv).at(loc) += 1;
                    servicio_vecindario.at(srv).at(vec) += 1;
                    spread_servicios.at(srv) += 1;

                    //lista la agregación: eliminar de la cola y pasar al siguiente
                    cout << "P" << p << "en M" << m_id << " ingresa correctamente" << endl;
                    cout << "Eliminando P" << p << " de la cola" << endl;
                    //procesos_cola.erase devuelve el siguiente iterador
                    proc_i = procesos_cola.erase(proc_i);
                    break;
                }
                //si no hay insert_ok, entonces probar con la siguiente máquina
                //continue;
            }//endfor(maquinas)

            //si no se agregó el proceso, seguirá en la cola nomás, por ahora
            if (!insert_ok)
            {
            	//avanzar el iterador
            	++proc_i;
            }
        }//endfor(procesos)

        //para asegurarse que esto sirve para algo, hay que ver si disminuye
        //la cantidad de procesos en la cola
        if (size_cola == procesos_cola.size())
        {
            cout << "resistance is futile (nada se agregó)" << endl;
            break;
        }
        else
        {
            //intentamos nuevamente
            cout << "Revisando si quedan procesos" << endl;
            size_cola = procesos_cola.size();
        }
        //ordenar máquinas
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
            cout << "Revisando asignacion de procesos" << endl;
            if (asignacion_procesos.at(p) == -1)
            {
                cout << "Proceso " << p << " quedó sin asignación!!" << endl;
                insert_ok = false;
                break;
            }
        }
        if (insert_ok) cout << "Todos los procesos asignados." << endl;
    }

    cout << "Fin del algoritmo greedy." << endl;
    cout << endl << "Resumen de las estructuras de datos:" << endl;

    cout << "Vector de procesos vs máqunas:" << endl;
	for (int p = 0; p < procesos_num; ++p) cout << asignacion_procesos.at(p) << ' ';
	cout << endl;
	cout << "Espacio disponible por máquina:" << endl;
	for (int m = 0; m < maquinas_num; ++m)
	{
		cout << "Maq " << m << ": ";
		for (int r = 0; r < recursos_num; ++r)
		{
			cout << espacio_disponible.at(m).at(r) << " ";
		}
		cout << endl;
	}
	cout << "Matriz srv/maq:" << endl;
	for (int s = 0; s < servicios_num; ++s)
	{
		for (int m = 0; m < maquinas_num; ++m)
		{
			cout << (int)servicio_maquina.at(s).at(m) << " ";
		}
		cout << endl;
	}
	cout << "Matriz srv/loc:" << endl;
	for (int s = 0; s < servicios_num; ++s)
	{
		for (int l = 0; l < localizaciones_num; ++l)
		{
			cout << servicio_localizacion.at(s).at(l) << " ";
		}
		cout << endl;
	}
	cout << "Matriz srv/vec:" << endl;
	for (int s = 0; s < servicios_num; ++s)
	{
		for (int v = 0; v < vecindarios_num; ++v)
		{
			cout << servicio_vecindario.at(s).at(v) << " ";
		}
		cout << endl;
	}
	cout << "Spread de los servicios:" << endl;
	for (int s = 0; s < servicios_num; ++s)
	{
		cout << spread_servicios.at(s) << " ";
	}
	cout << endl << endl;

    //true si está todo hecho, false si no se pudo agregar algo
    return insert_ok;
}





