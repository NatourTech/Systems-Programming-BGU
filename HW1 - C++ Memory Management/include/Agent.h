#pragma once

#include <vector>
#include "Graph.h"
#include "SelectionPolicy.h"

using std::vector;
class SelectionPolicy;

class Agent
{
public:
    Agent(int agentId, int partyId, SelectionPolicy *selectionPolicy);

    int select(vector<int> neighs,Graph &a,int pid);

    int getPartyId() const;
    int getId() const;
    void step(Simulation &);

    //added
    int getColId() const;
    void setColId(int _colId);
    void setPartyId(int id);
    void setAid(int aid);

    //dest
     virtual ~Agent();
     
    //copy cons
    Agent(const Agent &other);
    //copy assign oper
    Agent& operator=(const Agent& other);
    //move assign oper
    Agent& operator=(Agent&& other) noexcept ;
    //move const
    Agent(Agent&& other) noexcept ;

private:
    int mAgentId;
    int mPartyId;
    SelectionPolicy *mSelectionPolicy;

    //added 
    int colId;
};
