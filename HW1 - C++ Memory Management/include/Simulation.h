#pragma once

#include <vector>

#include "Graph.h"
#include "Agent.h"
#include "Coalitions.h"
#include "SelectionPolicy.h"


using std::string;
using std::vector;

class Simulation
{
public:
    Simulation(Graph g, vector<Agent> agents);

    void step();
    bool shouldTerminate() const;

    const Graph &getGraph() const;
    Agent& getAgent(int agId);

    Graph &getGraphref();

    const vector<Agent> &getAgents() const;

    
    //const //const
    const Party &getParty(int partyId) const;


    const vector<vector<int>> getPartiesByCoalitions() const;


    Party &getPartyref(int partyId);

    int getColMandates(int cId);

    vector<Coalition>& getcoals();

    vector<Agent>& getAgentsref();

    int select(vector<int> neighs,Graph& g,int aid, int pid);

    Coalition& getCol(int ColID);

    const Coalition& getColconst(int ColID) const;

    void pushClone(Agent b);


private:
    Graph mGraph;
    vector<Agent> mAgents;
    //added
    vector<Coalition> cols;
};
