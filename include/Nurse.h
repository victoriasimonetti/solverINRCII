#ifndef _Nurse_H
#define _Nurse_H

#include <string>
#include<vector>
using namespace std;

struct Nurse {

    string name;
    string contract;
    vector<int> skill_i; //Vetor de skill no formato: 0 n tem a skill, 1 tem a skill = 1 1 (nurse headnurse)
    vector<string> skill_s; //Lista de skills no formato de string: headnurse nurse

   
};

#endif
