#ifndef BRAHMS_HPP_
#define BRAHMS_HPP_

#include <string>
#include <vector>

#include "utils/string.hpp"
#include "data/Data.hpp"

class Brahms {

  public:

    Brahms();

//Getter
    int AL1() { return mAL1; }
    int BL2() { return mBL2; }
    int GL3() { return mGL3; }
    int SamplerSize() { return mSamplerSize; }

//Setter
  void SetAL1(int al1);
  void SetBL2(int bl2);
  void SetGL3(int gl3);
  void SetSamplerSize(int samplerSize);

//Push Function
    int Push_Request(char** req,
                     unsigned int* reqSize,
                     char** destIP,
                     unsigned int* destIPSize,
                     int* destPort,
                     Data* data);

    int Push_Receive_Request(std::string ans, Data* data);


//Pull Function
    int Pull_Request(char** req,
                    unsigned int* reqSize,
                    Data* data);

    int Pull_Reply(char** ans,
                   unsigned int* ansSize,
                   Data* data);

    int Pull_Reply_Byz(char** ans,
                   unsigned int* ansSize,
                   Data* data);


    int Pull_Receive_Reply(std::string ans, Data* data);

    int Pull_Init_Request(char** req,
                  unsigned int* reqSize,
                  char** destIP,
                  unsigned int* destIPSize,
                  int* destPort,
                  Data* data);

    int Pull_Init_Reply(char** ans,
                        unsigned int* ansSize,
                        Data* data);

    int Pull_Init_Receive_Request(std::string ans);

    int Pull_Init_Receive_Reply(std::string ans);

// - Merge View
    void MergeView(int aL1, int bL2, int gL3, Data* data);

    int Ping(char** req,
             unsigned int* reqSize,
             char** destIP,
             unsigned int* destIPSize,
             int* destPort,
             Data* data);

    int Pong(char** ans,
             unsigned int* ansSize,
             Data* data);

    int Cyclon_Request(char** req,
                       unsigned int* reqSize,
                       Data* data,
                       int cyclonSize);

    int Cyclon_Reply(char** ans,
                     unsigned int* ansSize,
                     Data* data,
                     int cyclonSize);

    int Cyclon_Pull_Receive_Reply(std::string ans,
                                  Data* data);
  private:
    int mAL1, mBL2, mGL3;
    int mSamplerSize;
};

#endif // BRAHMS_HPP_
