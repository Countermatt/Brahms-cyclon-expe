#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <sys/wait.h>
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <thread>
#include <chrono>
#include <csignal>
#include <math.h>
#include <cstring>


#include "utils/string.hpp"
using namespace std;

#define RED   "\x1B[31m"
#define GRN   "\x1B[32m"
#define RESET "\x1B[0m"

extern "C" pid_t spawnChild(const char* program,  char* const* arg_list)
{
    pid_t ch_pid = fork();
    if (ch_pid == -1) {
        perror("fork");
        exit(EXIT_FAILURE);
    }

    if (ch_pid > 0) {
        printf("spawn child with pid - %d\n", ch_pid);
        return ch_pid;
    } else {
        execvp(program, arg_list);
        perror("execve");
        exit(EXIT_FAILURE);
    }
}

int main(int argc, char** argv) {

  std::vector<std::string> params;
    for (int i = 1; i < argc; i++)
    {
        params.push_back(argv[i]);

    }

    int k = 0;
//Récupération conf de l'expérience----------------------------------------------------

    std::vector<std::string> expe_param;
    //Ouverture et prise en compte des paramètres
    cout << RED << "Lecture du fichier de configuration de l'expérience" << "\n" << RESET << endl;
    fstream file;
    string tp;
    string temp;
    file.open("expe_param/param.csv", ios::in);
    if (!file) {
    		cout << "No such file";
        return EXIT_SUCCESS;
    }
    else {
          while(getline(file,tp)){
    			   temp.append(tp);
    			if (file.eof())
    				break;

    		  }


    	}
    file.close();
    splitString(expe_param, temp, ';');
    std::vector<std::string> temp_param;

    //nombre de noeuds
    splitString(temp_param, expe_param[0], ':');
    int nbNode = std::stoi(temp_param[1]);

    //liste des machines
    splitString(temp_param, expe_param[1], ':');
    std::vector<std::string> hostList;
    splitString(hostList, temp_param[1], ',');

    //nombre de noeud sgx
    temp_param.clear();
    splitString(temp_param, expe_param[2], ':');
    int pSgxNode = std::stoi(temp_param[1]);
    int nbSgxNode = floor((float)(nbNode*pSgxNode/100));

    //nombre de noeud byzantins
    temp_param.clear();
    splitString(temp_param, expe_param[3], ':');
    int pByzNode = std::stoi(temp_param[1]);
    int nbByzNode = floor((float)(nbNode*pByzNode/100));

    //période RPS
    temp_param.clear();
    splitString(temp_param, expe_param[4], ':');
    int rpsPeriod = std::stoi(temp_param[1]);

    //nombre de tour
    temp_param.clear();
    splitString(temp_param, expe_param[5], ':');
    int nbTurn = std::stoi(temp_param[1]);

    //taille de la vue
    temp_param.clear();
    splitString(temp_param, expe_param[6], ':');
    int viewSize = std::stoi(temp_param[1]);

    //Alpha Brahms
    temp_param.clear();
    splitString(temp_param, expe_param[7], ':');
    float alpha = std::stof(temp_param[1]);

    //Bravo Brahms
    temp_param.clear();
    splitString(temp_param, expe_param[8], ':');
    float beta = std::stof(temp_param[1]);

    //Gamma Brahms
    temp_param.clear();
    splitString(temp_param, expe_param[9], ':');
    float gamma = std::stof(temp_param[1]);

    //taille cyclon
    temp_param.clear();
    splitString(temp_param, expe_param[10], ':');
    int cyclonSize = std::stoi(temp_param[1]);

    //taille sampler
    temp_param.clear();
    splitString(temp_param, expe_param[11], ':');
    int samplerSize = std::stoi(temp_param[1]);


    pid_t childtab[nbNode];
//Préparation conf des machines distantes---------------------------------------------------------
  //création des dossiers de conf dess
  cout << RED << "Créationdu dossier pour la configuration des nodes" << "\n" << RESET << endl;
   char * const args[] = { "mkdir", "NodeConf", NULL };

  pid_t child;
  int wstatus;

  child = spawnChild("mkdir", args);

  if (waitpid(child, &wstatus, WUNTRACED | WCONTINUED) == -1) {
      perror("waitpid");
      exit(EXIT_FAILURE);
  }

  std::string nodeFolder;

  for(int i = 0; i< nbNode; i++){
    nodeFolder.clear();
    nodeFolder.append("NodeConf/");
    nodeFolder.append(to_string(i));
     char * temp = const_cast<char*> ( nodeFolder.c_str() );
     char * const args[] = { "mkdir", temp, NULL };

    pid_t child;
    int wstatus;

    child = spawnChild("mkdir", args);

    if (waitpid(child, &wstatus, WUNTRACED | WCONTINUED) == -1) {
        perror("waitpid");
        exit(EXIT_FAILURE);
    }
  }

  for(int i = 0; i< nbNode; i++){
    nodeFolder.clear();
    nodeFolder.append("NodeConf/");
    nodeFolder.append(to_string(i));
    nodeFolder.append("/param");
    char * temp = const_cast<char*> ( nodeFolder.c_str() );
    char * const args[] = { "mkdir", temp, NULL };

    pid_t child;
    int wstatus;

    child = spawnChild("mkdir", args);

    if (waitpid(child, &wstatus, WUNTRACED | WCONTINUED) == -1) {
        perror("waitpid");
        exit(EXIT_FAILURE);
    }
  }

  for(int i = 0; i< nbNode; i++){
    nodeFolder.clear();
    nodeFolder.append("NodeConf/");
    nodeFolder.append(to_string(i));
    nodeFolder.append("/data");
    char * temp = const_cast<char*> ( nodeFolder.c_str() );
    char * const args[] = { "mkdir", temp, NULL };

    pid_t child;
    int wstatus;

    child = spawnChild("mkdir", args);

    if (waitpid(child, &wstatus, WUNTRACED | WCONTINUED) == -1) {
        perror("waitpid");
        exit(EXIT_FAILURE);
    }
  }
  //création liste couple ip port noeuds

  int i = 0;
  int h = 0;
  int nbHost = hostList.size();
  int port = 10300;
  std::vector<std::string> nodeList;
  std::string nodeIp;

  while(i < nbNode){
    nodeIp.clear();
    if(h == nbHost){
      h = 0;
      port++;
    }
    nodeIp.append(hostList[h]);
    nodeIp.append(":");
    nodeIp.append(to_string(port));
    nodeList.push_back(nodeIp);
    i++;
    h++;
  }
if(params[0].compare("preparation") == 0){
  //création des fichiers de config
  cout << RED << "Création des fichiers de configuration des nodes" << "\n" << RESET << endl;
  std::string nodeFile;
  int sgxNodeTemp = 0;
  int byzantinNodeTemp = 0;
  std::vector<std::string> ipPort_Temp;
  std::string tempConf;

  std::vector<std::string> sgxNodeList;
  std::vector<std::string> byzNodeList;

  for(int k = 0; k < nodeList.size(); k++){
    tempConf.clear();
    nodeFile.clear();
    nodeFile.append("NodeConf/");
    nodeFile.append(std::to_string(k));
    nodeFile.append("/param");
    nodeFile.append("/param.csv");

	  file.open(nodeFile, ios::out);
	  if (!file) {
		  cout << "File not created!";
      exit(EXIT_FAILURE);
	  }
	  else {
      tempConf.append(nodeList[k]);
      tempConf.append(";");
      if(sgxNodeTemp < nbSgxNode){
        tempConf.append("0");
        sgxNodeList.push_back(nodeList[k]);
      }
      else{
        tempConf.append("1");
      }
      tempConf.append(";");
      string next;
      h = 1;
      int randomElement = 0;
      int randomElementTemp;
      for(int i = 0; i < viewSize; i++){
        while(h == 1 ){
          randomElementTemp = rand() % nodeList.size();
          next.clear();
          if((next.compare(nodeList[k]) != 0) && (randomElementTemp != randomElement)){
            next.append(nodeList[randomElementTemp]);
            tempConf.append(next);
            h = 0;
            randomElement = randomElementTemp;
          }
        }
        if(i != viewSize - 1){
          tempConf.append(",");
        }
        h = 1;
      }

    tempConf.append(";");
    tempConf.append(to_string(rpsPeriod));
    tempConf.append(";");
    tempConf.append(to_string(nbTurn));
    tempConf.append(";");
    tempConf.append(to_string(viewSize));
    tempConf.append(";");
    tempConf.append(to_string(alpha));
    tempConf.append(";");
    tempConf.append(to_string(beta));
    tempConf.append(";");
    tempConf.append(to_string(gamma));
    tempConf.append(";");
    if((sgxNodeTemp == nbSgxNode) && (byzantinNodeTemp < nbByzNode)){
      tempConf.append("0");
      byzNodeList.push_back(nodeList[k]);
    }
    else{
      tempConf.append("1");
    }
    tempConf.append(";");

    file << tempConf;
    }
    file.close();
  }

//copy des executables dans les dossiers de conf
cout << RED << "copy des executables dans les dossiers de conf" << "\n" << RESET << endl;
for(int i = 0; i< nbNode; i++){
  nodeFolder.clear();
  nodeFolder.append("NodeConf/");

  nodeFolder.append(to_string(i));
  nodeFolder.append("/");
   char * temp = const_cast<char*> ( nodeFolder.c_str() );
   char * const args[] = { "cp", "-r", "expe_param/brahms/lib", temp, NULL };

  pid_t child;
  int wstatus;

  child = spawnChild("cp", args);

  if (waitpid(child, &wstatus, WUNTRACED | WCONTINUED) == -1) {
      perror("waitpid");
      exit(EXIT_FAILURE);
  }
}

for(int i = 0; i< nbNode; i++){
  nodeFolder.clear();
  nodeFolder.append("NodeConf/");

  nodeFolder.append(to_string(i));
  nodeFolder.append("/");
   char * temp = const_cast<char*> ( nodeFolder.c_str() );
   char * const args[] = { "cp", "-r", "expe_param/brahms/src", temp, NULL };

  pid_t child;
  int wstatus;

  child = spawnChild("cp", args);

  if (waitpid(child, &wstatus, WUNTRACED | WCONTINUED) == -1) {
      perror("waitpid");
      exit(EXIT_FAILURE);
  }
}

for(int i = 0; i< nbNode; i++){
  nodeFolder.clear();
  nodeFolder.append("NodeConf/");

  nodeFolder.append(to_string(i));
  nodeFolder.append("/");
   char * temp = const_cast<char*> ( nodeFolder.c_str() );
   char * const args[] = { "cp", "expe_param/brahms/Makefile", temp, NULL };

  pid_t child;
  int wstatus;

  child = spawnChild("cp", args);

  if (waitpid(child, &wstatus, WUNTRACED | WCONTINUED) == -1) {
      perror("waitpid");
      exit(EXIT_FAILURE);
  }
}
//Préparation des machines distantes---------------------------------------------------------------
    //Envoie des dossiers aux machines à distance
    cout << RED << "Envoie des dossiers aux machines à distance" << "\n" << RESET << endl;
    std::vector<std::string> conn_info_temp;
    string conn_info;
    char * temp_conn = nullptr;
    for(int i = 0; i< nbNode; i++){
      conn_info_temp.clear();
      conn_info.clear();
      splitString(conn_info_temp, nodeList[i], ':');
      conn_info.append("ubuntu@");
      conn_info.append(conn_info_temp[0]);
      conn_info.append(":~");
      temp_conn = const_cast<char*> ( conn_info.c_str() );

      nodeFolder.clear();
      nodeFolder.append("NodeConf/");
      nodeFolder.append(to_string(i));
      nodeFolder.append("/");
      char * temp = const_cast<char*> ( nodeFolder.c_str() );

      char * const args[] = { "scp", "-i", "expe_param/key/id_rsa", "-r", temp, temp_conn, NULL };

      int wstatus;

      child = spawnChild("scp", args);

      if (waitpid(child, &wstatus, WUNTRACED | WCONTINUED) == -1) {
          perror("waitpid");
          exit(EXIT_FAILURE);
      }
    }

    //compilation du fichier
    cout << RED << "compilation de Brahms sur les machines distantes" << "\n" << RESET << endl;
    string path_temp;

    for(int i = 0; i< nbNode; i++){
      conn_info_temp.clear();
      splitString(conn_info_temp, nodeList[i], ':');
      conn_info.clear();
      conn_info.append(conn_info_temp[0]);
      temp_conn = const_cast<char*> ( conn_info.c_str() );

      nodeFolder.clear();
      nodeFolder.append("./brahms/make");
      nodeFolder.append(to_string(i));
      nodeFolder.append("/brahms/make");
      char * temp = const_cast<char*> ( nodeFolder.c_str() );

      path_temp.clear();
      path_temp = "/home/ubuntu/";
      path_temp.append(to_string(i));
      char * path = const_cast<char*> ( path_temp.c_str() );
      char * const args[] = { "ssh", temp_conn, "-i", "expe_param/key/id_rsa", "-l", "ubuntu", "make", "-C", path, NULL };


      int wstatus;

      child = spawnChild("ssh", args);

      if (waitpid(child, &wstatus, WUNTRACED | WCONTINUED) == -1) {
          perror("waitpid");
          exit(EXIT_FAILURE);
      }
    }
}
else if(params[0].compare("lancement") == 0){
    //lancement de l'expérience
    pid_t childtab1[nbNode];
    std::vector<std::string> conn_info_temp;
    string conn_info;
    char * temp_conn = nullptr;
    cout << RED << "Lancement de l'expérience" << "\n" << RESET << endl;
    for(int i = 0; i< nbNode; i++){
      conn_info_temp.clear();
      splitString(conn_info_temp, nodeList[i], ':');
      conn_info.clear();
      conn_info.append(conn_info_temp[0]);
      temp_conn = const_cast<char*> ( conn_info.c_str() );

      nodeFolder.clear();
      nodeFolder.append("./");
      nodeFolder.append(to_string(i));
      nodeFolder.append("/build/brahms");
      char * temp = const_cast<char*> ( nodeFolder.c_str() );

      string path_temp = "/home/ubuntu/";
      path_temp.append(to_string(i));
      path_temp.append("/build/brahms");
      char * path = const_cast<char*> ( path_temp.c_str() );

      string param_temp = to_string(i);
      char * param= const_cast<char*> ( param_temp.c_str() );

      char * const args[] = { "ssh", temp_conn, "-i", "expe_param/key/id_rsa", "-l", "ubuntu", temp, param, NULL };


      int wstatus;

      childtab1[i] = spawnChild("ssh", args);


    }

    k = 0;


    std::this_thread::sleep_for(std::chrono::milliseconds(1000)*60);

    while(k<nbNode){
      cout << GRN << k << "\n" << RESET << endl;
      /*
      if (waitpid(childtab[k], &wstatus, WUNTRACED | WCONTINUED) == -1) {
          perror("waitpid");
          exit(EXIT_FAILURE);
      }
      */
      wait(&wstatus);
      k++;
    }

    //récupération des données de l'expérience
    cout << RED << "Récupération des données de l'expérience" << "\n" << RESET << endl;

  char * const args3[] = { "mkdir", "Result", NULL };

   child = spawnChild("mkdir", args3);

   if (waitpid(child, &wstatus, WUNTRACED | WCONTINUED) == -1) {
       perror("waitpid");
       exit(EXIT_FAILURE);
   }

/*
   std::string tempResult;
   for(int i = 0; i <  sgxNodeList.size(); i++){
     tempResult.append(sgxNodeList[i]);
     tempResult.append(";");
   }
   tempResult.append("\n");

  for(int i = 0; i <  byzNodeList.size(); i++){
    tempResult.append(byzNodeList[i]);
    if(i < byzNodeList.size()-1){
      tempResult.append(";");
    }
  }

  file.open("Result/NodeList.csv", ios::out);
  if (!file) {
    cout << "File not created!";
    exit(EXIT_FAILURE);
  }
  else {
    file << tempResult;
    file.close();
  }
*/
    temp_conn = nullptr;
    for(int i = 0; i< nbNode; i++){
      conn_info.clear();
      conn_info_temp.clear();
      splitString(conn_info_temp, nodeList[i], ':');
      conn_info.append("ubuntu@");
      conn_info.append(conn_info_temp[0]);
      conn_info.append(":");
      conn_info.append(to_string(i));
      temp_conn = const_cast<char*> ( conn_info.c_str() );

      nodeFolder.clear();
      nodeFolder.append("Result");
      char * temp = const_cast<char*> ( nodeFolder.c_str() );

      char * const args[] = { "scp", "-i", "expe_param/key/id_rsa", "-r", temp_conn, temp, NULL };

      pid_t child;
      int wstatus;

      child = spawnChild("scp", args);

      if (waitpid(child, &wstatus, WUNTRACED | WCONTINUED) == -1) {
          perror("waitpid");
          exit(EXIT_FAILURE);
      }
    }
}
else if(params[0].compare("recuperation") == 0){
  std::vector<std::string> conn_info_temp;
  string conn_info;
  char * temp_conn = nullptr;
    //supression des fichiers sur les machines distantes
    cout << RED << "Suprésion des dossiers sur les machines distantes" << "\n" << RESET << endl;
    for(int i = 0; i< nbNode; i++){
      conn_info_temp.clear();
      splitString(conn_info_temp, nodeList[i], ':');
      conn_info.clear();
      conn_info.append(conn_info_temp[0]);
      temp_conn = const_cast<char*> ( conn_info.c_str() );

      string path_temp;

      path_temp.append(to_string(i));
      char * path = const_cast<char*> ( path_temp.c_str() );
      char * const args[] = { "ssh", temp_conn, "-i", "expe_param/key/id_rsa", "-l", "ubuntu", "rm", "-rf", path, NULL };


      pid_t child;
      int wstatus;

      child = spawnChild("ssh", args);

      if (waitpid(child, &wstatus, WUNTRACED | WCONTINUED) == -1) {
          perror("waitpid");
          exit(EXIT_FAILURE);
      }

    }

    //suppression fichier temporaire sur le control
    cout << RED << "Suprésion des dossiers d'initialisation sur la machine de control" << "\n" << RESET << endl;
    char * const args2[] = { "rm", "-rf", "NodeConf", NULL };


    child = spawnChild("rm", args2);

    if (waitpid(child, &wstatus, WUNTRACED | WCONTINUED) == -1) {
        perror("waitpid");
        exit(EXIT_FAILURE);
    }
}
    exit(EXIT_SUCCESS);
}
