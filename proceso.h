#pragma once

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

