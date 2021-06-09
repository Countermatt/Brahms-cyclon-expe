#include <vector>
#include <random>
#include <sstream>
#include <exception>
#include <ctime>
#include <cstring>
#include <iostream>
#include <fstream>

#include "net/TCPListener.hpp"
#include "net/TCPConnection.hpp"
#include "data/Data.hpp"
#include "brahms/Brahms.hpp"

#include "utils/log.hpp"
#include "utils/string.hpp"
#include "RPS.hpp"

//color text
#define RED   "\x1B[31m"
#define GRN   "\x1B[32m"
#define RESET "\x1B[0m"

using namespace std;

RPS::RPS(int rpsTcpPort, std::string bootstrapIP, int nb_turn, int rps_period,
         int sgx, std::string myIP, int al, int bl, int gl, std::string path,
        int byzantin, int samplersize)
    : mListener(make_shared<TCPListener>(rpsTcpPort)),
    mRpsTcpPort(rpsTcpPort), mPushReqCnt(0), mPullReqCnt(0),
    mPingReqCnt(0), mData(), mBrahms(), mNbTurn(nb_turn),
    mRPSPeriod(rps_period), mSGX(sgx), mterminated(1), mByzantin(byzantin), mByzAttack(0)
{
  mDataPath = path;
  stringstream ss;
  mBrahms.SetAL1(al);
  mBrahms.SetBL2(bl);
  mBrahms.SetGL3(gl);
  mBrahms.SetSamplerSize(samplersize);
  std::vector<std::string> btstIP;
  splitString(btstIP, bootstrapIP, ',');

  mData.SetIdentifiant(mSGX);
  mData.SetMyIP(myIP);

  for(int i = 0; i < (int) btstIP.size(); i++){
    mData.GlobalAdd(btstIP[i]);
  }



  mListenerThread = thread(&RPS::listeningThread, this);
  std::this_thread::sleep_for(std::chrono::milliseconds(1000 * mRPSPeriod));
  mSenderThread = thread(&RPS::sendingThread, this);


  ss << "RPS initialized" << endl;
  LOG(ss.str().c_str());
}

void RPS::sendingThread()      // initiate RPS Request every RPS_SYNC_TIME
{
  stringstream ss;
  ss << RED << "SendingThread initialized" << RESET << endl;

  auto al1 = mBrahms.AL1();
  auto bl2 = mBrahms.BL2();
  auto gl3 = mBrahms.GL3();

  int tour = 0;
  int samplersize = mBrahms.SamplerSize();

    string message;
    message.clear();
    for(int k = 0; k < (int) mData.GlobalView().size(); k++){
      message << mData.GlobalView()[k];
      if(k < (int) mData.GlobalView().size() - 1){
        message << ",";
      }
    }
    message << "\n";
    message.close();


  while(tour < mNbTurn){
    ss << RED << "SendingThread tour: " << tour <<" sur "<< mNbTurn << RESET << endl;
    //Reset view

    if(al1 != 0){
      for(int i = 0; i < al1; i++){
        RPS::pushRequest();
      }
    }

    ss << RED << "fin push \n" << RESET << endl;

    if(bl2 != 0){
      for(int i = 0; i < bl2; i++){
        RPS::initPullRequest();
      }
    }

  ss << RED << "fin pull \n" << RESET << endl;

    for(int i = 0; i < samplersize; i++){
      RPS::samplerRequest(i);
    }

  ss << RED << "fin sampler \n" << RESET << endl;
  LOG(ss.str().c_str());

  mBrahms.MergeView(al1, bl2, gl3, &mData);

  //write globalview to data.csv
    for(int k = 0; k < (int) mData.GlobalView().size(); k++){
      message << mData.GlobalView()[k];
      if(k < (int) mData.GlobalView().size() - 1){
        message << ",";
      }
    }
    message << "\n";

  mData.PullReset();
  mData.PushReset();
  mData.StreamReset();

  std::this_thread::sleep_for(std::chrono::milliseconds(1000 * mRPSPeriod));
  tour++;
  mByzAttack = 0;
}

file.open(mDataPath ,ios::out | ios::app);
if (!file) {
    cout << "No such file";
}
else{
  file << message;
  file.close();
}
  ss << RED << "SendingThread Terminated" << RESET << endl;

  LOG(ss.str().c_str());
}

