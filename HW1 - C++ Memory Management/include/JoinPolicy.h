#pragma once
#include <vector>
#include "../include/Simulation.h"
#include <iostream>

class JoinPolicy {
public:
    virtual int pick(vector<int> offers,Simulation& sim) = 0;
    virtual ~JoinPolicy();
    virtual JoinPolicy* clone() const = 0;
    
};

class MandatesJoinPolicy : public JoinPolicy {
public:
    MandatesJoinPolicy();
    int pick(vector<int> offers,Simulation& sim) override;
    JoinPolicy* clone() const ;

};

class LastOfferJoinPolicy : public JoinPolicy {
public:
    LastOfferJoinPolicy();
    int pick(vector<int> offers,Simulation& sim) override;
    JoinPolicy* clone() const ;

};