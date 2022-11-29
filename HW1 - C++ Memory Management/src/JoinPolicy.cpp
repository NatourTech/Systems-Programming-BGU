#include "../include/JoinPolicy.h"
#include <vector>
#include <iostream>

using std::cout;
using std::endl;
using std::vector;


JoinPolicy::~JoinPolicy(){};

MandatesJoinPolicy::MandatesJoinPolicy(){}
LastOfferJoinPolicy::LastOfferJoinPolicy(){}

 
// return  index of agent  that choosed, with max mandates;
int MandatesJoinPolicy::pick(vector<int> offers,Simulation& sim){
    int max_mandates=-1;
    int id = -1;
    vector<Agent> agnts = sim.getAgents();
    for(std::vector<int>::size_type i = 0 ; i<offers.size();i++){
        int col_id = agnts[offers[i]].getColId();
        int col_mands = sim.getColMandates(col_id);
        if(col_mands > max_mandates){
            max_mandates = col_mands;
            id = offers[i];
        }

    }
    return id;

}

 
// return the  index that las joined;
int LastOfferJoinPolicy::pick(vector<int> offers,Simulation& sim){
    int last = offers[offers.size() - 1];
    return last;
}


//clone selection, Mandates policy, good practice
JoinPolicy* MandatesJoinPolicy::clone() const{
    return new MandatesJoinPolicy(*this);
}
//clone selection, lastOffer policy, good practice
JoinPolicy* LastOfferJoinPolicy::clone() const{
    return new LastOfferJoinPolicy(*this);
}