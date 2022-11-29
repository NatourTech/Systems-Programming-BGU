#include "../include/Coalitions.h"
//Coalition constructor
Coalition::Coalition(int coll_id):collParties(),mandates(0),id(coll_id) {
}
//return id's of parties in colition
const vector<int> Coalition::getParties() const{
    return collParties; 
}

//add party to colition
void Coalition::add(int pid, int _mandates){
    mandates += _mandates;
    this->collParties.push_back(pid);
}

//get colition mandates
int Coalition::getMandates() const{
    return mandates;
}