void RPS::listeningThread()
{
  stringstream ss;
  ss << GRN << "ListeningThread initialized" << RESET << endl;
  LOG(ss.str().c_str());
  int k = 0;
  while(k == 0){
      //Verify if listening thread is closed
      if (mListener->IsClosed()) {
        break;
      }
      shared_ptr<TCPConnection> conn;
      try {
        conn = mListener->Accept();
        ReceiveRequest(conn);
      } catch (exception &e) {
        stringstream ss;
        ss << GRN << "Error accepting connection: " << RESET << e.what();
        LOG(ss.str().c_str());
        continue;
      }
  }
  ss << GRN << "ListeningThread Terminated" << RESET << endl;
  LOG(ss.str().c_str());
}

void RPS::pushRequest()
{
  stringstream ss;

  char* req = nullptr;
  unsigned int reqSize = 0;
  char* destIP = nullptr;
  unsigned int destIPSize = 0;
  int destPort = 0;
  int res = 0;

  //Mise en place de la push request
  res = mBrahms.Push_Request(&req, &reqSize, &destIP, &destIPSize, &destPort, &mData);

  if (res != 0) {
    ss << "Error creating Push-brahms_REQ " << endl;
    LOG(ss.str().c_str());
    return;
  }

  auto qIP = string(destIP, destIPSize);
  if (destIP != nullptr) delete[] destIP;

  shared_ptr<TCPConnection> conn;
  try {
    conn = TCPConnection::Dial(qIP, destPort);
  } catch (exception& e) {
    ss << "Error dialing " << qIP << ": " << e.what();
    LOG(ss.str().c_str());
    if (req != nullptr) delete[] req;
    return;
  }

  try {
    conn->Write(req, reqSize);
  } catch (exception& e) {
    ss << "Error sending Push-brahms_REQ " << endl;
    LOG(ss.str().c_str());
    if (req != nullptr) delete[] req;
    return;
  }

  ss << "Sent Push_REQ " << endl;
  ss.clear();
  ss.str("");

}

void RPS::initPullRequest()
{
  stringstream ss;

  char* req = nullptr;
  unsigned int reqSize = 0;
  char* destIP = nullptr;
  unsigned int destIPSize = 0;
  int destPort = 0;
  int res = 0;

  //Mise en place de la pull request
  res = mBrahms.Pull_Init_Request(&req, &reqSize, &destIP, &destIPSize, &destPort, &mData);

  if (res != 0) {
    ss << "Error creating Pull-brahms_REQ" << endl;
    LOG(ss.str().c_str());
    return;
  }
  auto qIP = string(destIP, destIPSize);
  if (destIP != nullptr) delete[] destIP;

  res = 0;
//mise en place de la connection
  shared_ptr<TCPConnection> conn;
  try {
    conn = TCPConnection::Dial(qIP, destPort);
  } catch (exception& e) {
    ss << "Error dialing " << qIP << ": " << e.what();
    LOG(ss.str().c_str());
    if (req != nullptr) delete[] req;
    return;
  }

//envoi du pull init
  try {
    conn->Write(req, reqSize);
  } catch (exception& e) {
    ss << "Error sending Pull-init_REQ " << endl;
    LOG(ss.str().c_str());
    if (req != nullptr) delete[] req;
    return;
  }

  //Receive pull init Answer
    char* ans = nullptr;
    unsigned int ansSize = 0;
    try {
      conn->Read(&ans, &ansSize);
    } catch (exception &e) {
      ss << " Error receiving Pull-init_ANS " << endl;
      LOG(ss.str().c_str());
      conn->Close();
      if (req != nullptr) delete[] req;
      return;
    }
    conn->Close();


    //Traitement de la réponse
    std::vector<std::string> request;
    splitString(request, ans, ansSize, ';');

    if (ans != nullptr) delete[] ans;
    if (req != nullptr) delete[] req;

    if(request[0].compare("sgx") == 0){
      res = mBrahms.Cyclon_Request(&req, &reqSize, &mData, mBrahms.SamplerSize());
      char* reqCyclon = nullptr;
      unsigned int reqCyclonSize = 0;
      //envoie cyclon pull request

      shared_ptr<TCPConnection> connCyclon;
      try {
        connCyclon = TCPConnection::Dial(qIP, destPort);
      } catch (exception& e) {
        ss << "Error dialing " << qIP << ": " << e.what();
        LOG(ss.str().c_str());
        if (req != nullptr) delete[] req;
        return;
      }


      try {
        connCyclon->Write(reqCyclon, reqCyclonSize);
      } catch (exception& e) {
        ss << "Error sending Pull-cyclon_REQ " << endl;
        LOG(ss.str().c_str());
        if (reqCyclon != nullptr) delete[] reqCyclon;
        return;
      }

      ss << "Sent Pull_init " << endl;
      ss.clear();
      ss.str("");

      //reception pull reply

      char* ansCyclon = nullptr;
      unsigned int ansCyclonSize = 0;
      try {
        connCyclon->Read(&ansCyclon, &ansCyclonSize);
      } catch (exception &e) {
        ss << " Error receiving Pull-cyclon_ANS " << endl;
        LOG(ss.str().c_str());
        connCyclon->Close();
        if (reqCyclon != nullptr) delete[] reqCyclon;
        return;
      }
      connCyclon->Close();

      std::vector<std::string> requestcyclon;
      splitString(requestcyclon, ansCyclon, ansCyclonSize, ';');
      res = mBrahms.Cyclon_Pull_Receive_Reply(requestcyclon[2], &mData);
      if (res != 0) {
        ss << "Error handling Pull-cyclon_ANS " << endl;
        LOG(ss.str().c_str());
        if (ansCyclon != nullptr) delete[] ansCyclon;
        if (req != nullptr) delete[] req;
        return;
      }
      if (ansCyclon != nullptr) delete[] ansCyclon;
      if (reqCyclon != nullptr) delete[] reqCyclon;
      return;
    }

    else {
      char* reqBrahms = nullptr;
      unsigned int reqBrahmsSize = 0;
      res = mBrahms.Pull_Request(&reqBrahms, &reqBrahmsSize, &mData);


      //envoie cyclon pull request

      shared_ptr<TCPConnection> connBrahms;
      try {
        connBrahms = TCPConnection::Dial(qIP, destPort);
      } catch (exception& e) {
        ss << "Error dialing " << qIP << ": " << e.what();
        LOG(ss.str().c_str());
        if (req != nullptr) delete[] req;
        return;
      }

      try {
        connBrahms->Write(reqBrahms, reqBrahmsSize);
      } catch (exception& e) {
        ss << "Error sending Pull-brahms_REQ " << endl;
        LOG(ss.str().c_str());
        if (reqBrahms != nullptr) delete[] reqBrahms;
        return;
      }


      //reception pull reply
      char* ansBrahms = nullptr;
      unsigned int ansBrahmsSize = 0;
      try {
        connBrahms->Read(&ansBrahms, &ansBrahmsSize);
      } catch (exception &e) {
        ss << " Error receiving Pull-brahms_ANS " << endl;
        LOG(ss.str().c_str());
        connBrahms->Close();
        if (reqBrahms != nullptr) delete[] reqBrahms;
        return;
      }
      connBrahms->Close();

      std::vector<std::string> requestbrahms;
      splitString(requestbrahms, ansBrahms, ansBrahmsSize, ';');

      res = mBrahms.Pull_Receive_Reply(requestbrahms[2], &mData);
      if (ansBrahms != nullptr) delete[] ansBrahms;
      if (reqBrahms != nullptr) delete[] reqBrahms;
      return;
      }
      return;

}

