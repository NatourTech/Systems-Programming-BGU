#include "../include/SelectionPolicy.h"
#include <iostream>
#include <vector>
#include "Graph.h"

//test
#include <iostream>
using std::vector;
using std::cout;
using std::endl;


SelectionPolicy::~SelectionPolicy()= default;

MandatesSelectionPolicy::MandatesSelectionPolicy(){}
EdgeWeightSelectionPolicy::EdgeWeightSelectionPolicy(){}

 
// return the party index in vector with max mandates;
int MandatesSelectionPolicy::select(vector<int> agentNeighbors,Graph& g,int Pid){
    int max_mandates = -1;
    int id = -1;
    for(std::vector<int>::size_type i = 0; i < agentNeighbors.size(); i++)
    {
        int currM = g.getPartyref(agentNeighbors[i]).getMandates();
        if( currM >  max_mandates){
            max_mandates = currM;
            id = agentNeighbors[i];
        }
       
    }
    return id;
}

// return the party index in vector with max edgeWeight;
int EdgeWeightSelectionPolicy::select(vector<int> agentNeighbors,Graph& g,int Pid){
    int maxWieght = -1;
    int id = - 1;
    for(std::vector<Party>::size_type i = 0; i < agentNeighbors.size(); i++)
    {
        int currW = g.getEdgeWeight(Pid,agentNeighbors[i]);
        if(currW > maxWieght){
            maxWieght = currW;
            id = agentNeighbors[i];
        }
    }
    return id;
}
//clone selection, Edge policy, good practice
SelectionPolicy* EdgeWeightSelectionPolicy::clone() const{
    return new EdgeWeightSelectionPolicy(*this);
}
//clone selection, Mandates policy, good practice
SelectionPolicy* MandatesSelectionPolicy::clone() const{
    return new MandatesSelectionPolicy(*this);
}
