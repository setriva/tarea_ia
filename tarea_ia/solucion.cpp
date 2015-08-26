#include "solucion.h"
#include <list>
#include <iterator>

solucion::solucion()
{
	costoMovimientoMaquinas = NULL;
    balances_num = 0;
    balances = NULL;

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
    //espacio_disponible = vector<vector<int>>();
    uso_procesos = vector<vector<int>>();
    uso_transitivo = vector<vector<int>>();
    servicio_maquina = vector<vector<bool>>();
    servicio_localizacion = vector<vector<int>>();
    servicio_vecindario = vector<vector<int>>();
    spread_servicios = vector<int>();

    costo_solucion = 0;
    tam_busqueda = 0;
    temperatura = 0;

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

void solucion::calcularCostoSolucion()
{
    costo_solucion = 0;

    //Temps
    int costo_carga = 0;
    int costo_balance = 0;
    int costo_movproceso = 0;
    int costo_movservicio = 0;
    int costo_movmaquina = 0;

    //Costo de carga: cuánto se pasa la máquina de su safety cost
    //recordar ponderar por cada recurso

    vector<int> exceso_carga(recursos_num);	//exceso TOTAL de carga
    vector<int> uvec(recursos_num);	//espacio utilizado
    vector<int> svec(recursos_num);	//capacidad de seguridad
    vector<int> exc(recursos_num);	//espacio máximo
    int bal, r1, r2, ratio;			//parámetros de balanceo
    for (vector<maquina>::iterator im = maquinas->begin(); im != maquinas->end(); ++im)
    {
    	//cálculo del costo de carga
    	//uvec = im->getEspacioMaxVector();
    	svec = im->getEspacioSafeVector();
    	for (int r = 0; r < recursos_num; ++r)
    	{
    		//sumar uso recursos
    		uvec.at(r) = uso_procesos.at(im->getId()).at(r);
    		//sumar transitivos
    		uvec.at(r) += uso_transitivo.at(im->getId()).at(r);
    		//restar espacio utilizado y capacidad segura
    		exc.at(r) = uvec.at(r) - svec.at(r);
    		//si es menor a 0, que sea 0
    		if (exc.at(r) < 0) exc.at(r) = 0;
    		//agregar al exceso de carga
    		exceso_carga.at(r) += exc.at(r);
    	}

    	//cálculo del costo de balance
    	//cuánto se pasa el recurso R2 de ratio veces el recurso R1
    	for (int b = 0; b < balances_num; ++b)
    	{
    		//leer trío de balance
    		r1 = balances->at(b).at(0);
    		r2 = balances->at(b).at(1);
    		ratio = balances->at(b).at(2);
    		//calcular balance
    		bal = ((uvec.at(r1) * ratio) - uvec.at(r2));
    		//sumar ponderado
    		costo_balance += (balances->at(b).at(3) * bal);
    	}
    }

    //ponderar costo de carga
    for (int r = 0; r < recursos_num; ++r)
    {
    	costo_carga += (exceso_carga.at(r) * peso_recursos->at(r));
    }

    //sumar costo carga ponderado
    costo_solucion += costo_carga;

    //sumar costo balance ponderado
    costo_solucion += costo_balance;

    //Costo movimiento procesos: suma de ip->getCosteMovimiento de asignaciones distintas a sol_inicial
    //Costo movimiento servicio: max(procesos movidos de un servicio)
    //Costo movimiento máquinas: sumar: cmm[sol_inicial->asignación][this->asignación] para cada proceso
    vector<int> srv_mov(servicios_num, 0);
    int mi, mf;		//maquina inicial, maquina final
    for (int p = 0; p < procesos_num; ++p)
    {
    	mi = sol_inicial->asignacion_procesos.at(p);
    	mf = this->asignacion_procesos.at(p);
    	if (mi != mf)
    	{
    		//sumar costes de movimiento
    		costo_movproceso += procesos->at(p).getCosteMovimiento();
    		//contar servicio con movimiento
    		srv_mov.at(procesos->at(p).getServicio()) += 1;
    		//sumar coste de movimiento de máquina
    		costo_movmaquina += costoMovimientoMaquinas->at(mi).at(mf);
    	}
    }

    //sumar costo movimiento proceso ponderado
    costo_solucion += (costo_movproceso * peso_proceso);

    //anotar servicio con más procesos movidos
	for (vector<int>::iterator is = srv_mov.begin(); is != srv_mov.end(); ++is)
	{
		if (costo_movservicio < *is) costo_movservicio = *is;
	}

	//sumar costo movimiento servicio ponderado
    costo_solucion += (costo_movservicio * peso_servicio);

    //sumar costo movimiento maquina ponderado
    costo_solucion += (costo_movmaquina * peso_maquina);
}

long solucion::getCostoSolucion()
{
	return costo_solucion;
}

vector<int> solucion::getAsignacion()
{
	return asignacion_procesos;
}

void solucion::setBalanceTriple(vector<vector<int>>* bals)
{
    balances = bals;
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

    //espacio requerido siempre:
    uso_transitivo.resize(maquinas_num, vector<int>(recursos_num, 0));
}

void solucion::setObjetos(vector<proceso>* procs, vector<maquina>* maqs, vector<servicio>* srvs)
{
    procesos = procs;
    maquinas = maqs;
    servicios = srvs;
}

void solucion::setTemperatura(long double temp)
{
	temperatura = temp;
}

void solucion::setTamBusqueda(int tam)
{
	tam_busqueda = tam;
}

bool solucion::ordenProcesoPrioridad(const proceso &left, const proceso &right) const
{
	return (left.getPrioridad() < right.getPrioridad());
}

bool solucion::ordenProcesoPeso(const proceso &left, const proceso &right) const
{
	return (left.getPeso() < right.getPeso());
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
    /*for(im = maquinas->begin(); im != maquinas->end(); ++im)
    {
        espacio_disponible.push_back(im->getEspacioMaxVector());
    }*/

    cout << "Preparando estructuras de datos" << endl;
    uso_procesos.resize(maquinas_num, vector<int>(recursos_num, 0));
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

        //sumar al uso de recursos y restar al espacio disponible
        for (int r = 0; r < recursos_num; ++r)
        {
        	//sumar a uso procesos
        	uso_procesos.at(maq).at(r) += ip->getUsoRecursosVector().at(r);
        	//restar al espacio disponible
        	//espacio_disponible.at(maq).at(r) -= ip->getUsoRecursosVector().at(r);
        }

        //servicio en la máquina
        srv = ip->getServicio();
        servicio_maquina.at(srv).at(maq) = true;

        //sumar servicio en la localización y en el vecindario
        loc = maquinas->at(maq).getLocation();
        vec = maquinas->at(maq).getNeighborhood();
        servicio_localizacion.at(srv).at(loc) += 1;
        servicio_vecindario.at(srv).at(vec) += 1;

        //spread factor de la localización
        //spread_servicios.at(srv) += 1;
    }
    //rellenar el spread de los servicios (cant. localizaciones por servicio)
    for (vector<servicio>::iterator is = servicios->begin(); is != servicios->end(); ++is)
    {
    	vector<int> procs = is->getListaProcesos();
    	list<int> locs;
    	for (auto ip = procs.begin(); ip != procs.end(); ++ip)
    	{
    		locs.push_back(maquinas->at(asignacion_procesos.at(*ip)).getLocation());
    	}
    	//quedarse sólo con los índices únicos
    	locs.sort(); locs.unique();
    	//guardar spread
    	spread_servicios.at(is->getId()) = locs.size();
    }

    //Revisión
    /*cout << "Vector de procesos vs máqunas:" << endl;
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
    }*/
    /*
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
    }*/
    cout << "Spread de los servicios:" << endl;
    for (int s = 0; s < servicios_num; ++s)
    {
        cout << spread_servicios.at(s) << " ";
    }
    cout << endl << endl;

    //done
    return 0;
}

