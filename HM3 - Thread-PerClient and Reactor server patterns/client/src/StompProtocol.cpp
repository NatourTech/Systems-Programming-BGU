#include "../include/StompProtocol.h"
#include "../include/event.h"
#include <string>
#include <map>
#include <iostream>
#include <vector>
#include <boost/algorithm/string.hpp>
#include <utility>


using boost::asio::ip::tcp;

using std::cin;
using std::cout;
using std::cerr;
using std::endl;
using std::string;
using std::vector;



StompProtocol::StompProtocol():gr(),currUser(""),subscriptions(),sub_unsub(),subscribtionID(0),receiptID(0),desc_rec_id(-1),headers(),headers_reply(),error(false),connected(false){
    this->headers["login"] = 1;
    this->headers["report"] = 2;
    this->headers["join"] = 3;
    this->headers["exit"] = 4;
    this->headers["logout"] = 5;
    this->headers["summary"] = 6;
    this->headers_reply["CONNECTED"] = 0;
    this->headers_reply["MESSAGE"] = 1;
    this->headers_reply["RECEIPT"] = 2;
    this->headers_reply["ERROR"] = 3;
}

std::vector<string> StompProtocol::Proccess(string msg){  
std::vector<string> frames;
vector<std::string> afterSplit = Splitter(msg,' ');
std::string header = afterSplit.at(0);
int val = headers[header];
switch (val)
{
case 1:
    frames.push_back(this->CONNECT(afterSplit));
    break;
case 2:
    frames = this->SEND(afterSplit);
    break;
case 3:
    frames.push_back(this->SUBSCRIBE(afterSplit));
    break;
case 4:
    frames.push_back(this->UNSUBSCRIBE(afterSplit));
    break;
case 5:
    frames.push_back(this->DISCONNECT(afterSplit));
    break;
case 6:
    frames.push_back(this->SUMMARY(afterSplit));
    break;
default:
    break;
}
return frames;
}

//log in
string StompProtocol::CONNECT(std::vector<std::string> msg){
std::ostringstream builder;
if(!connected){
builder << "CONNECT" << "\n" << "accept-version:1.2" <<"\n" << "host:stomp.cs.bgu.ac.il"<<"\n";
currUser = msg[2];
builder <<"login:"<<msg[2]<<"\n";
builder <<"passcode:"<<msg[3]<<"\n\n";
builder<<"\0";
string res = builder.str();
return res;
}
cout<<"The client is already logged in, log out before trying again"<<endl;
return "";
}
//report
std::vector<string> StompProtocol::SEND(std::vector<std::string> msg){
std::ostringstream builder;
std::vector<std::string> listOfFrames;
names_and_events name_ev = parseEventsFile(msg[1]);
string topic = name_ev.team_a_name+"_"+name_ev.team_b_name;
int sub_id = is_subbed(currUser, topic);
if(sub_id != -1){
vector<Event> evs = name_ev.events;
for(unsigned int i = 0 ; i<evs.size();i++){
builder<<"SEND"<<"\n";
builder<<"destination:/" << evs[i].get_team_a_name()<<"_"<<evs[i].get_team_b_name()<<"\n\n";
builder<<evs[i].toString(currUser);
builder<<"\0";
string curr = builder.str();
builder.str("");
listOfFrames.push_back(curr);
}
return listOfFrames;
}
cout<<"Your are not subscribed, can't send report!, please subscribe first"<<endl;
return listOfFrames;
}
//join
string StompProtocol::SUBSCRIBE(std::vector<std::string> msg){
std::ostringstream builder;
int sub_id = is_subbed(currUser,msg[1]);
if (sub_id == -1) { 
builder <<"SUBSCRIBE" << "\n";
builder <<"destination:/"<<msg[1]<<"\n";
builder<<"id:"<<subscribtionID<<"\n";
builder<<"receipt:"<<receiptID<<"\n\n";
builder<<"\0";
string res = builder.str();
sub_unsub[receiptID] = std::make_pair(msg[1], true);
//subscriptions[currUser][msg[1]] = subscribtionID;
//subscribtionID++;
receiptID++;
return res;
}
cout<<"You are already subscribed to this topic !"<<endl;
return "";
}
//exit
string StompProtocol::UNSUBSCRIBE(std::vector<std::string> msg){
std::ostringstream builder;
int sub_id = is_subbed(currUser, msg[1]);
if(sub_id != -1){
builder<<"UNSUBSCRIBE"<<"\n";
builder<<"id:"<<subscriptions[currUser][msg[1]]<<"\n";
builder<<"receipt:"<<receiptID<<"\n\n";
builder<<"\0";
sub_unsub[receiptID] = std::make_pair(msg[1], false);
receiptID++;
string res = builder.str();
return res;
}
cout<<"You are not subscribed to this channel, no need to unsubscribe!"<<endl;
return "";
}
//log out
string StompProtocol::DISCONNECT(std::vector<std::string> msg){
std::ostringstream builder;
//
builder<<"DISCONNECT"<<"\n";
builder<<"receipt:"<<desc_rec_id<<"\n";
builder<<"\0";
string res = builder.str();
desc_rec_id--;
return res;
}
//summary
string StompProtocol::SUMMARY(std::vector<std::string> msg)
{
    string topic = msg[1];
    string user = msg[2];
    string file = msg[3];
    int sub_id = is_subbed(currUser,topic);
    if(sub_id != -1){
    gr.spit_report(topic,user,file);
    }else{cout<<"You are not subscribed to this channel, can't fetch summary...\n";}
    return "";
}

