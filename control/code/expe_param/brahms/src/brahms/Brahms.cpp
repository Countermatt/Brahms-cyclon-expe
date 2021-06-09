#include "data/Data.hpp"
#include "brahms/Brahms.hpp"

#include <iostream>
#include <ostream>

Brahms::Brahms()
{
  mAL1 = 0;
  mBL2 = 0;
  mGL3 = 0;
  mSamplerSize = 0;
}

void Brahms::SetAL1(int al1)
{
  mAL1 = al1;
}

void Brahms::SetBL2(int bl2)
{
  mBL2 = bl2;
}

void Brahms::SetGL3(int gl3)
{
  mGL3 = gl3;
}

void Brahms::SetSamplerSize(int samplersize)
{
  mSamplerSize = samplersize;
}
//Push
int Brahms::Push_Request(char** req,
                 unsigned int* reqSize,
                 char** destIP,
                 unsigned int* destIPSize,
                 int* destPort,
                 Data* data)
{
  auto View = data->GlobalView();
  auto myIP = data->MyIP();



  //Choix du destinataire
  std::string dIP;
  std::string dPort;
  std::vector<std::string> IPPort;

  dIP = data->RandomElement(View);
  splitString(IPPort, dIP, ':');

  //Préparation IP et Port
  *destIPSize = IPPort[0].size();
  *destIP = new char[*destIPSize];
  IPPort[0].copy(*destIP, *destIPSize);

  dPort = IPPort[1];
  *destPort = std::stoi(dPort);


  //Construction de la requête
  std::string reqIP;
  reqIP.append(data->Identifiant());
  reqIP.append(";");
  reqIP.append("PushRequest");
  reqIP.append(";");
  reqIP.append(myIP);


  *reqSize = reqIP.size();
  *req = new char[*reqSize];
  reqIP.copy(*req, *reqSize);

  return 0;
}

int Brahms::Push_Receive_Request(std::string ans,
                                 Data* data)
{
  data->PushAdd(ans);
  data->StreamAdd(ans);
  return 0;
}


//Pull
int Brahms::Pull_Request(char** req,
                 unsigned int* reqSize,
                 Data* data)
{

  auto View = data->GlobalView();
  //Mise en place de la requête
  std::string reqIP;

  reqIP.append(data->Identifiant());
  reqIP.append(";");
  reqIP.append("PullRequest");
  reqIP.append(";");
  reqIP.append("0.0.0.0");
  *reqSize = reqIP.size();
  *req = new char[*reqSize];
  reqIP.copy(*req, *reqSize);

  return 0;
}

int Brahms::Pull_Reply(char** ans,
               unsigned int* ansSize,
               Data* data)
{

  auto View = data->GlobalView();
  auto myIP = data->MyIP();

  std::string reqIP;
  reqIP.append(data->Identifiant());
  reqIP.append(";");
  reqIP.append("PullReply");
  reqIP.append(";");

  int viewsize = View.size();
  for(int i = 0; i < viewsize; i++){
      reqIP.append(View[i]);
      reqIP.append(",");

  }


  reqIP.append(myIP);

  *ansSize = reqIP.size();
  *ans = new char[*ansSize];
  reqIP.copy(*ans, *ansSize);

  return 0;
}

int Brahms::Pull_Reply_Byz(char** ans,
               unsigned int* ansSize,
               Data* data)
{

  auto View = data->GlobalView();
  auto myIP = data->MyIP();

  std::string reqIP;
  reqIP.append(data->Identifiant());
  reqIP.append(";");
  reqIP.append("PullReply");
  reqIP.append(";");

  int viewsize = View.size();
  for(int i = 0; i < viewsize; i++){
      reqIP.append(myIP);
      reqIP.append(",");

  }


  reqIP.append(myIP);

  *ansSize = reqIP.size();
  *ans = new char[*ansSize];
  reqIP.copy(*ans, *ansSize);

  return 0;
}

int Brahms::Pull_Receive_Reply(std::string ans,
                               Data* data)
{
  std::vector<std::string> View;

  splitString(View, ans, ',');


  for(int i = 0; i < (int) View.size(); i++){
    data->PullAdd(View[i]);
    data->StreamAdd(View[i]);
  }

  return 0;
}

int Brahms::Pull_Init_Request(char** req,
              unsigned int* reqSize,
              char** destIP,
              unsigned int* destIPSize,
              int* destPort,
              Data* data)
{
  auto View = data->GlobalView();

  //Choix du destinataire
  std::string dIP;
  std::string dPort;
  std::vector<std::string> IPPort;

  dIP = data->RandomElement(View);
  splitString(IPPort, dIP, ':');

  //Préparation IP et Port
  *destIPSize = IPPort[0].size();
  *destIP = new char[*destIPSize];
  IPPort[0].copy(*destIP, *destIPSize);

  dPort = IPPort[1];
  *destPort = std::stoi(dPort);



  //Mise en place de la requête
  std::string reqIP;

  reqIP.append(data->Identifiant());
  reqIP.append(";");
  reqIP.append("PullInitRequest");
  reqIP.append(";");
  reqIP.append("0.0.0.0");

  *reqSize = reqIP.size();
  *req = new char[*reqSize];
  reqIP.copy(*req, *reqSize);

  return 0;
}