//greedy (DISABLED)
bool solucion::llenarSolucion()
{
	cout << "Greedy está deshabilitado" << endl << endl;
	return false;

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
    //obtener uso de recursos transitivos: ese no cambia
    uso_transitivo = sol_inicial->uso_transitivo;
    //llenar espacio disponible
    /*for(vector<maquina>::iterator im = maquinas->begin(); im != maquinas->end(); ++im)
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
                //así que se puede acceder directamente
                maq_i->setCapacidadUtilizada(r, (maquinas->at(im->getId()).getEspacioMax(r) - auxvec.at(r)));
            }
        }
        //ingresar espacio disponible en la máquina
        espacio_disponible.push_back(auxvec);
        //avanzar el iterador
        if (maq_i != maquinas_sort.end()) advance(maq_i, 1);
    }*/

    cout << "Primer ordenamiento lista de máquinas" << endl;

    //ahora se ordena la lista
    //de ahora en adelante no se asegura que los id de las máquinas sean correlativos
    maquinas_sort.sort();

    //Cola de procesos
    list<proceso> procesos_cola;
    list<proceso>::iterator proc_i;

    //agregar todos los procesos a la cola
    for (size_t p = 0; p < procesos->size(); ++p)
    {
    	procesos_cola.push_back(procesos->at(p));
    }

    //cada proceso tiene una prioridad (dada por el servicio al que pertenece) y un peso
    //el orden en que ingresan los procesos a las máquinas debe ser por prioridad y por peso
    //se ordena primero por peso y después por prioridad; de forma que dentro de los procesos
    //con igual prioridad, vayan los más pesados primero

    procesos_cola.reverse();

    //del más ligero al más pesado
    procesos_cola.sort([](const proceso& l, const proceso& r){return l.getPeso() < r.getPeso();});

    //del menos prioritario al más prioritario
    procesos_cola.sort([](const proceso& l, const proceso& r){return l.getPrioridad() < r.getPrioridad();});

    //la lista va del más pesado y más prioritario hacia abajo
    procesos_cola.reverse();

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
                    if(maquinas->at(m_id).getEspacioMax(r) - uso_procesos.at(m_id).at(r) - uso_transitivo.at(m_id).at(r)
                    		< procesos->at(p).getUsoRecursos(r))
                    {
                        cout << "P" << p << " en M" << m_id << " rechazo por falta de recursos" << endl;
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
                    //for (int r = 0; r < recursos_num; ++r)
                    //    espacio_disponible.at(m_id).at(r) -= procesos->at(p).getUsoRecursos(r);
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
                    //ordenar máquinas
                    cout << "Reordenando máquinas" << endl;
                    maquinas_sort.sort();
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

    }//endwhile(cola vacía)

    //fin del greedy
    if (!procesos_cola.empty()) insert_ok = false;
    else
    {
        insert_ok = true;
        //asegurándose de la asignación
        cout << "Revisando asignación de procesos" << endl;
        for (int p = 0; p < procesos_num; ++p)
        {
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
    /*cout << endl << "Resumen de las estructuras de datos:" << endl;

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
	}*/
	/*
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
	*/
	/*cout << "Spread de los servicios:" << endl;
	for (int s = 0; s < servicios_num; ++s)
	{
		cout << spread_servicios.at(s) << " ";
	}
	cout << endl << endl;
	 */
    //true si están todos los procesos asignados, false si no se pudo agregar algo
    return insert_ok;
}

void solucion::descartarGreedy()
{
	asignacion_procesos = sol_inicial->asignacion_procesos;
	spread_servicios = sol_inicial->spread_servicios;
	uso_procesos = sol_inicial->uso_procesos;
	//uso_transitivo = sol_inicial->uso_transitivo;
	//espacio_disponible = sol_inicial->espacio_disponible;
	servicio_maquina = sol_inicial->servicio_maquina;
	servicio_localizacion = sol_inicial->servicio_localizacion;
	servicio_vecindario = sol_inicial->servicio_vecindario;
}

bool solucion::move()
{
	//variables
	bool moved = false;
	uniform_int_distribution<int> proc_dado(0, (procesos_num-1));
	uniform_int_distribution<int> maq_dado(0, (maquinas_num-1));
	//Dado
	extern std::mt19937 generador;

	//escoger un proceso proc en una máquina m0
	int proc_id = proc_dado(generador);
	int maq0_id = asignacion_procesos.at(proc_id);
	int serv_id = procesos->at(proc_id).getServicio();
	int spreadmin = servicios->at(serv_id).getSpreadmin();
	vector<int> recursos_proc = procesos->at(proc_id).getUsoRecursosVector();
	int loc0_id = maquinas->at(maq0_id).getLocation();
	int vec0_id = maquinas->at(maq0_id).getNeighborhood();

	//escoger máquina random maq, que no sea m0
	int maq_id = maq_dado(generador);
	while(maq_id == maq0_id) maq_id = maq_dado(generador);


	//parámetros para rollback
	//int rbm, rbloc_id, rbvec_id;
	vector<vector<int>> rbuso_procesos, rbserv_localizacion, rbserv_vecindario;
	vector<vector<bool>> rbserv_maquina;
	vector<int> rbasign_procesos;

	//revisando maq y las tam_busqueda máqs siguientes
	for(int m = maq_id; m < maquinas_num && m < (maq_id + tam_busqueda); ++m)
	{
		//si m es la máquina inicial, descartar
		if (m == maq0_id) continue;

		//revisar si proc cabe en m
		bool entra = true;
		for (int r = 0; r < recursos_num; ++r)
		{
			int espacioMax = maquinas->at(m).getEspacioMax(r);
			int usoProcesos = uso_procesos.at(m).at(r);
			int usoTransitivo = uso_transitivo.at(m).at(r);
			int recursoProc = recursos_proc.at(r);

			if((espacioMax - usoProcesos - usoTransitivo) < recursoProc) entra = false;
		}
		//revisar colisión de servicios
		if (servicio_maquina.at(serv_id).at(m)) entra = false;

		//si hay dramas de entrada, probar otra máquina
		if (!entra) continue;

		//revisar si proc puede salir de maq0
		bool sale = true;
		//si las máquinas son de distinta loc, revisar spreadmin
		int loc_id = maquinas->at(m).getLocation();
		if (loc0_id != loc_id)
		{
			if (spread_servicios.at(serv_id) <= spreadmin)
				sale = false;
		}

		//estos son los servicios de los que depende serv_id
		vector<int> dependencias = servicios->at(serv_id).getDependenciaVector();
		//y estos son los servicios que dependen de serv_id
		vector<int> dependientes = servicios->at(serv_id).getDependientes();
		//si queda más de un proceso del servicio en el vecindario, puedo moverme
		//si no hay procesos del servicio en el vecindario, algo anda muy mal
		//si queda uno, tengo que asegurarme que donde lo mueva estén las dependencias
		int vec_id = maquinas->at(m).getNeighborhood();
		if (servicio_vecindario.at(serv_id).at(vec0_id) == 1)
		{
			//revisar todas las dependencias
			for (auto idep = dependencias.begin(); idep != dependencias.end(); ++idep)
			{
				//si no hay procesos de la dependencia en el vecindario destino, fallar
				if(servicio_vecindario.at(*idep).at(vec_id) <= 0)
				{
					sale = false;
					break;
				}
			}

			//revisar dependientes
			for (auto idep = dependientes.begin(); idep != dependientes.end(); ++idep)
			{
				//si no hay procesos de los dependientes en el vecindario destino, fallar
				//la alternativa (encadenar dependientes) no vale la pena intentarla
				if(servicio_vecindario.at(*idep).at(vec_id) <= 0)
				{
					sale = false;
					break;
				}
			}
		}

		//si hay dramas de salida, probar otra máquina
		if (!sale) continue;

		//seguimos acá, se puede entonces sacar el proceso de la maq0 y meterlo a m
		//actualizar asignación, spread, espacio_disponible,
		//servicio_maquina, servicio_loc, servicio_vec
		if (entra && sale)
		{
			//sacar copia de las matrices
			rbuso_procesos = uso_procesos;
			rbasign_procesos = asignacion_procesos;
			rbserv_maquina = servicio_maquina;
			rbserv_localizacion = servicio_localizacion;
			rbserv_vecindario = servicio_vecindario;

			//cambiar uso de recursos
			for (int r = 0; r < recursos_num; ++r)
			{
				//quitar recurso en maq0 y sumar recurso en m
				uso_procesos.at(maq0_id).at(r) 	-= recursos_proc.at(r);
				uso_procesos.at(m).at(r)		+= recursos_proc.at(r);

				//insanity check
				if (uso_procesos.at(maq0_id).at(r) < 0 ||
					uso_procesos.at(m).at(r) > maquinas->at(m).getEspacioMax(r))
				{
					cout << "USO DE PROCESOS MENOR A CERO O SOBREPASO DE ESPACIO EN MAQUINA" << endl;
					return false;
				}

				//si el recurso es transitivo, se revisa asignación inicial para saber qué hacer
				if (recursos_trans->at(r))
				{
					//si la máquina origen es la de AI, el proceso sale de su AI: entra a uso transitivo
					if (maq0_id == sol_inicial->asignacion_procesos.at(proc_id))
					{
						uso_transitivo.at(maq0_id).at(r) += recursos_proc.at(r);
					}
					//si la máquina destino es la de AI, el proceso vuelve: sale de uso transitivo
					if (m == sol_inicial->asignacion_procesos.at(proc_id))
					{
						uso_transitivo.at(m).at(r) -= recursos_proc.at(r);
					}
				}

			}
			//cambiar asignación
			asignacion_procesos.at(proc_id) = m;
			//cambiar servicio_maquina
			servicio_maquina.at(serv_id).at(maq0_id) = false;
			servicio_maquina.at(serv_id).at(m) = true;
			//cambiar servicio_loc y calcular spread (si es que hubo cambio de localización)
			if (loc_id != loc0_id)
			{
				servicio_localizacion.at(serv_id).at(loc0_id) -= 1;
				servicio_localizacion.at(serv_id).at(loc_id) += 1;
				//recalcular spread: loc0 se quedó sin procesos de serv, luego bajó el spread del servicio
				if (servicio_localizacion.at(serv_id).at(loc0_id) == 0)
					spread_servicios.at(serv_id)--;
				//recalcular spread: loc ahora tiene su primer proceso de serv, luego subió el spread del servicio
				if (servicio_localizacion.at(serv_id).at(loc_id) == 1)
					spread_servicios.at(serv_id)++;

			}
			//cambiar servicio_vec
			if (vec_id != vec0_id)
			{
				servicio_vecindario.at(serv_id).at(vec0_id) -= 1;
				servicio_vecindario.at(serv_id).at(vec_id) += 1;
			}
			//hecho el cambio, decidimos si se movió la cosa
			//rbm = m;
			//rbloc_id = loc_id;
			//rbvec_id = vec_id;
			moved = true;
			break;
		}//endif
	}//endfor?

	//Revisión de costes y tirada de dado
	if (moved)
	{
		int costo_sol_anterior = costo_solucion;
		calcularCostoSolucion();
		//calcular diferencia
		int delta = costo_sol_anterior - costo_solucion;
		//si costo_solucion menor que costo_sol_anterior, quedarse
		if (delta > 0) return true;
		//si no, dado
		uniform_real_distribution<double> exp_dado(0, 1);
		double exp_eval = exp(delta/temperatura);
		//tirar dado
		double exp_exec_dado = exp_dado(generador);
		if (exp_exec_dado < exp_eval)
		{
			moved = true;
		}
		else
		{
			//rollback
			/*
			for (int r = 0; r < recursos_num; ++r)
			{
				uso_procesos.at(rbm).at(r) -= recursos_proc.at(r);
				uso_procesos.at(maq0_id).at(r) -= recursos_proc.at(r);

				if (recursos_trans->at(r))
				{
					//si la máquina origen es la de AI, el proceso sale de su AI: entra a uso transitivo
					if (rbm == sol_inicial->asignacion_procesos.at(proc_id))
					{
						uso_transitivo.at(rbm).at(r) += recursos_proc.at(r);
					}
					//si la máquina destino es la de AI, el proceso vuelve: sale de uso transitivo
					if (maq0_id == sol_inicial->asignacion_procesos.at(proc_id))
					{
						uso_transitivo.at(maq0_id).at(r) -= recursos_proc.at(r);
					}
				}
			}
			asignacion_procesos.at(proc_id) = maq0_id;

			servicio_maquina.at(serv_id).at(rbm) = false;
			servicio_maquina.at(serv_id).at(maq0_id) = true;

			if (rbloc_id != loc0_id)
			{
				servicio_localizacion.at(serv_id).at(rbloc_id) -= 1;
				servicio_localizacion.at(serv_id).at(loc0_id) += 1;
				//recalcular spread: loc0 se quedó sin procesos de serv, luego bajó el spread del servicio
				if (servicio_localizacion.at(serv_id).at(rbloc_id) == 0)
					spread_servicios.at(serv_id)--;
				//recalcular spread: loc ahora tiene su primer proceso de serv, luego subió el spread del servicio
				if (servicio_localizacion.at(serv_id).at(loc0_id) == 1)
					spread_servicios.at(serv_id)++;

			}
			//cambiar servicio_vec
			if (rbvec_id != vec0_id)
			{
				servicio_vecindario.at(serv_id).at(rbvec_id) -= 1;
				servicio_vecindario.at(serv_id).at(vec0_id) += 1;
			}*/

			//restaurar
			uso_procesos = rbuso_procesos;
			asignacion_procesos = rbasign_procesos;
			servicio_maquina = rbserv_maquina;
			servicio_localizacion = rbserv_localizacion;
			servicio_vecindario = rbserv_vecindario;
			moved = false;
			calcularCostoSolucion();
		}
	}

	//si se insertó algo, moved será true
	//si no hubo movimiento, moved será false
	return moved;
}

bool solucion::swap()
{
	//vars, dado
	bool moved = false;
	uniform_int_distribution<int> pizq_dado(0, (procesos_num-1));
	uniform_int_distribution<int> pder_dado(0, (procesos_num-1));
	extern std::mt19937 generador;

	//escoger proceso p_izq en máq m_izq
	int proc_izq = pizq_dado(generador);
	int maq_izq = asignacion_procesos.at(proc_izq);
	int serv_izq = procesos->at(proc_izq).getServicio();
	int spmin_izq = servicios->at(serv_izq).getSpreadmin();
	vector<int> rec_izq = procesos->at(proc_izq).getUsoRecursosVector();
	int loc_izq = maquinas->at(maq_izq).getLocation();
	int vec_izq = maquinas->at(maq_izq).getNeighborhood();

	//escoger otro proceso p_der en máq m_der
	//da igual que m_der = m_izq, eso se revisa en el loop de revisión
	int p = pder_dado(generador);

	//variables para hacer undo
	//int uproc_der, umaq_der, userv_der, uloc_der, uvec_der;
	//vector<int> urec_der;
	vector<vector<int>> rbuso_procesos, rbserv_localizacion, rbserv_vecindario;
	vector<vector<bool>> rbserv_maquina;
	vector<int> rbasign_procesos;

	//revisando si se puede hacer swap a los procesos (y a los tam_busqueda siguientes)
	for(int proc_der = p; proc_der < procesos_num && proc_der < (p + tam_busqueda); ++proc_der)
	{
		//obtener máquina destino
		int maq_der = asignacion_procesos.at(proc_der);
		//si es la máquina de la izquierda, descartar proceso
		if (maq_der == maq_izq) continue;

		//condiciones de peso
		bool rest_peso = true;
		vector<int> rec_der = procesos->at(proc_der).getUsoRecursosVector();
		for (int r = 0; r < recursos_num; ++r)
		{
			int maxEspacioIzq = maquinas->at(maq_izq).getEspacioMax(r);
			int maxEspacioDer = maquinas->at(maq_der).getEspacioMax(r);
			int usoProcesosIzq = uso_procesos.at(maq_izq).at(r);
			int usoProcesosDer = uso_procesos.at(maq_der).at(r);
			int usoTransitivoIzq = uso_transitivo.at(maq_izq).at(r);
			int usoTransitivoDer = uso_transitivo.at(maq_der).at(r);
			int recursoIzq = rec_izq.at(r);
			int recursoDer = rec_der.at(r);

			//revisar si rec_izq cabe en maq_der, eliminando rec_der:
			//espacio máx en máq. derecha - uso procesos reg/trans en máq. derecha + espacio que deja el proceso saliente a la derecha
			int espacioDisponibleDer = maxEspacioDer - usoProcesosDer - usoTransitivoDer;
			if (espacioDisponibleDer + recursoDer < recursoIzq)
			{
				rest_peso = false;
				break;
			}
			//revisar si rec_der cabe en maq_izq, eliminando rec_izq:
			//espacio máx en máq. izquierda - uso procesos reg/trans en máq. izquierda + espacio que deja el proceso saliente a la izquierda
			int espacioDisponibleIzq = maxEspacioIzq - usoProcesosIzq - usoTransitivoIzq;
			if (espacioDisponibleIzq + recursoIzq < recursoDer)
			{
				rest_peso = false;
				break;
			}
		}
		//si se pasa un peso, cambiar a otro proceso
		if (!rest_peso) continue;

		//revisar colisión de servicios cruzados
		bool rest_servs = true;
		int serv_der = procesos->at(proc_der).getServicio();
		//si un par servicio/máquina ya existe, cambiar a otro proceso...
		if (servicio_maquina.at(serv_izq).at(maq_der) || servicio_maquina.at(serv_der).at(maq_izq))
		{
			//a menos que los procesos sean del mismo servicio: si es así, pueden intercambiarse
			if (serv_izq == serv_der) rest_servs = true;
			else rest_servs = false;
		}
		if (!rest_servs) continue;

		//revisar restricciones de spreadmin
		bool rest_locs = true;
		int loc_der = maquinas->at(maq_der).getLocation();
		//si las dos máquinas son de distinta loc, revisar spreadmins
		if (loc_izq != loc_der)
		{
			//servicio izquierda
			if (spread_servicios.at(serv_izq) <= spmin_izq) rest_locs = false;
			//servicio derecha
			int spmin_der = servicios->at(serv_der).getSpreadmin();
			if (spread_servicios.at(serv_der) <= spmin_der) rest_locs = false;
		}
		if (!rest_locs) continue;

		//revisar restricciones de dependencia (oh dear)
		bool vecs = true;
		int vec_der = maquinas->at(maq_der).getNeighborhood();
		//parte 1: resolver dependencias de serv_izq; destino: vec_der
		vector<int> dependencias = servicios->at(serv_izq).getDependenciaVector();
		vector<int> dependientes = servicios->at(serv_izq).getDependientes();
		//si queda un proceso de serv_izq en vec_izq, revisar
		if (servicio_vecindario.at(serv_izq).at(vec_izq) == 1)
		{
			//todas las dependencias y dependientes
			for (auto idep = dependencias.begin(); idep != dependencias.end(); ++idep)
			{
				if(servicio_vecindario.at(*idep).at(vec_der) <= 0)
				{
					vecs = false;
					break;
				}
			}
			for (auto idep = dependientes.begin(); idep != dependientes.end(); ++idep)
			{
				if(servicio_vecindario.at(*idep).at(vec_der) <= 0)
				{
					vecs = false;
					break;
				}
			}
		}
		if (!vecs) continue;
		//parte 2: resolver dependencias de serv_der; destino: vec_izq
		dependencias = servicios->at(serv_der).getDependenciaVector();
		dependientes = servicios->at(serv_der).getDependientes();
		if (servicio_vecindario.at(serv_der).at(vec_der) == 1)
		{
			//todas las dependencias y dependientes
			for (auto idep = dependencias.begin(); idep != dependencias.end(); ++idep)
			{
				if(servicio_vecindario.at(*idep).at(vec_izq) <= 0)
				{
					vecs = false;
					break;
				}
			}
			for (auto idep = dependientes.begin(); idep != dependientes.end(); ++idep)
			{
				if(servicio_vecindario.at(*idep).at(vec_izq) <= 0)
				{
					vecs = false;
					break;
				}
			}
		}
		if (!vecs) continue;

		//si llegamos hasta acá, realizar el swap
		//actualizar asignación, spread, espacio_disponible,
		//servicio_maquina, servicio_loc, servicio_vec

		//anotar variables para undo
		rbuso_procesos = uso_procesos;
		rbasign_procesos = asignacion_procesos;
		rbserv_maquina = servicio_maquina;
		rbserv_localizacion = servicio_localizacion;
		rbserv_vecindario = servicio_vecindario;


		//cambiar uso de recursos
		for (int r = 0; r < recursos_num; ++r)
		{
			//quitar recursos en las máquinas origen (same)
			uso_procesos.at(maq_izq).at(r)	-= rec_izq.at(r);
			uso_procesos.at(maq_der).at(r)	-= rec_der.at(r);
			//agregar recursos en las máquinas destino (swap)
			uso_procesos.at(maq_izq).at(r)	+= rec_der.at(r);
			uso_procesos.at(maq_der).at(r)	+= rec_izq.at(r);

			//manejo de recursos transitivos
			if (recursos_trans->at(r))
			{
				//proc_izq: revisar máquina origen/destino (same/swap)
				if (maq_izq == sol_inicial->asignacion_procesos.at(proc_izq))
					uso_transitivo.at(maq_izq).at(r) += rec_izq.at(r);
				if (maq_der == sol_inicial->asignacion_procesos.at(proc_izq))
					uso_transitivo.at(maq_der).at(r) -= rec_izq.at(r);
				//proc_der: same as previous
				if (maq_der == sol_inicial->asignacion_procesos.at(proc_der))
					uso_transitivo.at(maq_der).at(r) += rec_der.at(r);
				if (maq_izq == sol_inicial->asignacion_procesos.at(proc_der))
					uso_transitivo.at(maq_izq).at(r) -= rec_der.at(r);
			}
		}

		//cambio de asignación (swap)
		asignacion_procesos.at(proc_izq) = maq_der;
		asignacion_procesos.at(proc_der) = maq_izq;
		//servicio_maquina para serv_izq
		servicio_maquina.at(serv_izq).at(maq_izq) = false;
		servicio_maquina.at(serv_izq).at(maq_der) = true;
		//servicio_maquina para serv_der
		servicio_maquina.at(serv_der).at(maq_der) = false;
		servicio_maquina.at(serv_der).at(maq_izq) = true;

		//actualizar servicio_loc
		if (loc_izq != loc_der)
		{
			//loc_izq
			servicio_localizacion.at(serv_izq).at(loc_izq) -= 1;
			servicio_localizacion.at(serv_izq).at(loc_der) += 1;
			//loc_der
			servicio_localizacion.at(serv_der).at(loc_der) -= 1;
			servicio_localizacion.at(serv_der).at(loc_izq) += 1;
			//recalcular spread para serv_izq
			if (servicio_localizacion.at(serv_izq).at(loc_izq) == 0)
				spread_servicios.at(serv_izq)--;
			if (servicio_localizacion.at(serv_izq).at(loc_der) == 1)
				spread_servicios.at(serv_izq)++;
			//recalcular spread para serv_der
			if (servicio_localizacion.at(serv_der).at(loc_der) == 0)
				spread_servicios.at(serv_der)--;
			if (servicio_localizacion.at(serv_der).at(loc_izq) == 1)
				spread_servicios.at(serv_der)++;
		}

		//actualizar servicio_vec
		if (vec_izq != vec_der)
		{
			servicio_vecindario.at(serv_izq).at(vec_izq) -=1;
			servicio_vecindario.at(serv_izq).at(vec_der) +=1;
			servicio_vecindario.at(serv_der).at(vec_der) -=1;
			servicio_vecindario.at(serv_der).at(vec_izq) +=1;
		}
		//movimiento válido
		moved = true;
		break;

	}//endfor(revisión de procesos)

	//Revisión de costes y tirada de dado
	if (moved)
	{
		int costo_sol_anterior = costo_solucion;
		calcularCostoSolucion();
		//calcular diferencia, tirar dado, etc
		int delta = costo_sol_anterior - costo_solucion;
		//si costo_solucion es menor que la anterior, la solución pasa
		if (delta > 0) return true;
		//si no, tirar un dado
		uniform_real_distribution<double> exp_dado(0, 1);
		double exp_eval = exp(delta/temperatura);
		//tirar dado (revisar si esto es cierto!!)
		//si pasa, nos quedamos con empeorar la solución
		if (exp_eval > exp_dado(generador)) moved = true;
		//si no pasa, hacer el rollback
		else
		{
			//rollback:
			/*
			//dentro de este scope, *_izq será rb*_der
			//y *_der será rb*_izq
			int rbproc_izq = uproc_der;			int rbproc_der = proc_izq;
			int rbmaq_izq = umaq_der;			int rbmaq_der = maq_izq;
			int rbserv_izq = userv_der;			int rbserv_der = serv_izq;
			int rbloc_izq = uloc_der;			int rbloc_der = loc_izq;
			int rbvec_izq = uvec_der;			int rbvec_der = vec_izq;
			vector<int> rbrec_izq = urec_der;	vector<int> rbrec_der = rec_izq;

			//ejecutar los mismos movimientos para el swap
			//cambiar uso de recursos
			for (int r = 0; r < recursos_num; ++r)
			{
				//quitar recursos en las máquinas origen (same)
				uso_procesos.at(rbmaq_izq).at(r)	-= rbrec_izq.at(r);
				uso_procesos.at(rbmaq_der).at(r)	-= rbrec_der.at(r);
				//agregar recursos en las máquinas destino (swap)
				uso_procesos.at(rbmaq_izq).at(r)	+= rbrec_der.at(r);
				uso_procesos.at(rbmaq_der).at(r)	+= rbrec_izq.at(r);

				//manejo de recursos transitivos
				if (recursos_trans->at(r))
				{
					//proc_izq: revisar máquina origen/destino (same/swap)
					if (rbmaq_izq == sol_inicial->asignacion_procesos.at(rbproc_izq))
						uso_transitivo.at(rbmaq_izq).at(r) += rbrec_izq.at(r);
					if (rbmaq_der == sol_inicial->asignacion_procesos.at(rbproc_izq))
						uso_transitivo.at(rbmaq_der).at(r) -= rbrec_izq.at(r);
					//proc_der: same as previous
					if (rbmaq_der == sol_inicial->asignacion_procesos.at(rbproc_der))
						uso_transitivo.at(rbmaq_der).at(r) += rbrec_der.at(r);
					if (rbmaq_izq == sol_inicial->asignacion_procesos.at(rbproc_der))
						uso_transitivo.at(rbmaq_izq).at(r) -= rbrec_der.at(r);
				}
			}

			//cambio de asignación (swap)
			asignacion_procesos.at(rbproc_izq) = rbmaq_der;
			asignacion_procesos.at(rbproc_der) = rbmaq_izq;
			//servicio_maquina para serv_izq
			servicio_maquina.at(rbserv_izq).at(rbmaq_izq) = false;
			servicio_maquina.at(rbserv_izq).at(rbmaq_der) = true;
			//servicio_maquina para serv_der
			servicio_maquina.at(rbserv_der).at(rbmaq_der) = false;
			servicio_maquina.at(rbserv_der).at(rbmaq_izq) = true;

			//actualizar servicio_loc
			if (rbloc_izq != rbloc_der)
			{
				//loc_izq
				servicio_localizacion.at(rbserv_izq).at(rbloc_izq) -= 1;
				servicio_localizacion.at(rbserv_izq).at(rbloc_der) += 1;
				//loc_der
				servicio_localizacion.at(rbserv_der).at(rbloc_der) -= 1;
				servicio_localizacion.at(rbserv_der).at(rbloc_izq) += 1;
				//recalcular spread para serv_izq
				if (servicio_localizacion.at(rbserv_izq).at(rbloc_izq) == 0)
					spread_servicios.at(rbserv_izq)--;
				if (servicio_localizacion.at(rbserv_izq).at(rbloc_der) == 1)
					spread_servicios.at(rbserv_izq)++;
				//recalcular spread para serv_der
				if (servicio_localizacion.at(rbserv_der).at(rbloc_der) == 0)
					spread_servicios.at(rbserv_der)--;
				if (servicio_localizacion.at(rbserv_der).at(rbloc_izq) == 1)
					spread_servicios.at(rbserv_der)++;
			}

			//actualizar servicio_vec
			if (rbvec_izq != rbvec_der)
			{
				servicio_vecindario.at(rbserv_izq).at(rbvec_izq) -=1;
				servicio_vecindario.at(rbserv_izq).at(rbvec_der) +=1;
				servicio_vecindario.at(rbserv_der).at(rbvec_der) -=1;
				servicio_vecindario.at(rbserv_der).at(rbvec_izq) +=1;
			}
			*/

			//restaurar
			uso_procesos = rbuso_procesos;
			asignacion_procesos = rbasign_procesos;
			servicio_maquina = rbserv_maquina;
			servicio_localizacion = rbserv_localizacion;
			servicio_vecindario = rbserv_vecindario;
			moved = false;
			calcularCostoSolucion();
		}
	}
	return moved;
}



