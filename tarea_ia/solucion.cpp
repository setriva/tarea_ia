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
    	uvec = im->getEspacioMaxVector();
    	svec = im->getEspacioSafeVector();
    	for (int r = 0; r < recursos_num; ++r)
    	{
    		//obtener espacio utilizado en base al disponible
    		//mantener el espacio transitivo como referencia al espacio disponible
    		uvec.at(r) -= espacio_disponible.at(im->getId()).at(r);
    		//restar espacio utilizado y capacidad segura
    		exc.at(r) = svec.at(r) - uvec.at(r);
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

int solucion::getCostoSolucion()
{
	return costo_solucion;
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
}

void solucion::setObjetos(vector<proceso>* procs, vector<maquina>* maqs, vector<servicio>* srvs)
{
    procesos = procs;
    maquinas = maqs;
    servicios = srvs;
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
                    if(espacio_disponible.at(m_id).at(r) < procesos->at(p).getUsoRecursos(r))
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
	cout << "Spread de los servicios:" << endl;
	for (int s = 0; s < servicios_num; ++s)
	{
		cout << spread_servicios.at(s) << " ";
	}
	cout << endl << endl;

    //true si están todos los procesos asignados, false si no se pudo agregar algo
    return insert_ok;
}

void solucion::descartarGreedy()
{
	asignacion_procesos = sol_inicial->asignacion_procesos;
	spread_servicios = sol_inicial->spread_servicios;
	espacio_disponible = sol_inicial->espacio_disponible;
	servicio_maquina = sol_inicial->servicio_maquina;
	servicio_localizacion = sol_inicial->servicio_localizacion;
	servicio_vecindario = sol_inicial->servicio_vecindario;
}



