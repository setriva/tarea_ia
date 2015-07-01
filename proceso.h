#pragma once

class proceso
{
public:
    //Init
    proceso();
    ~proceso();
    void initRecursos(int recursos_num);
    //setters
    void setServicio(int nServicio);
    void setUsoRecursos(int id, int valor);
    void setCosteMovimiento(int nCoste);
    
private:
    int id_servicio;
    int* recursos;
    int coste_movimiento;
};

