#pragma once
#include <string>
#include <iostream>
#include <fstream>

#include "json.hpp"
#include "Simulation.h"
#include "Agent.h"
#include "SelectionPolicy.h"
#include "JoinPolicy.h"
#include "Coalitions.h"
#include "Graph.h"
#include "Parser.h" 

using json = nlohmann::json;
using std::string;

class Parser
{
public:
    static Simulation readSimulation(const string &path);
    static json makeJson(const Simulation &s);

private:
    static Simulation parseSimulation(const json &jSimulation);
    static Graph parseGraph(const json &jGraph, const vector<Party> &parties);
    static Agent parseAgent(const json &jAgent, int agentId);
    static Party parseParty(const json &jParty, const int id);
};