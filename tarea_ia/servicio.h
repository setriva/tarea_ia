#pragma once
#include <vector>
#include <set>

using namespace std;

class servicio
{
public:
    servicio();

    //setters
    void setId(int id);
    void setSpreadmin(int nspreadmin);
    void initDependencias(int dependencias_num);
    void setDependencia(int id, int valor);
    void setDependencia(vector<int> vec);
    void setDependiente(int dep);
    //getters
    int getId();
    int getSpreadmin();
    int getDependencia(int id);
    int getDependenciaNum();
    vector<int> getDependenciaVector();
    vector<int> getDependientes();
    int getPrioridad();

    //manipulación de procesos
    void pushProceso(int newProceso);
    vector<int> getListaProcesos();

    //ordenamiento
    bool operator<(const servicio& srv) const;
    void calcularPrioridad();

private:
    int id;
    int spreadmin;
    //de quién depende este servicio
    int dependencias_num;
    vector<int> dependencias;

    //quién depende de este servicio
    set<int> dependientes;

    //esto guarda el ID de los procesos
    vector<int> idProcesos;
    //ordenamiento
    int prioridad;
};
