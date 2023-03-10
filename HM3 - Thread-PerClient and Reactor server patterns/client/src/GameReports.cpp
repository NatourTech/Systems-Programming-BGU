#include "../include/GameReports.h"
#include <string>
#include <map>
#include <iostream>
#include <sstream>
#include <vector>
#include <boost/algorithm/string.hpp>
#include <utility>
#include <fstream>
using std::cin;
using std::cout;
using std::cerr;
using std::endl;
using std::string;
using std::vector;

GameReports::GameReports():reps(),half_ch(){}

void GameReports::save_report(){}
void GameReports::PREPARE(std::string report,std::string user) 
{
    string leftRep;
    string general_ups;
    string team_a_ups;
    string team_b_ups;
    int idx = user.find(":");
    std::string u = user.substr(idx+2);

    Event ev = Create_Event(report);
    this->reps[u][ev.get_team_a_name()+"_"+ev.get_team_b_name()][ev.get_time()] = ev;

}
Event GameReports::Create_Event(std::string report)
{
    string leftRep;
    std::map<string,string> kv;
    std::map<string,string> team_a_updates;
    std::map<string,string> team_b_updates;
    std::map<string,string> general_game_updates;
    string desc;
    int idx2 = report.find("general game updates:");
    string curr = report.substr(0,idx2);
    leftRep = report.substr(idx2);
    std::stringstream ss(curr);
    std::string line;
    while (std::getline(ss, line, '\n')) {
    std::size_t pos = line.find(':');
    std::string key = line.substr(0, pos);
    std::string value = line.substr(pos + 2);
    kv[key] = value;
    }
    int idx3 = leftRep.find("team a updates:");
    curr = leftRep.substr(0,idx3);
    leftRep = leftRep.substr(idx3);
    general_game_updates = take_give(curr)["general game updates"];

    int idx4 = leftRep.find("team b updates:");
    curr = leftRep.substr(0,idx4);
    leftRep = leftRep.substr(idx4);
    team_a_updates = take_give(curr)["team a updates"];

    
    int idx5 = leftRep.find("description:");
    curr = leftRep.substr(0,idx5);
    leftRep = leftRep.substr(idx5);
    team_b_updates = take_give(curr)["team b updates"];


    int idx6 = leftRep.find("\n");
    desc = leftRep.substr(idx6+1);


    int time = std::stoi(kv["time"]);
    Event ev(kv["team a"],kv["team b"], kv["event name"],time,general_game_updates,team_a_updates,team_b_updates,desc);
    string topic  = ev.get_team_a_name() + "_" + ev.get_team_b_name();
    if(half_ch.find(topic) == half_ch.end())
    {
    half_ch[topic] = true;
    }
    ev.beforeHalf = half_ch[topic];
    if(general_game_updates.find("\tbefore halftime") != general_game_updates.end() && general_game_updates["\tbefore halftime"] == "false")
    {
        half_ch[topic] = false;
    }

    if(!ev.beforeHalf)
    {
        ev.update_time(ev.get_time() + 10000);
    }
    return ev;

}
void GameReports::spit_report(string topic,string user, string file)
{
    std::ostringstream builder;
    int idx = topic.find("_");
    string team_a = topic.substr(0,idx);
    string team_b = topic.substr(idx+1);
    std::map<string,string> gn_stats;
    std::map<string,string> team_a_stats;
    std::map<string,string> team_b_stats;
    map<int ,map<string, string>> events_time_desc;

    std::map<int, Event> evs = reps[user][topic];
    std::vector<std::pair<int, Event>> vec(evs.begin(), evs.end());
    std::sort(vec.begin(), vec.end(), [](auto& lhs, auto& rhs) { return lhs.first < rhs.first; });
    for(long unsigned int i = 0 ; i<vec.size();i++)
    {
        Event curr = vec[i].second;
        if(curr.beforeHalf == false) curr.update_time(curr.get_time()-10000);
        events_time_desc[curr.get_time()][curr.get_name()] = curr.get_discription();
        map<string,string> curr_gn_ups = curr.get_game_updates();
        map<string,string> curr_ga_ups = curr.get_team_a_updates();
        map<string,string> curr_gb_ups = curr.get_team_b_updates();
        for (const auto &pair : curr_gn_ups)
        {
            gn_stats[pair.first] = pair.second;
        }
        for (const auto &pair : curr_ga_ups)
        {
            team_a_stats[pair.first] = pair.second;
        }
        for (const auto &pair : curr_gb_ups)
        {
            team_b_stats[pair.first] = pair.second;
        }

    }
    builder<<team_a <<" vs "<<team_b<<"\n";
    builder<<"Game stats:"<<"\n";
    builder<<"General stats:"<<"\n";
    map<string,string> gn_stats_sorted = this->sort_lexico(gn_stats);
    for (const auto &pair : gn_stats_sorted)
    {
        builder<<pair.first<<": "<<pair.second<<"\n";
    }
    builder<<team_a<<" stats:"<<"\n";
    map<string,string> team_a_stats_sorted = this->sort_lexico(team_a_stats);
    for (const auto &pair : team_a_stats_sorted)
    {
        builder<<pair.first<<": "<<pair.second<<"\n";
    }

    builder<<team_b<<" stats:"<<"\n";
    map<string,string> team_b_stats_sorted = this->sort_lexico(team_b_stats);
    for (const auto &pair : team_b_stats_sorted)
    {
        builder<<pair.first<<": "<<pair.second<<"\n";
    }
    builder<<"Game event reports:"<<"\n";
    for (const auto &pair : events_time_desc)
    {
        string name;
        string desc;
        map<string, string> nm_desc = pair.second;
        for(auto& pair: nm_desc) 
        {
             name = pair.first;
             desc = pair.second;
        }

        builder<<pair.first<<" - "<<name<<":"<<"\n\n";
        builder<<desc<<"\n\n";
    }

    string full_rep = builder.str();
    std::ofstream file_e(file); // Create a new file
    if (file_e.is_open()) {
        file_e << full_rep;
        file_e.close();
        std::cout << "File created and report written to " << file << std::endl;
    } else {
        std::cout << "Unable to create/open file " << file << std::endl;
    }
}





map<string,map<string,string>> GameReports::take_give(string inp)
{
   map<string,map<string,string>> result;

   std::stringstream ss(inp);
   string line, key;
   getline(ss,key,':');
   trim(key);
   while(getline(ss,line)){
       if(line.size() >0){
       stringstream linestream(line);
       string innerkey,innervalue;
       getline(linestream,innerkey,':');
       trim(innerkey);
       getline(linestream,innervalue);
       trim(innervalue);
       result[key][innerkey] = innervalue;
       }
   }
return result;

}


void GameReports::trim(string &s)
{
    size_t first = s.find_first_not_of(' ');
    size_t last = s.find_last_not_of(' ');
    s=s.substr(first,(last-first+1));
}



map<string,string> GameReports::sort_lexico(map<string,string> to_sort)
{
    std::vector<std::string> keys;
    for(const auto& item : to_sort) {
        keys.push_back(item.first);
    }
    std::sort(keys.begin(), keys.end());
    std::map<std::string, string> sorted;
    for (const auto& key : keys) {
        sorted[key] = to_sort[key];
    }
    return sorted;
}