#ifndef _Instance_H
#define _Instance_H

#include <vector>
#include <cstdio>
#include <cstring>
#include <cstdlib>
#include <algorithm> 
#include <iostream> 
#include <sstream>
#include <fstream>  
#include <tr1/unordered_map>
#include "Nurse.h"
#include <utility> 
using namespace std;

typedef tr1::unordered_map<string, int> mapStringInt;

class Instance {

private:

public:

    mapStringInt skillIndex;
    mapStringInt shiftIndex;
    mapStringInt nurseIndex;
    mapStringInt contractIndex;
    mapStringInt dayIndex;

    string scenarioFile;
    string historyFile;
    string weekFile;

    //Informações gerais do cenário - Scenario
    int n_weeks; //Número de semanas
    int n_skills; //Número de habilidades
    int n_shifts; //Número de tipos de turnos
    int n_nurses; //Numero de enfermeiros
    vector<int> minShiftConsecutive; //Lista com: turno e min turnos consecutivos NAO INCLUI FOLGA
    vector<int> maxShiftConsecutive; //Lista com: turno e max turnos consecutivos NAO INCLUI FOLGA
    vector <vector <int> > forbiddenShift; //matriz de turnos x turnos, 1 = proibido, 0 = não é proibido INCLUI FOLGA  
    int n_contracts; //Número de contratos
    vector<int> minDaysWorked; //Minimo de dias trabalhados na escala toda
    vector<int> maxDaysWorked; //Maximo de dias trabalhados na escala toda
    vector<int> minDaysConsecutiveWorked; //Minimo de dias consecutivos trabalhados
    vector<int> maxDaysConsecutiveWorked; //Maximo de dias consecutivos trabalhados
    vector<int> minDaysOff; //Minimo de dias de folga consecutivos
    vector<int> maxDaysOff; //Maximo de dias consecutivo e folga
    vector<int> maxWorkingWeekends; //Maximo numero de fins de semana trabalhando
    vector<int> completeWeekendConstraint; //1=presença 0=abstenção, da restrição de complete weekend    
    vector<Nurse> nurses;
    string infoFile; //n005w4
	string name;

    //listas simples com os nomes dos enfermeiros, habilidades, dias e turnos.
    vector<string> listNameNurse;
    vector<string> listNameSkill;
    vector<string> listNameDay;
    vector<string> listNameShift;


    //Informaçõe da semana - Week
    vector<vector<vector<int> > > minRequerementsWeek; //Cobertura minima por: turno x habilidade x dia da semana (segunda a domingo) NAO INCLUI FOLGA
    vector<vector<vector<int> > > optRequerementsWeek; //Cobertura ótima por: turno x habilidade x dia da semana (segunda a domingo)  NAO INCLUI FOLGA  
    vector <vector < vector<int> > > preferences; //matriz de enfermeiros x dias, caso tenha preferencia em um dia, o numero do turno é colocado, cc -1.  
    int atualWeek; 
   
    //Informações do histórico para CADA ENFERMEIRO - history
    vector<int> historyTotalDaysWorked; // Numero total de dias trabalhados
    vector<int> historyTotalWorkedWeekends; //numero total de fins de semana trabalhando
    vector<int> historyLastShiftWorked; //Qual foi o ultimo turno trabalhado
    vector<int> historyLastShiftConsecutive; //Quantos dias consecutivos que trabalhou no turno do ultimo turno.
    vector<int> historyTotalConsecutiveDays; //Total de dias consecutivos trabalhados
    vector<int> historyTotalConsecutiveDaysOff; //Total de dias consecutivos de folga.
   
	void dataSolver(string datafile);
	Instance(string instancia);
	void readFiles();	
};

#endif
