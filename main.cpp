#include <iostream>
#include <fstream>
#include <string>

#include <chrono>

#include "proceso.h"
#include "servicio.h"
#include "maquina.h"
#include "solucion.h"

#include "aux.h"

using namespace std;

//COMPILAR CON FLAG -std=c++11

/* mrp:
 * -t time limit
 * -p instance_filename
 * -i original_solution_filename
 * -o new_solution_filename
 * -s seed
 * -T initial temperature
 * -N constant temp iterations
 * -R cooling ratio
 */

int main(int argc, char **argv)
{
    //Solución
    solucion sol_inicial;

    //Variables de la instancia
    int recursos_num;
    bool* recursos_trans;
    int* recursos_pesos;
    //Máquinas
    int maquinas_num;
    int localizaciones_num = 0;
    int vecindarios_num = 0;
    maquina* maquinas;
    int** costoMovimientoMaquinas;
    //Servicios
    int servicios_num;
    servicio* servicios;
    //Procesos
    int procesos_num;
    proceso* procesos;
    //Auxiliares
    string line;
    int auxint;

    //Obtener ruta de archivo instancia
    char* fileInstancia = getCmdOption(argv, argv+argc, "-p");
    if (!fileInstancia)
    {
        cout << "Falta el archivo de instancia.\n";
        return -1;
    }

    cout << "La instancia es " << fileInstancia << endl;

    //Cronometrando tiempo de lectura
    auto inicio_lectura = chrono::high_resolution_clock::now();

    ifstream instancia(fileInstancia, ifstream::in);

    if (!instancia.good())
    {
        cout << "Instancia no existe!" << endl;
        return -1;
    }

    //primera línea: cantidad de recursos
    instancia >> line;
    recursos_num = stoi(line);

    cout << "Cargando " << recursos_num << " recursos" << endl;

    //inicializar arrays de recursos
    recursos_trans = new bool[recursos_num];
    recursos_pesos = new int[recursos_num];

    //rellenar arrays de recursos
    for (int i = 0; i < recursos_num; ++i)
    {
        //boolean (is in TR)
        instancia >> line;
        recursos_trans[i] = (bool)stoi(line);
        instancia >> line;
        recursos_pesos[i] = stoi(line);
    }
    //pasar parámetros de recursos a la solución
    sol_inicial.setParametrosRecursos(recursos_num, recursos_pesos, recursos_trans);

    //siguiente línea: cantidad de máquinas
    instancia >> line;
    maquinas_num = stoi(line);

    cout << "Cargando " << maquinas_num << " maquinas" << endl;

    //reservar memoria para máquinas
    maquinas = new maquina[maquinas_num];
    //reservando columnas de costos
    costoMovimientoMaquinas = new int*[maquinas_num];
    //llenando las máquinas
    for (int i = 0; i < maquinas_num; ++i)
    {
        //inicializar máquinas
        maquinas[i].init(i, recursos_num, maquinas_num);
        //reservando fila de costos
        costoMovimientoMaquinas[i] = new int[maquinas_num];

        //leer: neighborhood
        instancia >> line;
        auxint = stoi(line);    //neighborhood
        maquinas[i].setNeighborhood(auxint);
        //contar vecindarios
        if (auxint > vecindarios_num) vecindarios_num = auxint;
        //cout << "neigh: " << line << ' ' << maquinas[i].getNeighborhood() << '\n';

        //leer: location
        instancia >> line;
        auxint = stoi(line);
        maquinas[i].setLocation(auxint);
        //contar localizaciones
        if (auxint > localizaciones_num) localizaciones_num = auxint;
        //cout << "loc: " << line << ' ' << maquinas[i].getLocation() << '\n';

        //leer: capacidades máximas
        for (int j = 0; j < recursos_num; ++j)
        {
            instancia >> line;
            maquinas[i].setCapacidadMax(j, stoi(line));
        }

        //leer: capacidades seguras
        for (int j = 0; j < recursos_num; ++j)
        {
            instancia >> line;
            maquinas[i].setCapacidadSafe(j, stoi(line));
        }

        //leer: coste de movimiento a máquinas
        for (int j = 0; j < maquinas_num; ++j)
        {
            instancia >> line;
            costoMovimientoMaquinas[i][j] = stoi(line);
        }
        cout << '.';
    }
    cout << endl;
    sol_inicial.setCMM(costoMovimientoMaquinas);
    //siguiente línea: cantidad de servicios
    instancia >> line;
    servicios_num = stoi(line);

    cout << "Cargando " << servicios_num << " servicios" << endl;

    //reservar memoria
    servicios = new servicio[servicios_num];
    //llenar
    for (int i = 0; i < servicios_num; ++i)
    {
        //id del servicio
        servicios[i].setId(i);
        //spreadmin
        instancia >> line;
        servicios[i].setSpreadmin(stoi(line));
        //dependencias: auxint es la cantidad de dependencias
        instancia >> line;
        auxint = stoi(line);
        //hay dependencias (el segundo dígito no es cero)
        if (auxint != 0)
        {
            //inicializar
            servicios[i].initDependencias(auxint);
            for (int j = 0; j < auxint; ++j)
            {
                //leer las dependencias del servicio
                instancia >> line;
                servicios[i].setDependencia(j, stoi(line));
            }
            cout << "Servicio " << i << "tiene " << auxint << "dependencias" << endl;
        }
        cout << '.';
    }
    cout << endl;
    //leer procesos
    instancia >> line;
    procesos_num = stoi(line);

    cout << "Cargando " << procesos_num << " procesos" << endl;

    //reservando memoria de procesos
    procesos = new proceso[procesos_num];
    //rellenar!
    for (int i = 0; i < procesos_num; ++i)
    {
        //guardar identificador
        procesos[i].setId(i);

        //auxint: servicio del proceso
        instancia >> line;
        auxint = stoi(line);

        //guardar servicio en proceso
        procesos[i].setServicio(auxint);
        //guardar proceso en servicio
        servicios[auxint].pushProceso(i);

        //uso de recursos
        procesos[i].initRecursos(recursos_num);
        for (int j = 0; j < recursos_num; ++j)
        {
            instancia >> line;
            procesos[i].setUsoRecursos(j, stoi(line));
        }

        //coste de mover el proceso
        instancia >> line;
        procesos[i].setCosteMovimiento(stoi(line));
        cout << '.';
    }
    cout << endl;

    //balanceadores
    instancia >> line;
    //auxint: balances_num
    auxint = stoi(line);
    sol_inicial.initBalanceTriple(auxint);
    //más auxiliares para guardar la tripleta
    int rec1, rec2, ratio;
    for (int i = 0; i < auxint; ++i)
    {
        //rec1
        instancia >> line;
        rec1 = stoi(line);
        //rec2
        instancia >> line;
        rec2 = stoi(line);
        //ratio
        instancia >> line;
        ratio = stoi(line);
        //guardar
        sol_inicial.setBalanceTriple(i, rec1, rec2, ratio);
    }
    //pesos finales
    instancia >> line;
    sol_inicial.setPesoBalance(stoi(line));
    instancia >> line;
    sol_inicial.setPesoMoverProceso(stoi(line));
    instancia >> line;
    sol_inicial.setPesoMoverServicio(stoi(line));
    instancia >> line;
    sol_inicial.setPesoMoverMaquina(stoi(line));

    //cerrar archivo de instancia
    instancia.close();

    //Fin de lectura de instancia
    auto fin_lectura = chrono::high_resolution_clock::now();
    cout << "Tiempo de lectura de instancia: " << chrono::duration_cast<chrono::microseconds>(fin_lectura-inicio_lectura).count() << "us" << endl;

    //Obtener ruta de archivo solución inicial
    char* fileSolInicial = getCmdOption(argv, argv+argc, "-i");
    if (!fileSolInicial)
    {
        cout << "Falta el archivo de solución inicial.\n";
        return -1;
    }

    //inicializar estructuras de la solución
    sol_inicial.setCantidades(procesos_num, maquinas_num, servicios_num, localizaciones_num, vecindarios_num);
    //pasar modelo
    sol_inicial.setObjetos(procesos, maquinas);

    //leer archivo
    if (sol_inicial.llenarSolucion(fileSolInicial) != 0)
    {
        cout << "Algo pasó" << endl;
        return -1;
    }







    //limpiar arrays
    delete[] recursos_trans;
    delete[] recursos_pesos;
    delete[] maquinas;
    return 0;
}
