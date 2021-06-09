#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string>
#include <vector>

#include "data/Data.hpp"
#include "utils/string.hpp"

using namespace std;


std::string Data::RandomElement(std::vector<std::string> View){

  int rand_val;

  srand(time(NULL));

  if (View.empty()){
    return NULL;
  }

  rand_val = rand() % View.size();

  return View[rand_val];

}


void Data::PullAdd(std::string element){
  mPullView.push_back(element);
}
void Data::PushAdd(std::string element){
  mPushView.push_back(element);
}
void Data::GlobalAdd(std::string element){
  mGlobalView.push_back(element);
}
void Data::SamplerAdd(std::string element){
  mSamplerView.push_back(element);
}
void Data::StreamAdd(std::string element){
  mStreamView.push_back(element);
}

void Data::SamplerRemove(int i){
  std::vector<std::string> temp;
  temp = SamplerView();
  mSamplerView.clear();
  mSamplerView.shrink_to_fit();

  for(int k = 0; k < (int) temp.size(); k++){
    if(k != i){
      mSamplerView.push_back(temp[k]);
    }
  }
}

void Data::PullReset(){
  mPullView.clear();
  mPullView.shrink_to_fit();
}
void Data::PushReset(){
  mPushView.clear();
  mPushView.shrink_to_fit();
}
void Data::GlobalReset(){
  mGlobalView.clear();
  mGlobalView.shrink_to_fit();
}
void Data::StreamReset(){
  mStreamView.clear();
  mStreamView.shrink_to_fit();
}

void Data::SamplerResize(){
  mSamplerView.shrink_to_fit();
}

void Data::SetIdentifiant(int sgx){
  if(sgx == 0){
    mIdentifiant = "sgx";
  }
  else{
    mIdentifiant = "nonsgx";
  }
}

void Data::SetMyIP(std::string myIP){
  mMyIP = myIP;
}

std::string Data::GlobalPrint(){
  auto view = GlobalView();
  string result;

  for(int i =0; i < (int) view.size() - 1; i++){
    result.append(view[i]);
    result.append(",");
  }
  result.append(view[(int) view.size() - 1]);
  result.append(";");
  return result;
}
