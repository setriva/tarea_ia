#pragma once
#include <list>

class proceso
{
public:
    //Init
    proceso();
    proceso(int recursos);
    void setCostes(int* cost);
    
private:
    int* costes;
};

class maquina
{
public:
    maquina();
    
private:
    std::list<proceso> procesos;
    

};