#pragma once
#include<vector>
#include<iostream>
#include<map>
#include "../include/event.h"
using namespace std;
class GameReports
{
    private: 
    //map<username ------> map<Topic --------> map<time -----> Event > > >;
    std::map<string,map<string,map<int,Event>>> reps;
    std::map<string,bool> half_ch;
    public:
    GameReports();
    void save_report();
    void PREPARE(std::string report,std::string user);
    Event Create_Event(std::string report);
    void spit_report(string topic,string user, string file);
    map<string,map<string,string>> take_give(string inp);
    void trim(string &s);
    map<string,string> sort_lexico(map<string,string> to_sort);
};