int Brahms::Pull_Init_Reply(char** ans,
                    unsigned int* ansSize,
                    Data* data)
{

  std::string reqIP;
  reqIP.append(data->Identifiant());
  reqIP.append(";");
  reqIP.append("PullInitReply");
  reqIP.append(";");
  reqIP.append("0.0.0.0");

  *ansSize = reqIP.size();
  *ans = new char[*ansSize];
  reqIP.copy(*ans, *ansSize);

  return 0;
}

//Sample Ping Pong
int Brahms::Ping(char** req,
                 unsigned int* reqSize,
                 char** destIP,
                 unsigned int* destIPSize,
                 int* destPort,
                 Data* data)
{
  auto View = data->GlobalView();
  auto myIP = data->MyIP();

  //Choix du destinataire
  std::string dIP;
  std::string dPort;
  std::vector<std::string> IPPort;

  dIP = data->RandomElement(View);
  splitString(IPPort, dIP, ':');

  //Préparation IP et Port
  *destIPSize = IPPort[0].size();
  *destIP = new char[*destIPSize];
  IPPort[0].copy(*destIP, *destIPSize);

  dPort = IPPort[1];
  *destPort = std::stoi(dPort);

  //Construction de la requête
  std::string reqIP;
  reqIP.append(data->Identifiant());
  reqIP.append(";");
  reqIP.append("Ping");
  reqIP.append(";");
  reqIP.append(myIP);

  *reqSize = reqIP.size();
  *req = new char[*reqSize];
  reqIP.copy(*req, *reqSize);

  return 0;
}

int Brahms::Pong(char** ans,
               unsigned int* ansSize,
               Data* data)
{
  auto View = data->GlobalView();
  auto myIP = data->MyIP();

  std::string reqIP;
  reqIP.append(data->Identifiant());
  reqIP.append(";");
  reqIP.append("Pong");
  reqIP.append(";");
  reqIP.append(myIP);

  *ansSize = reqIP.size();
  *ans = new char[*ansSize];
  reqIP.copy(*ans, *ansSize);

  return 0;

}

//Merge View
void Brahms::MergeView(int aL1,
               int bL2,
               int gL3,
               Data* data)
{

  auto pullView = data->PullView();
  auto pushView = data->PushView();
  auto sampleView = data->SamplerView();

  data->GlobalReset();
  int i =0;
  if(pushView.size() != 0){
    while( i < aL1){
      data->GlobalAdd(data->RandomElement(pushView));
      i++;
    }
  }
  i = 0;
  if(pullView.size() != 0){
    while( i < bL2){
      data->GlobalAdd(data->RandomElement(pullView));
      i++;
    }
  }
  i = 0;
  if(sampleView.size() != 0){
    while( i < gL3){
      data->GlobalAdd(data->RandomElement(sampleView));
      i++;
    }
  }
}

int Brahms::Cyclon_Request(char** req,
                   unsigned int* reqSize,
                   Data* data,
                   int cyclonSize)
{
  auto View = data->GlobalView();

  //Mise en place de la requête
  std::string reqIP;

  reqIP.append(data->Identifiant());
  reqIP.append(";");
  reqIP.append("CyclonRequest");
  reqIP.append(";");

  for(int i = 0; i < cyclonSize; i++){
      reqIP.append(data->RandomElement(View));
      reqIP.append(",");
  }

  *reqSize = reqIP.size();
  *req = new char[*reqSize];
  reqIP.copy(*req, *reqSize);

  return 0;
}

int Brahms::Cyclon_Reply(char** ans,
                 unsigned int* ansSize,
                 Data* data,
                 int cyclonSize)
{

  auto View = data->GlobalView();
  auto myIP = data->MyIP();

  std::string reqIP;
  reqIP.append(data->Identifiant());
  reqIP.append(";");
  reqIP.append("CyclonReply");
  reqIP.append(";");

  for(int i = 0; i < cyclonSize; i++){
      reqIP.append(data->RandomElement(View));
      reqIP.append(",");
  }

  reqIP.append(myIP);

  *ansSize = reqIP.size();
  *ans = new char[*ansSize];
  reqIP.copy(*ans, *ansSize);

  return 0;
}

int Brahms::Cyclon_Pull_Receive_Reply(std::string ans,
                               Data* data)
{
  std::vector<std::string> View;

  splitString(View, ans, ',');


  for(int i = 0; i < (int) View.size(); i++){
    data->PullAdd(View[i]);
    data->StreamAdd(View[i]);
  }

  return 0;
}
