#pragma once
#include <vector>

using namespace std;

class servicio
{
public:
    servicio();
    ~servicio();
    //setters
    void setId(int id);
    void setSpreadmin(int nspreadmin);
    void initDependencias(int dependencias_num);
    void setDependencia(int id, int valor);
    //getters
    int getSpreadmin();
    int getDependencia(int id);
    int getDependenciaNum();

    //manipulación de procesos
    void pushProceso(int newProceso);

private:
    int id;
    int spreadmin;
    int* dependencias;
    int dependencias_num;
    //esto guarda el ID de los procesos
    vector<int> idProcesos;
};
