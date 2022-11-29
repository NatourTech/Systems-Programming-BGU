#include "Agent.h"

// we added
#include "Simulation.h"
#include "../include/SelectionPolicy.h"
#include <vector>

using std::vector;
using std::cout;
using std::endl;

//Agent constructor
Agent::Agent(int agentId, int partyId, SelectionPolicy *selectionPolicy) : mAgentId(agentId), mPartyId(partyId), mSelectionPolicy(selectionPolicy),colId(-1)
{
    
}
//Agent destructor
 Agent::~Agent()
 {if(mSelectionPolicy) delete mSelectionPolicy;}

Agent::Agent(const Agent &other):mAgentId(other.mAgentId),mPartyId(other.mPartyId),mSelectionPolicy(other.mSelectionPolicy),colId(other.colId){

    mSelectionPolicy = other.mSelectionPolicy->clone();
}


int Agent::getId() const
{
    return mAgentId;
}

int Agent::getPartyId() const
{
    return mPartyId;
}
//Agent Step
void Agent::step(Simulation &sim)
{
    int id = getPartyId();
    vector<int> neighbours;
    Graph& b = sim.getGraphref();
    // find all nb that not joind 
    for(int i = 0; i< b.getNumVertices(); i++){
        if(b.getEdgeWeight(id, i) != 0 && b.getParty(i).getState() != State::Joined){
            if(b.getPartyref(i).checkUniOffer(this->colId,sim)) {
                neighbours.push_back(b.getParty(i).getId());
                  }
        } 
    }
    int pass = this->mSelectionPolicy->select(neighbours,b, this->getPartyId());
    if(pass!=-1)
    b.getPartyref(pass).acceptOffer(this->getId());

}


int Agent::getColId() const {
    return colId;
}

void Agent::setColId(int _colId){
    colId = _colId;

}

void Agent::setPartyId(int id) {
    this->mPartyId = id;
}
void Agent::setAid(int aid) {
    this->mAgentId = aid;
}

// Agent Move Constructor 
Agent::Agent(Agent &&other) noexcept: mAgentId(other.mAgentId),mPartyId(other.mPartyId),mSelectionPolicy(other.mSelectionPolicy),colId(other.colId){
    other.mSelectionPolicy = nullptr;
}

//Agent copy Assignment
Agent &Agent::operator=(const Agent &other){
    if(this != &other){
       if(mSelectionPolicy) delete mSelectionPolicy;
    mSelectionPolicy = other.mSelectionPolicy->clone();
    colId = other.colId;
    mAgentId = other.mAgentId;
    mPartyId = other.mPartyId;
    }
    return *this;
}
//Agent Move Assignment
Agent &Agent::operator=(Agent &&other) noexcept {
    if(mSelectionPolicy) delete mSelectionPolicy;
    colId = other.colId;
    mSelectionPolicy = other.mSelectionPolicy;
    mAgentId = other.mAgentId;
    mPartyId = other.mPartyId;
    other.mSelectionPolicy = nullptr;
    return *this;
}

//Agent select's party
int Agent::select(vector<int> neighs,Graph &a,int pid){
    return mSelectionPolicy->select(neighs,a,pid);
}