void RPS::samplerRequest(int i)
{
  mData.SamplerResize();
  auto streamview = mData.StreamView();
  int samplesize = mData.SamplerView().size();

  if( i >= samplesize){
    mData.SamplerAdd(mData.RandomElement(streamview));
  }
  else{
    stringstream ss;

    char* req = nullptr;
    unsigned int reqSize = 0;
    char* destIP = nullptr;
    unsigned int destIPSize = 0;
    int destPort = 0;
    int res = 0;
    int test = 0;
    //Mise en place de la push request
    res = mBrahms.Ping(&req, &reqSize, &destIP, &destIPSize, &destPort, &mData);
    if (res != 0) {
      ss << "Error creating Ping_REQ " << endl;
      LOG(ss.str().c_str());
      return;
    }

    auto qIP = string(destIP, destIPSize);
    if (destIP != nullptr) delete[] destIP;

    shared_ptr<TCPConnection> conn;
    try {
      conn = TCPConnection::Dial(qIP, destPort);
    } catch (exception& e) {
      ss << "Error dialing " << qIP << ": " << e.what();
      LOG(ss.str().c_str());
      if (req != nullptr) delete[] req;
      test = 1;
    }

    try {
      conn->Write(req, reqSize);
    } catch (exception& e) {
      ss << "Error sending Pull_REQ " << endl;
      LOG(ss.str().c_str());
      if (req != nullptr) delete[] req;
      return;
    }

    //Receive RPS Answer
      char* ans = nullptr;
      unsigned int ansSize = 0;
      try {
        conn->Read(&ans, &ansSize);
      } catch (exception &e) {
        ss << " Error receiving Pull_ANS " << endl;
        LOG(ss.str().c_str());
        conn->Close();
        if (req != nullptr){
          delete[] req;
          test = 1;
        }
      }
      conn->Close();

    if(test == 1){
      mData.SamplerRemove(i);
      mData.SamplerAdd(mData.RandomElement(mData.StreamView()));
    }
  }

}

