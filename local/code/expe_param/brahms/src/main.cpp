#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <thread>
#include <chrono>
#include <csignal>
#include <cxxopts.hpp>
#include <math.h>
#include <unistd.h>

#include "data/Data.hpp"
#include "utils/string.hpp"
#include "brahms/Brahms.hpp"
#include "RPS.hpp"

using namespace std;

int main(int argc, char** argv) {
  std::vector<std::string> params;
    for (int i = 1; i < argc; i++)
    {
        params.push_back(argv[i]);

    }

  char str[1024];
  char *path = NULL;
  path = getcwd(str,1024);
  //creation du chemin vers le dossier data
  std::string path_data_string(path);
  path_data_string.append(params[0]);
  path_data_string.append("/data/data.csv");

  //Ouverture et prise en compte des paramètres
  fstream file;
  std::string temp;
  std::vector<std::string> param;


  //creation du chemin vers le fichier de conf
  std::string path_string(path);
  path_string.append("/");
  path_string.append(params[0]);
  path_string.append("/param/param.csv");
  file.open(path_string, ios::in);
  if (!file) {
  		cout << "No such file";
      return EXIT_SUCCESS;
  }
  else {

  		while (1) {
  			file >> temp;
  			if (file.eof())
  				break;

  		}

      splitString(param, temp, ';');
  	}
  file.close();



  std::vector<std::string> ipPort_Temp;
  splitString(ipPort_Temp, param[0], ':');
  std::string myIP = ipPort_Temp[0];
  int rpsPort = std::stoi(ipPort_Temp[1]);
  int sgx = std::stoi(param[1]);
  std::string bsIPs = param[2];
  int rps_period = std::stoi(param[3]);
  int nb_turn = std::stoi(param[4]);
  int view_lenght = std::stoi(param[5]);
  float alpha = std::stof(param[6]);
  float beta = std::stof(param[7]);
  float gamma = std::stof(param[8]);
  int byzantin = std::stoi(param[9]);

  if(alpha + beta + gamma > 1){
    stringstream ss;
    ss << "alpha + beta + gamma > 1" << endl;
    return EXIT_SUCCESS;
  }


  int Al = (int)floor(view_lenght*(float)alpha);
  int Bl = (int)floor(view_lenght*(float)beta);
  int Gl = view_lenght - Al - Bl;



  auto rps = std::make_shared<RPS>(rpsPort, bsIPs, nb_turn, rps_period, sgx, myIP, Al, Bl, Gl, path_data_string, byzantin, gamma);
  while (true)
      std::this_thread::sleep_for(std::chrono::milliseconds(10));

    return EXIT_SUCCESS;
}
