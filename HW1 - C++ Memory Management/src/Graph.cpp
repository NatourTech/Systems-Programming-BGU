#include "Graph.h"

//test
#include <iostream>
using std::cout;
using std::endl;

Graph::Graph(vector<Party> vertices, vector<vector<int>> edges) : mVertices(vertices), mEdges(edges) 
{
}

int Graph::getMandates(int partyId) const
{
    return mVertices[partyId].getMandates();
}

int Graph::getEdgeWeight(int v1, int v2) const
{
    return mEdges[v1][v2];
}

int Graph::getNumVertices() const
{
    return mVertices.size();
}

const Party & Graph::getParty(int partyId) const {
    return mVertices[partyId];
}

Party & Graph::getPartyref(int partyId) {

    return mVertices[partyId];
    
}

vector<Party> Graph::getVertices() const {
    return mVertices;
}
