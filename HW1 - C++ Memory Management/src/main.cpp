#include <iostream>
#include <iomanip>
#include "Parser.h"



using std::cout;
using std::endl;

int main(int argc, char **argv)
{

    if (argc != 2)
    {
        cout << "usage: " << argv[0] << " <config_path>" << endl;
        return -1;
    }

    // read simulation from config file
    const string config_path = argv[1];
    Simulation simulation = Parser::readSimulation(argv[1]);


    // run simulation and store json state after each iteration
    vector<json> outPerIter = {Parser::makeJson(simulation)};
    
    while (!simulation.shouldTerminate())
    {
        simulation.step();
        outPerIter.push_back(Parser::makeJson(simulation));
    }

    // writing the outputs list to a file
    const string output_path = config_path.substr(0, config_path.find_last_of('.')) + ".out";
    std::ofstream outputFile(output_path);
    outputFile << std::setw(4) << json(outPerIter) << endl;
    return 0;
}