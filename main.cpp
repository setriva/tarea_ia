#include <iostream>
#include <fstream>
#include <string>

#include "proceso.h"
#include "servicio.h"
#include "maquina.h"

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
    //Variables de la instancia
    int recursos_num;
    bool* recursos_trans;
    int* recursos_pesos;
    //Máquinas
    int maquinas_num;
    maquina* maquinas;
    //Servicios
    int servicios_num;
    servicio* servicios;
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
    
    ifstream instancia(fileInstancia);
    
    //primera línea: cantidad de recursos
    instancia >> line;
    recursos_num = stoi(line);
    
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
    
    //siguiente línea: cantidad de máquinas
    instancia >> line;
    maquinas_num = stoi(line);
    
    //reservar memoria para máquinas
    maquinas = new maquina[maquinas_num];
    //llenando las máquinas
    for (int i = 0; i < maquinas_num; ++i)
    {
        //inicializar máquinas
        maquinas[i].init(recursos_num, maquinas_num);
        
        //leer: neighborhood
        instancia >> line;
        maquinas[i].setNeighborhood(stoi(line));
        cout << "neigh: " << line << ' ' << maquinas[i].getNeighborhood() << '\n';
        
        //leer: location
        instancia >> line;
        maquinas[i].setLocation(stoi(line));
        cout << "loc: " << line << ' ' << maquinas[i].getLocation() << '\n';
        
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
            maquinas[i].setCosteMovimiento(j, stoi(line));
        }
    }
    //siguiente línea: cantidad de servicios
    instancia >> line;
    servicios_num = stoi(line);
    //reservar memoria
    servicios = new servicio[servicios_num];
    //llenar
    for (int i = 0; i < servicios_num; ++i)
    {
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
        }
    }
    
    
    
    //cerrar archivo
    instancia.close();
    
    
    //limpiar arrays
    delete[] recursos_trans;
    delete[] recursos_pesos;
    delete[] maquinas;
    return 0;
}
