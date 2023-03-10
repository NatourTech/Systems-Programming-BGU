#pragma once

#include "../include/ConnectionHandler.h"
#include <vector>
#include <utility>
#include "../include/GameReports.h"
// TODO: implement the STOMP protocol
using std::vector;
//client recieves predefined message from terminal
//protocol shall proccess  the terminal message and parse it to be a legal frame with headers and body
//
class StompProtocol
{
private:
//recipt id DS - (printing sub and unsub)
GameReports gr;
string currUser;
std::map<string,map<string,int>> subscriptions;
std::map<int,pair<string,bool>> sub_unsub;
int subscribtionID;
int receiptID;
int desc_rec_id;
std::map<string,int> headers;
std::map<string,int> headers_reply;

public:
bool error;
bool connected;
StompProtocol();
//terminal msg
std::vector<string> Proccess(std::string msg);
bool Proccess_reply(std::string msg);
vector<std::string> Splitter(std::string msg, char deli);

//log in
string CONNECT(std::vector<std::string> msg);
//report
std::vector<string> SEND(std::vector<std::string> msg);
//join
string SUBSCRIBE(std::vector<std::string> msg);
//exit
string UNSUBSCRIBE(std::vector<std::string> msg);
//log out
string DISCONNECT(std::vector<std::string> msg);
//summary
string SUMMARY(std::vector<std::string> msg);
//-----------------server replies----------
bool CONNECTED(std::vector<std::string> msg);
bool MESSAGE(std::string msg);
bool RECEIPT(std::vector<std::string> msg);
bool ERROR(std::string msg);


std::pair<map<string,string>,std::string> FrameExtractor(std::string frame);

int is_subbed(string user, string topic);
};