void RPS::ReceiveRequest(shared_ptr<TCPConnection> conn)
{
  stringstream ss;

  char* req = nullptr;
  unsigned int reqSize = 0;
  char* ans = nullptr;
  unsigned int ansSize = 0;
  int res = 0;

  int type = 4;
  std::vector<std::string> request;


  //Read RPS Request
  try {
    conn->Read(&req, &reqSize);
  } catch (exception &e) {
    conn->Close();
    return;
  }
  //Determine if it's a Push Request, a Pull request, or a ping
  splitString(request, req, reqSize, ';');

  if(request[1].compare("PushRequest") == 0){
    type = 0;
  }
  if(request[1].compare("PullRequest") == 0){
    type = 1;
  }
  if(request[1].compare("Ping") == 0){
    type = 2;
  }
  if(request[1].compare("CyclonRequest") == 0){
    type = 3;
  }

  auto ip = request[2];
  switch (type) {
  //Push Reply
    case 0:

      mBrahms.Push_Receive_Request(ip, &mData);
      conn->Close();
      break;

  //Pull Reply
    case 1:
      if((mByzAttack == 0) && (mByzantin == 0)){
        res = mBrahms.Pull_Reply_Byz(&ans, &ansSize, &mData);
      }
      else{
        res = mBrahms.Pull_Reply(&ans, &ansSize, &mData);
      }
      if (req != nullptr) delete[] req;

      if (res != 0) {
        conn->Close();
        return;
      }

      //Answer to RPS Request
      try {
        // Send it the Message
        conn->Write(ans, ansSize);
      } catch (exception &e) {
        conn->Close();
        if (ans != nullptr) delete[] ans;
        return;
      }
      conn->Close();
      if (ans != nullptr) delete[] ans;
      break;


    //Ping Reply
    case 2:
      res = mBrahms.Pong(&ans, &ansSize, &mData);

      if (res != 0) {
        conn->Close();
        return;
      }

      //Answer to RPS Request
      try {
        // Send it the Message
        conn->Write(ans, ansSize);
      } catch (exception &e) {
        conn->Close();
        if (ans != nullptr) delete[] ans;
        return;
      }
      conn->Close();
      if (ans != nullptr) delete[] ans;
      break;

    //Cyclon Reply
    case 3:
      res = mBrahms.Cyclon_Reply(&ans, &ansSize, &mData, mBrahms.SamplerSize());
      if (req != nullptr) delete[] req;

      if (res != 0) {
        conn->Close();
        return;
      }

      //Answer to RPS Request
      try {
        // Send it the Message
        conn->Write(ans, ansSize);
      } catch (exception &e) {
        conn->Close();
        if (ans != nullptr) delete[] ans;
        return;
      }
      conn->Close();
      if (ans != nullptr) delete[] ans;
      break;

    default:
      break;
  }


}

void RPS::endRequest()
{
  stringstream ss;

  char* req = nullptr;
  unsigned int reqSize = 0;
  unsigned int destIPSize = 0;
  int destPort = 0;
  int res = 0;
  char* destIP = nullptr;
  string temp = "127.0.0.1:end:127.0.0.1";
  std::vector<std::string> IpPort;
  splitString(IpPort, mData.MyIP(), ':');

  destIPSize = IpPort[0].size();
  destPort = stoi(IpPort[1]);
  IpPort[0].copy(destIP, destIPSize);
  temp.copy(req, destIPSize);
  if (res != 0) {
    ss << "Error creating end_REQ " << endl;
    LOG(ss.str().c_str());
    return;
  }

  auto qIP = string(destIP, destIPSize);
  if (destIP != nullptr) delete[] destIP;

  shared_ptr<TCPConnection> conn;
  try {
    conn = TCPConnection::Dial(qIP, destPort);
  } catch (exception& e) {
    ss << "Error dialing " << qIP << ": " << e.what();
    LOG(ss.str().c_str());
    if (req != nullptr) delete[] req;
    return;
  }

  try {
    conn->Write(req, reqSize);
  } catch (exception& e) {
    ss << "Error sending end_REQ " << endl;
    LOG(ss.str().c_str());
    if (req != nullptr) delete[] req;
    return;
  }

  ss << "Sent end_REQ " << endl;
  ss.clear();
  ss.str("");

}

void RPS::setTerminated(int k){
  mterminated = k;
}
