#ifndef RPS_HPP_
#define RPS_HPP_

#include <Define.hpp>

#define RPS_LISTENING_PORT 10337


#include <chrono>
#include <memory>
#include <vector>
#include <mutex>
#include <thread>
#include <iostream>

#include "net/TCPListener.hpp"
#include "net/TCPConnection.hpp"
#include "data/Data.hpp"
#include "brahms/Brahms.hpp"

using namespace std;

class RPS {

public:
  RPS(int rpsTcpPort, std::string bootstrapIP, int nb_turn,
    int rps_period, int sgx, std::string myIP, int al,
    int bl, int gl, std::string path, int byzantin,
    int samplersize);

  int isTerminated() { return mterminated; }
  void setTerminated(int k);

  void sendingThread();
  void listeningThread();

  //pull request
  void initPullRequest();

  //push request
  void pushRequest();


  void ReceiveRequest(shared_ptr<TCPConnection> conn);
  void samplerRequest(int i);
  void samplerReply();
  void endRequest();


private:



  std::shared_ptr<TCPListener> mListener;
  std::thread mListenerThread, mQueryListenerThread, mSenderThread;
  unsigned int mRpsTcpPort;
  unsigned int mPushReqCnt, mPullReqCnt, mPingReqCnt;
  Data mData;
  Brahms mBrahms;
  int mNbTurn;
  int mRPSPeriod;
  int mSGX;
  int mterminated;
  int mByzantin;
  int mByzAttack;
  std::string mDataPath;
};


#endif //BRAHMS_HPP_
