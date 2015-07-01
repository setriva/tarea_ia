#pragma once

class servicio
{
public:
    servicio();
    ~servicio();
    //setters
    void setSpreadmin(int nspreadmin);
    void initDependencias(int dependencias_num);
    void setDependencia(int id, int valor);
    //getters
    int getSpreadmin();
    int getDependencia(int id);
    
private:
    int spreadmin;
    int* dependencias;
};