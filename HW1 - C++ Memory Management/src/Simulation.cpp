#include "Simulation.h"
#include "Coalitions.h"
#include "SelectionPolicy.h"
#include "Agent.h"
#include <iostream>
#include <utility>
#include <vector>


using std::cout;
using std::endl;

Simulation::Simulation(Graph graph, vector<Agent> agents) : mGraph(graph), mAgents(agents),cols()
{

    for(unsigned int i = 0; i < mAgents.size(); i++){
        mAgents[i].setColId(i);
        Coalition c(i);
        int pid = mAgents[i].getPartyId();
        Party& p = getPartyref(pid);
        p.setColId(i);
        int mandates  = p.getMandates();
        c.add(pid, mandates);
        cols.push_back(c);
    }
}

void Simulation::step()
{
   for(unsigned int i = 0 ; i<mGraph.getVertices().size();i++){
       mGraph.getPartyref(i).step(*this);
        
    }
    for(unsigned int i = 0 ; i<mAgents.size();i++){
        mAgents[i].step(*this);
    }

}

bool Simulation::shouldTerminate() const
{
   
    for(std::vector<Coalition>::size_type j =0 ; j<cols.size();j++){
        if(cols[j].getMandates()>= 61) return true;
    }
    bool terminate = true;
    vector<Party> parties = mGraph.getVertices();
    for(std::vector<Party>::size_type i = 0 ; i< parties.size() && terminate;i++){
        if(parties[i].getState() != State::Joined) {
            terminate = false;
        }
    }
    if(terminate){

    }
    return terminate;
}

const Graph &Simulation::getGraph() const
{
    return mGraph;
}


const vector<Agent>& Simulation::getAgents() const
{
    return mAgents;
}

const Party &Simulation::getParty(int partyId) const
{
    return mGraph.getParty(partyId);
}
Party &Simulation::getPartyref(int partyId) {
    return mGraph.getPartyref(partyId);
}

Graph &Simulation::getGraphref() {
    return mGraph;
}

int Simulation::getColMandates(int cId) {
    return cols[cId].getMandates();
}


void Simulation::pushClone(Agent b){
    mAgents.push_back(b);
}


vector<Coalition>& Simulation::getcoals() {
    return cols;
}
Coalition& Simulation::getCol(int ColID){
    return cols[ColID];
}
const Coalition& Simulation::getColconst(int ColID) const{
    return cols[ColID];
}

vector<Agent>& Simulation::getAgentsref(){
    return mAgents;
}


Agent& Simulation::getAgent(int agId){
    return mAgents[agId];
}


/// This method returns a "coalition" vector, where each element is a vector of party IDs in the coalition.
/// At the simulation initialization - the result will be [[agent0.partyId], [agent1.partyId], ...]
const vector<vector<int>> Simulation::getPartiesByCoalitions() const
{
    vector<vector<int>> toReturn;
    for(std::vector<Coalition>::size_type i = 0 ; i<cols.size();i++){
        toReturn.push_back(getColconst(i).getParties());
    }
    return toReturn;
    // TODO: you MUST implement this method for getting proper output, read the documentation above.
}

int Simulation::select(vector<int> neighs,Graph& g,int aid, int pid){
   return mAgents[aid].select(neighs,g,pid);
}