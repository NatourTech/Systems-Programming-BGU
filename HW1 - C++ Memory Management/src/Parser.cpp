#include "Parser.h"
#include <iostream>

using std::cout;
using std::endl;

Simulation Parser::readSimulation(const string &path)
{
    std::ifstream inFile(path);
    if (!inFile.is_open())
        throw std::invalid_argument("Could not open file " + path);
    json jSimulation = json::parse(inFile);
    return parseSimulation(jSimulation);
}

Agent Parser::parseAgent(const json &jAgent, int agentId)
{
    SelectionPolicy *sp;
    string selectionPolicy = jAgent["selection_policy"];
    if (selectionPolicy == "M"){
        sp = new MandatesSelectionPolicy;
    }
    else if (selectionPolicy == "E"){
        sp = new EdgeWeightSelectionPolicy;
        }
    else
        throw std::invalid_argument("Invalid selection policy");

    int partyId = jAgent["party_id"];
    return Agent(agentId, partyId, sp);
}

Party Parser::parseParty(const json &jParty, const int id)
{
    JoinPolicy *jp;
    string joinPolicy = jParty["join_policy"];
    if (joinPolicy == "M")
    {
        jp = new MandatesJoinPolicy;
    }
    else if (joinPolicy == "L")
    {
        jp = new LastOfferJoinPolicy;
    }
    else
        throw std::invalid_argument("Invalid join policy");

    string name = jParty["name"];
    int mandates = jParty["mandates"];
    return Party(id, name, mandates, jp);
}

Graph Parser::parseGraph(const json &jGraph, const vector<Party> &parties)
{
    vector<vector<int>> edges = jGraph;

    // make sure all size fits the number of parties
    if (edges.size() != parties.size())
        throw std::invalid_argument("The graph does not match the parties number");

    // make sure that the matrix is square
    for (auto &edge : edges)
    {
        if (edge.size() != parties.size())
            throw std::invalid_argument("The graph does not match the parties number");
    }

    // make sure that the matrix is symmetric
    for (unsigned int i = 0; i < edges.size(); ++i)
    {
        for (unsigned int j = 0; j < i; ++j)
        {
            if (edges[i][j] != edges[j][i])
                throw std::invalid_argument("The graph is not symmetric");
        }
    }

    return Graph(parties, edges);
}

Simulation Parser::parseSimulation(const json &jSimulation)
{
    // create agents list
    vector<Agent> agents;
    for (unsigned int i = 0; i < jSimulation["agents"].size(); ++i){
        agents.push_back(parseAgent(jSimulation["agents"][i], i));
    }

    // create parties list
    vector<Party> parties;
    for (unsigned int i = 0; i < jSimulation["parties"].size(); ++i)
        parties.push_back(parseParty(jSimulation["parties"][i], i));

    // change the state for the parties that are initialized with agent
    for (auto &agent : agents)
        parties[agent.getPartyId()].setState(State::Joined);

    // verify that the number of mandates sums to 120
    int mandatesSum = 0;
    for (auto &party : parties)
        mandatesSum += party.getMandates();
    if (mandatesSum != 120)
        throw std::invalid_argument("The parties mandates sum is not 120");

    // create graph
    Graph g = parseGraph(jSimulation["graph"], parties);
    return Simulation(g, agents);
}

string stateToString(State state)
{
    switch (state)
    {
    case State::Waiting:
        return "Waiting";
    case State::Joined:
        return "Joined";
    case State::CollectingOffers:
        return "CollectingOffers";
    default:
        return "Unknown";
    }
}

json Parser::makeJson(const Simulation &s)
{
    json out;
    const Graph &g = s.getGraph();
    // we want to save for each party: id, state, mandates and name
    vector<json> parties;
    for (int i = 0; i < s.getGraph().getNumVertices(); i++)
    {
        json party;
        party["id"] = i;
        party["state"] = stateToString(s.getParty(i).getState());
        party["mandates"] = s.getGraph().getMandates(i);
        party["name"] = s.getParty(i).getName();
        parties.push_back(party);
    }
    out["parties"] = parties;

    // we want to save for each agent: partyId
    vector<json> agents;
    for (const Agent &agent : s.getAgents())
    {
        json jAgent;
        jAgent["id"] = agent.getId();
        jAgent["partyId"] = agent.getPartyId();
        agents.push_back(jAgent);
    }
    out["agents"] = agents;

    // we want to save for each coalition: list of parties and number of mandates
    vector<json> coalitions;
    const vector<vector<int>> coalitionsParties = s.getPartiesByCoalitions();
    for (size_t i = 0; i < coalitionsParties.size(); i++)
    {
        json coalition;

        vector<int> partiesIds = coalitionsParties[i];

        int mandates = 0;
        for (int &partyId : partiesIds)
        {
            mandates += s.getGraph().getMandates(partyId);
        }
        coalition["parties"] = partiesIds;
        coalition["mandates"] = mandates;
        coalitions.push_back(coalition);
    }
    out["coalitions"] = coalitions;

    // we want to save for edges: from, to, weight
    vector<json> edges;
    for (int i = 0; i < g.getNumVertices(); i++)
    {
        for (int j = 0; j < g.getNumVertices(); j++)
        {
            if (g.getEdgeWeight(i, j) > 0)
            {
                json edge;
                edge["from"] = i;
                edge["to"] = j;
                edge["weight"] = g.getEdgeWeight(i, j);
                edges.push_back(edge);
            }
        }
    }
    out["edges"] = edges;

    return out;
}