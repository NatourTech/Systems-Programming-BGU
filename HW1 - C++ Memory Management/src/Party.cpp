#include "Party.h"
#include <utility>
#include <vector>
#include "Agent.h"
#include "Simulation.h"
#include "JoinPolicy.h"


//test
#include <iostream>
using std::cout;
using std::endl;
using std::vector;

//Party Constructor
Party::Party(int id, string name, int mandates, JoinPolicy *jp) : mId(id), mName(name), mMandates(mandates), mJoinPolicy(jp), mState(Waiting),colId(-1),iteration(0),agColOffers()
{
}

State Party::getState() const
{
    return mState;
}

void Party::setState(State state)
{
    mState = state;
}

int Party::getMandates() const
{
    return mMandates;
}

const string & Party::getName() const
{
    return mName;
}
int Party::getId() const
{
    return mId;
}
void Party::setColId(int NcId) {
    this->colId = NcId;
}

int Party::getColId() const {
    return colId;
}

//Party check's if an offer not received in the past
bool Party::checkUniOffer(int col,Simulation &sim) {
    vector<Agent> agents = sim.getAgents();
    for(int agntId : agColOffers){

        if(agents[agntId].getColId() == col ) return false;
    }
    return true;
    
}
//Party add an offer to Party vector
void Party::addOffer(int Aid) {
    this->agColOffers.push_back(Aid);
}


//Party Step
void Party::step(Simulation &s)
{
    if(getState() == State::CollectingOffers){
        iteration++;
        if(iteration == 3){
            //get the selected col's agent (id) according joinPolicy
            int selected = this->mJoinPolicy->pick(agColOffers,s);
            int selectedColId = s.getAgent(selected).getColId();
            Coalition& ref = s.getCol(selectedColId);

            //add party details to new join col
            ref.add(this->getId(),this->getMandates());
            //update party col Id
            this->setColId(selectedColId);
            
            // clone Agent to the party
            Agent cloned = s.getAgent(selected);
            cloned.setPartyId(this->getId());
            cloned.setAid(s.getAgents().size());
            s.pushClone(cloned);
            this->setState(State::Joined);
        }
    }
}
//Party add offer for agent offers vector and change state if nedded
void Party::acceptOffer(int agid){
    if(mState == State::Waiting){
        setState(State::CollectingOffers);
    }
    agColOffers.push_back(agid);
}

//Party destructor
Party::~Party()
{if(mJoinPolicy) delete mJoinPolicy;}


Party::Party(const Party &other):mId(other.mId),mName(other.mName),mMandates(other.mMandates),mJoinPolicy(other.mJoinPolicy),mState(other.getState()),colId(other.colId),iteration(other.iteration),agColOffers(other.agColOffers){
mJoinPolicy = other.mJoinPolicy->clone();
}

// Party Move Constructor 
Party::Party(Party &&other) noexcept : mId(other.mId),mName(other.mName),mMandates(other.mMandates),mJoinPolicy(other.mJoinPolicy),mState(other.getState()),colId(other.colId),iteration(other.iteration),agColOffers(other.agColOffers){
    other.mJoinPolicy = nullptr;
}

//Party copy Assignment
Party &Party::operator=(const Party &other) {
    if(this != &other){
        if(mJoinPolicy) delete mJoinPolicy;
        mJoinPolicy = other.mJoinPolicy->clone();
        mId = other.mId;
        mName = other.mName;
        mMandates = other.mMandates;
        mState = other.getState();
        colId = other.colId;
        iteration = other.iteration;
        agColOffers = other.agColOffers;
    }
    return *this;
}
//Party Move Assignment
Party &Party::operator=(Party &&other) noexcept {
    if(mJoinPolicy) delete mJoinPolicy;
    mId = other.mId;
    mName = other.mName;
    mMandates = other.mMandates;
    mState = other.getState();
    colId = other.colId;
    iteration = other.iteration;
    agColOffers = other.agColOffers;
    other.mJoinPolicy = nullptr;
    return *this;
}
