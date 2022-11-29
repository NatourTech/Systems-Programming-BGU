#pragma once
#include <vector>

using std::vector;

class Coalition {
    public:
        Coalition(int coll_id);
        
        void add(int pid, int _mandates);
        int getMandates() const;
        vector<int> getParties();
        const vector<int> getParties() const;

    private:
        vector<int> collParties;
        int mandates;
        int id;        
    
};