vector<std::string> StompProtocol::Splitter(std::string msg,char deli){
std::vector<std::string> words;

std::istringstream stream(msg);
std::string word;
while (std::getline(stream, word, deli)) 
{
    words.push_back(word);
}
return words;
}

bool StompProtocol::Proccess_reply(std::string msg)
{
    vector<std::string> afterSplit = Splitter(msg,'\n');
    int val = headers_reply[afterSplit[0]];
    switch (val)
    {
    case 0: 
        return this->CONNECTED(afterSplit);
    case 1:
        return this->MESSAGE(msg);
    case 2:
        return this->RECEIPT(afterSplit);
    case 3:
        return this->ERROR(msg);
    default:
        return true;
        break;
    }
}


bool StompProtocol::CONNECTED(std::vector<std::string> msg)
{
    cout<<"Logged in successfully"<<endl;
    this->connected = true;
    return true;
}
bool StompProtocol::MESSAGE(std::string msg)
{
    std::pair<map<std::string,std::string>,std::string> frame_p = FrameExtractor(msg);
    std::string body = frame_p.second;
    std::string user;
    std::size_t idx = body.find("\n");
    if(idx != std::string::npos){
        user = body.substr(0,idx);
        body = body.substr(idx+1);
    }
    gr.PREPARE(body,user);
    return true;
}

bool StompProtocol::RECEIPT(std::vector<std::string> msg)
{
    std::string rec = msg[1];
    std::vector<std::string> recId = Splitter(rec,':');
    int id  = stoi(recId[1]);
    if(id<0)
    {
        cout<<"Loggin out..."<<endl;
        sub_unsub.clear();
        subscriptions[currUser].clear();
        this->connected = false;
        subscribtionID = 0;
        receiptID = 0;
        desc_rec_id = -1;
        return false;
    }
    if (sub_unsub.find(id) != sub_unsub.end())
    {
        std::pair<string, bool> curr = sub_unsub[id];
        if(curr.second){
            subscriptions[currUser][curr.first] = subscribtionID;
            subscribtionID++;
            cout<<"Joined channel "<< curr.first<<endl;
        }else{
            subscriptions[currUser].erase(curr.first);
            cout<<"Exited channel "<< curr.first<<endl;
        }
        sub_unsub.erase(id);
    }
    else{
    cout<<"Received a receipt of id: " << id<<endl;
    }
    return true;
}

bool StompProtocol::ERROR(std::string frame){
    std::pair<map<std::string,std::string>,std::string> p = FrameExtractor(frame);
    std::cout<<"Error received from server...\n"<<std::endl;
    map<std::string,std::string> headers = p.first;
    cout<<"Error message:\n";
    for (const auto &pair : headers) 
    {
    cout<< pair.first << ": " << pair.second <<"\n\n";
    }
    cout<<p.second<<endl;
    cout<<"\n\n"<<"Logging out..."<<endl;
    //subscriptions[currUser].clear();
    this->connected = false;
    return false;
}


std::pair<map<std::string,std::string>,std::string> StompProtocol::FrameExtractor(std::string frame){
size_t headers_end = frame.find("\n");
frame = frame.substr(headers_end +1);
std::string headers_str, body_str;
headers_end = frame.find("\n\n");
headers_str = frame.substr(0, headers_end);
body_str = frame.substr(headers_end+3);

std::map<std::string, std::string> headers;
char *header_str = &headers_str[0];
char *header = strtok(header_str, "\n");
while (header) {
    char *colon = strchr(header, ':');
    if (colon) {
        std::string name(header, colon - header);
        std::string value(colon + 1);
        headers[name] = value;
    }
    header = strtok(nullptr, "\n");
}
return std::make_pair(headers,body_str);
}


int StompProtocol::is_subbed(string user, string topic)
{
    std::map<std::string, int> sub = subscriptions[user];
    if (sub.count(topic))
    {
        int sub_id = sub[topic];
        return sub_id;
    }
    else
    {
        return -1;
    }
}
