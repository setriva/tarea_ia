#include <iostream>
#include <fstream>
#include <string>
#include <vector>

#include <chrono>
#include <cmath>
#include <ctime>
#include <random>

#include "proceso.h"
#include "servicio.h"
#include "maquina.h"
#include "solucion.h"

#include "aux.h"

using namespace std;

//COMPILAR CON FLAG -std=c++11

/* tarea_ia:
 * -t tiempo límite
 * -p modelo
 * -i solución inicial
 * -o solución nueva
 * -s seed
 * -T temperatura inicial
 * -N iteraciones constantes
 * -R cooling ratio
 * -C limitación espacio de búsqueda
 * -A chance move
 */

int main(int argc, char **argv)
{
    //Solución
    solucion sol_inicial;

    //Variables de la instancia
    int recursos_num;
    vector<bool> recursos_trans;
    vector<int> recursos_pesos;
    //Máquinas
    int maquinas_num;
    int localizaciones_num = 0;
    int vecindarios_num = 0;
    vector<maquina> maquinas;
    vector<vector<int>> costoMovimientoMaquinas;
    //Servicios
    int servicios_num;
    vector<servicio> servicios;
    //Procesos
    int procesos_num;
    vector<proceso> procesos;
    //Balances
    int balances_num;
    vector<vector<int>> balances;
    //Auxiliares
    string line;
    int auxint;
    vector<int> auxvec;

    //Variables de ejecución
    int tiempo_limite = 300;
    int semilla;
    try
    {
    	random_device rd;
    	semilla = rd();
    }
    catch (...)
    {
    	if (!cmdOptionExists(argv, argv+argc, "-s"))
    	{
    		cout << "Se requiere ingresar una semilla!" << endl;
    		return -1;
    	}
    	semilla = stoi(getCmdOption(argv, argv+argc, "-s"));
    }
    long double temperatura_inicial = 1e7l;
    long double iteraciones_temperatura = 1e5l;
    int tam_busqueda = 50;
    float ratio_enfriamiento = 0.97f;
    float chance_move = 0.7f;
    string file_salida = "output.txt";

    //Texto de ayuda
    if (argc < 2)
    {
    	cout << "tarea_ia: parámetros" << endl;
    	cout << "-p \t Archivo model" << endl;
    	cout << "-i \t Archivo assignment" << endl;
    	cout << "-o \t Solución encontrada (default output.txt)" << endl;
    	cout << "-t \t Tiempo límite de ejecución (default 300)" << endl;
    	cout << "-s \t Semilla (default random)" << endl;
    	cout << "-T \t Temperatura inicial (10^T) (default 7)" << endl;
    	cout << "-N \t Iteraciones (10^N) manteniendo temperatura (default 5)" << endl;
    	cout << "-C \t Limitación del espacio de búsqueda (default 50)" << endl;
    	cout << "-A \t Probabilidad de escoger move sobre swap (default 0.7)" << endl;
    	cout << "-R \t Razón de enfriamiento (default 0.97)" << endl;
    }

    //logging
    time_t rawtime;
    struct tm * timeinfo;
    char buffer[80];
    time (&rawtime);
    timeinfo = localtime(&rawtime);
    strftime(buffer,80,"%Y%m%d_%I%M%S",timeinfo);

    string logstr;
    logstr.append("log_");
    logstr.append(buffer);
    logstr.append(".txt");
    ofstream log(logstr);
    cout << "Se ingresará log en archivo: " << logstr << endl;
    streambuf* coutbuf = cout.rdbuf();
    cout.rdbuf(log.rdbuf());

    //Abrir instancia
    if (!cmdOptionExists(argv, argv+argc, "-p"))
    {
    	cout.rdbuf(coutbuf);
    	cout << "Falta el archivo de instancia." << endl;
    	return -1;
    }
    char* fi = getCmdOption(argv, argv+argc, "-p");
    string fileInstancia(fi);
    ifstream instancia(fileInstancia, ifstream::in);
    if (!instancia.good())
    {
    	cout.rdbuf(coutbuf);
        cout << "Instancia no existe!";
        cout << "(intentando con " << fileInstancia << ")"<<  endl;
        return -1;
    }

    //Abrir solución inicial
    if (!cmdOptionExists(argv, argv+argc, "-i"))
    {
    	cout.rdbuf(coutbuf);
        cout << "Falta el archivo de solución inicial.\n";
        return -1;
    }
    char* fsi = getCmdOption(argv, argv+argc, "-i");
    string fileSolInicial(fsi);
    ifstream fsol(fileSolInicial, ifstream::in);
    if (!fsol.good())
    {
    	cout.rdbuf(coutbuf);
        cout << "Solución inicial no existe!" << endl;
        return -1;
    }

    cout << "La instancia es " << fileInstancia << endl;
    cout << "Solución inicial es " << fileSolInicial << endl;

    if (cmdOptionExists(argv, argv+argc, "-s"))	//semilla
    {
    	semilla = stoi(getCmdOption(argv, argv+argc, "-s"));
    }
    mt19937 mt(semilla);
    cout << "Se utiliza la semilla " << semilla << "." << endl;

    cout << endl;
    cout << "Se iniciará lectura de instancia." << endl;
    //cout << "Pulse una tecla" << endl;
    //cin.get();

    //Cronometrando tiempo de lectura de instancia
    auto inicio_lectura = chrono::high_resolution_clock::now();

    //primera línea: cantidad de recursos
    instancia >> line;
    recursos_num = stoi(line);

    cout << "Cargando " << recursos_num << " recursos" << endl;
/*
    //setear tamaño vectores de recursos
    recursos_trans.resize(recursos_num); // = new bool[recursos_num];
    recursos_pesos.resize(recursos_num); // = new int[recursos_num];
*/
    //rellenar arrays de recursos
    for (int i = 0; i < recursos_num; ++i)
    {
        //boolean (is in TR)
        instancia >> line;
        recursos_trans.push_back((bool)stoi(line));
        instancia >> line;
        recursos_pesos.push_back(stoi(line));
    }
    //pasar parámetros de recursos a la solución
    sol_inicial.setParametrosRecursos(recursos_num, &recursos_pesos, &recursos_trans);

    //siguiente línea: cantidad de máquinas
    instancia >> line;
    maquinas_num = stoi(line);

    cout << "Cargando " << maquinas_num << " maquinas" << endl;

    //reservar memoria para máquinas
    //(creando las máquinas?)
    maquinas.resize(maquinas_num, maquina()); // = new maquina[maquinas_num];
    //reservando matriz de costos
    //costoMovimientoMaquinas.resize(maquinas_num, vector<int>(maquinas_num)); // = new int*[maquinas_num];
    //llenando las máquinas
    //for (int i = 0; i < maquinas_num; ++i)
    for (vector<maquina>::iterator im = maquinas.begin(); im != maquinas.end(); ++im)
    {
        //inicializar máquinas
        im->init(distance(maquinas.begin(), im), recursos_num);
    	//im->setId(distance(maquinas.begin(), im));
        //leer: neighborhood
        instancia >> line;
        auxint = stoi(line);    //neighborhood
        im->setNeighborhood(auxint);
        //contar vecindarios
        if (auxint > vecindarios_num) vecindarios_num = auxint;
        //cout << "neigh: " << line << ' ' << maquinas[i].getNeighborhood() << '\n';

        //leer: location
        instancia >> line;
        auxint = stoi(line);
        im->setLocation(auxint);
        //contar localizaciones
        if (auxint > localizaciones_num) localizaciones_num = auxint;
        //cout << "loc: " << line << ' ' << maquinas[i].getLocation() << '\n';

        //leer: capacidades máximas
        auxvec.clear();
        for (size_t r = 0; r < recursos_trans.size(); ++r)
        {
            instancia >> line;
            //maquinas[i]->setCapacidadMax(j, stoi(line));
            auxvec.push_back(stoi(line));
        }
        im->setCapacidadMax(auxvec);

        //leer: capacidades seguras
        auxvec.clear();
        for (size_t r = 0; r < recursos_trans.size(); ++r)
        {
            instancia >> line;
            //maquinas[i].setCapacidadSafe(j, stoi(line));
            auxvec.push_back(stoi(line));
        }
        im->setCapacidadSafe(auxvec);

        //leer: coste de movimiento a máquinas
        auxvec.clear();
        for (int j = 0; j < maquinas_num; ++j)
        {
            instancia >> line;
            //costoMovimientoMaquinas[i][j] = stoi(line);
            auxvec.push_back(stoi(line));
        }
        costoMovimientoMaquinas.push_back(auxvec);

    }
    //vistos los vecs y locs máximas, sumar uno para sacar la cantidad
    vecindarios_num++; localizaciones_num++;
    cout << "Se encontraron " << vecindarios_num << " vecindarios y ";
    cout << localizaciones_num << " localizaciones." << endl;

    //agregar cmm a solución inicial
    sol_inicial.setCMM(&costoMovimientoMaquinas);

    //siguiente línea: cantidad de servicios
    instancia >> line;
    servicios_num = stoi(line);

    cout << "Cargando " << servicios_num << " servicios" << endl;

    //reservar memoria
    servicios.resize(servicios_num, servicio());
    //llenar
    //for (int i = 0; i < servicios_num; ++i)
    for (vector<servicio>::iterator is = servicios.begin(); is != servicios.end(); ++is)
    {
        //id del servicio
        is->setId(distance(servicios.begin(), is));
        //spreadmin
        instancia >> line;
        is->setSpreadmin(stoi(line));
        //dependencias: auxint es la cantidad de dependencias
        instancia >> line;
        auxint = stoi(line);
        //hay dependencias (el segundo dígito no es cero)
        if (auxint != 0)
        {
            //inicializar
            //servicios[i].initDependencias(auxint);
        	auxvec.clear();
            for (int j = 0; j < auxint; ++j)
            {
                //leer las dependencias del servicio
                instancia >> line;
                //servicios[i].setDependencia(j, stoi(line));
                auxvec.push_back(stoi(line));
            }
            is->setDependencia(auxvec);
            cout << "Servicio " << is->getId() << "tiene " << auxvec.size() << "dependencias" << endl;
        }
        //cout << '.';
    }
    //leer procesos
    instancia >> line;
    procesos_num = stoi(line);

    cout << "Cargando " << procesos_num << " procesos" << endl;

    //reservando memoria de procesos
    procesos.resize(procesos_num, proceso());
    //rellenar!
    //for (int i = 0; i < procesos_num; ++i)
    for (vector<proceso>::iterator ip = procesos.begin(); ip != procesos.end(); ++ip)
    {
        //guardar identificador
        ip->setId(distance(procesos.begin(), ip));

        //auxint: servicio del proceso
        instancia >> line;
        auxint = stoi(line);

        //guardar servicio en proceso
        ip->setServicio(auxint);
        //guardar proceso en servicio
        servicios[auxint].pushProceso(ip->getId());

        //uso de recursos
        //procesos[i].initRecursos(recursos_num);
        auxvec.clear();
        for (size_t r = 0; r < recursos_trans.size(); ++r)
        {
            instancia >> line;
            //procesos[i].setUsoRecursos(j, stoi(line));
            auxvec.push_back(stoi(line));
        }
        ip->setUsoRecursos(auxvec);

        //coste de mover el proceso
        instancia >> line;
        ip->setCosteMovimiento(stoi(line));
        //cout << '.';
    }

    //actualizar prioridad de los servicios
    for (vector<servicio>::iterator is = servicios.begin(); is != servicios.end(); ++is)
    {
    	is->calcularPrioridad();
    }
    //pasar prioridad a los procesos
    for (vector<proceso>::iterator ip = procesos.begin(); ip != procesos.end(); ++ip)
    {
    	ip->setPrioridad(servicios.at(ip->getServicio()).getPrioridad());
    }

    cout << "Prioridades de los procesos:" << endl;
    for (int i = 0; i < procesos_num; ++i)
    {
    	cout << procesos.at(i).getPrioridad() << ' ';
    }


    //balanceadores
    instancia >> line;
    balances_num = stoi(line);
    auxvec.clear();
    for (int b = 0; b < balances_num; ++b)
    {
        //rec1
        instancia >> line;
        auxvec.push_back(stoi(line));
        //rec2
        instancia >> line;
        auxvec.push_back(stoi(line));
        //ratio
        instancia >> line;
        auxvec.push_back(stoi(line));
        //peso del balanceador
        instancia >> line;
        auxvec.push_back(stoi(line));
        //guardar
        balances.push_back(auxvec);

    }
    //set en solución inicial
    sol_inicial.setBalanceTriple(&balances);
    //set en las máquinas (para el balanceo de espacio libre)
    /*for (vector<maquina>::iterator im = maquinas.begin(); im != maquinas.end(); ++im)
    {
    	im->setBalances(&balances);
    }*/

    //pesos finales
    instancia >> line;
    sol_inicial.setPesoMoverProceso(stoi(line));
    instancia >> line;
    sol_inicial.setPesoMoverServicio(stoi(line));
    instancia >> line;
    sol_inicial.setPesoMoverMaquina(stoi(line));

    //cerrar archivo de instancia? se supone que no es necesario
    //instancia.close();

    //Fin de lectura de instancia
    auto fin_lectura = chrono::high_resolution_clock::now();
    cout << "Tiempo de lectura de instancia: " << chrono::duration_cast<chrono::microseconds>(fin_lectura-inicio_lectura).count() << "us" << endl;

    cout << "Se leerá solución inicial." << endl;
    //cout << "Pulse una tecla" << endl;
    //cin.get();

    //Cronometrando tiempo de lectura de solución inicial
    inicio_lectura = chrono::high_resolution_clock::now();

    //cout << "Intentando inicializar estructuras de solución inicial" << endl;
    cout << "Pasando cantidades necesarias" << endl;

    //inicializar estructuras de la solución
    sol_inicial.setCantidades(procesos_num, maquinas_num, servicios_num, localizaciones_num, vecindarios_num);

    cout << "Intentando pasar objetos" << endl;

    //pasar modelo
    sol_inicial.setObjetos(&procesos, &maquinas, &servicios);

    cout << "Intentando llenar solución" << endl;

    //leer archivo
    if (sol_inicial.llenarSolucion(fsol) != 0)
    {
    	cout.rdbuf(coutbuf);
        cout << "Algo pasó" << endl;
        return -1;
    }

    //Fin de lectura de solución inicial
    fin_lectura = chrono::high_resolution_clock::now();
    cout << "Tiempo de lectura de sol. inicial: " << chrono::duration_cast<chrono::microseconds>(fin_lectura-inicio_lectura).count() << "us" << endl;

    cout << "Se generará solución greedy." << endl;
    //cout << "Pulse una tecla" << endl;
    //cin.get();

    inicio_lectura = chrono::high_resolution_clock::now();

    //preparar solución greedy
    solucion sol_nueva(sol_inicial);

    cout << "Iniciando algoritmo greedy." << endl;

    bool greedy = sol_nueva.llenarSolucion();

    fin_lectura = chrono::high_resolution_clock::now();
    cout << "Tiempo de generación de solución: " << chrono::duration_cast<chrono::milliseconds>(fin_lectura-inicio_lectura).count() << "ms" << endl;

    if (!greedy)
    {
        cout << "Algoritmo greedy no funcionó, se descartará." << endl;
        sol_nueva.descartarGreedy();
    }

    cout << endl << "Iniciando algoritmo Simulated Annealing." << endl;

    //Obteniendo parámetros de SA
    if (cmdOptionExists(argv, argv+argc, "-o"))	//output sol encontrada
    {
    	file_salida = string(getCmdOption(argv, argv+argc, "-o"));
    }
    cout << "El archivo de salida es " << file_salida << "." << endl;

    if (cmdOptionExists(argv, argv+argc, "-t"))	//tiempo limite
    {
    	tiempo_limite = stoi(getCmdOption(argv, argv+argc, "-t"));
    }
    cout << "Se ejecutará el algoritmo durante " << tiempo_limite << "segundos." << endl;

    if (cmdOptionExists(argv, argv+argc, "-T"))	//temp inicial
    {
    	auxint = stoi(getCmdOption(argv, argv+argc, "-T"));
    	temperatura_inicial = pow(10, auxint);
    }
    cout << "Temperatura inicial es " << temperatura_inicial << endl;

    if (cmdOptionExists(argv, argv+argc, "-N"))	//iter temp cte
    {
    	auxint = stoi(getCmdOption(argv, argv+argc, "-N"));
    	iteraciones_temperatura = pow(10, auxint);
    }
    cout << "Iteraciones manteniendo temperatura: " << iteraciones_temperatura << endl;

    if (cmdOptionExists(argv, argv+argc, "-C"))	//limit esp busq
    {
    	tam_busqueda = stoi(getCmdOption(argv, argv+argc, "-C"));
    }
    cout << "Límite del espacio de búsqueda: " << tam_busqueda << endl;

    if (cmdOptionExists(argv, argv+argc, "-A"))	//chance move
    {
    	chance_move = stof(getCmdOption(argv, argv+argc, "-A"));
    }
    cout << "Probabilidad de move sobre swap: " << chance_move << endl;

    if (cmdOptionExists(argv, argv+argc, "-R"))	//cool ratio
    {
    	ratio_enfriamiento = stof(getCmdOption(argv, argv+argc, "-R"));
    }
    cout << "Razón de enfriamiento: " << ratio_enfriamiento << endl;


    cout.rdbuf(coutbuf);
    cout << "Ejecución finalizada.";
    return 0;
}
