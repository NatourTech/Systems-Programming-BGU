#pragma once
#include <string>
#include <vector>

using std::string;
using std::vector;

class JoinPolicy;
class Simulation;

enum State
{
    Waiting,
    CollectingOffers,
    Joined
};

class Party
{
public:
    Party(int id, string name, int mandates, JoinPolicy *); 

    void acceptOffer(int agid);
    State getState() const;
    
    void setState(State state);

    int getMandates() const;

    void step(Simulation &s);

    const string &getName() const;

    int getId() const;

    void setColId(int NcId);

    int getColId() const;

    bool checkUniOffer(int col,Simulation &sim);

    void addOffer(int Aid);

    //dest
    virtual ~Party();
    //copy cons
    Party(const Party &other);
    //copy assign oper
    Party& operator=(const Party& other);
    //move assign oper
    Party& operator=(Party&& other) noexcept ;
    //move const
    Party(Party&& other) noexcept ;

private:
    int mId;
    string mName;
    int mMandates;
    JoinPolicy *mJoinPolicy{};
    State mState;

    // WE ADDED
    int colId;
    int iteration;
    vector<int> agColOffers;

};
