#pragma once
#include <vector>

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
    //getters
    int getId();
    int getSpreadmin();
    int getDependencia(int id);
    int getDependenciaNum();
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
    vector<int> dependencias;
    int dependencias_num;
    //esto guarda el ID de los procesos
    vector<int> idProcesos;
    //ordenamiento
    int prioridad;
};
