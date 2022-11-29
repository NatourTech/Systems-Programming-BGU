#pragma once
#include <vector>
#include "Party.h"
#include "Graph.h"
#include <iostream>
using std::vector;

class SelectionPolicy { 
    public:
        virtual int select(vector<int> agentNeighbors,Graph& g,int Pid) = 0;
        virtual ~SelectionPolicy();
        virtual SelectionPolicy* clone() const = 0;
};

class MandatesSelectionPolicy: public SelectionPolicy{ 
    public:
    MandatesSelectionPolicy();
    int select(vector<int> agentNeighbors,Graph& g, int Pid) override;
    SelectionPolicy* clone() const ;
       
};

class EdgeWeightSelectionPolicy: public SelectionPolicy{ 
    public: 
    EdgeWeightSelectionPolicy();
    int select(vector<int> agentNeighbors,Graph& g,int Pid) override;
    SelectionPolicy* clone() const;  

};