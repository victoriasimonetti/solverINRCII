#ifndef _Solution_H
#define _Solution_H

#include <vector>
#include <stack>
#include <cstdio>
#include <cstring>
#include <cstdlib>
#include <algorithm> 
#include <iostream> 
#include "../include/Instance.h"
#include "../include/Fulfilled.h"
#include "../zen/ZenSolver.h"
using namespace std;


struct Change {
    int nurse;
    int day;
    int oldShift;
    int shift; //Atual
    int oldSkill;
    int skill; //Atual
};


class Solution {

private:

    Instance* inst;
    stack<Change> listChanges;
    bool flagUndo; //Indica se está desfazendo o movimento.
public:
    int costSolution;

    vector<vector<Fulfilled> > schedule; //Escala de enfermagem com: enfermeiro x dia. Ex: enf1 (0 Nurse) (0 headnurse) ... 
    void geraSolutionFile(); 
    void atualizaFileCustom(string cusInFile, string cusOutFile);
    void printFinalSolution(string cusOutFile);
    void print();

	void calcIncremental(int shiftAtual, int nurseMod, int shiftMod, int dayMod);
    void calcIncremental(int shiftAtual, int nurseMod, int shiftMod, int dayMod, int skill, int skillMod);
    void incr_h3_shiftTypeSucessions(int shiftAtual, int nurseMod, int shiftMod, int dayMod);
    void incr_s1_h2(int shiftAtual, int shiftMod, int dayMod, int skill, int skillMod);
    void incr_s3_consecutiveDaysOff(int shiftAtual, int nurseMod, int shiftMod, int dayMod);
    void incr_s4_preferences(int shiftAtual, int nurseMod, int shiftMod, int dayMod);
    void incr_s2a_consecutiveShift(int shiftAtual, int nurseMod, int shiftMod, int dayMod);
    void incr_s2b_consecutiveWorkingDays(int shiftAtual, int nurseMod, int shiftMod, int dayMod);
    void incr_s5_completeWeekend(int shiftAtual, int nurseMod, int shiftMod, int dayMod);
    void incr_s7_totalWorkingWeekends(int shiftAtual, int nurseMod, int shiftMod, int dayMod);
    void incr_s6_totalAssignments(int shiftAtual, int nurseMod, int shiftMod, int dayMod);

    Solution(Instance* inst);
    void forcaBruta();
    void h3_shiftTypeSucessions(int n);
    void s1_h2_optimalMinStaffing();
    void s3_consecutiveDaysOff(int n);
    void s4_preferences(int n);
    void s2a_consecutiveShift(int n);
    void s2b_consecutiveWorkingDays(int n);
    void s5_completeWeekend(int n);
    void s7_totalWorkingWeekends(int n);
    void s6_totalAssignments(int n);
   
    void undoLastMove();
    void clearChanges();
	void printSolution();

    //Restrições Hard
    int h_underStaffing; //Numero de enfermeiros para cada turnos de uma habilidade deve ser maior/igual que o minimo.
    int h_shiftTypeSuccessions; //o turno do enfermeiro e o proximo turno dele deve respeitar as sucessões.

    //Restrições Soft para uma semana
	int s_maxconsecutiveShift; 
    int s_maxconsecutiveWorkingDays; 
    int s_maxconsecutiveDaysOff;
	int s_minconsecutiveShift; 
    int s_minconsecutiveWorkingDays; 
    int s_minconsecutiveDaysOff;

    int s_optimalCoverage; //o numero de enfermeiros turnoxhabilidade deve ser igual/maior que o ótimo. Peso 30

    int s_consecutiveShift; //min e max numero de consecutivos dias de mesmo turno. Peso 15
    vector<int> s_consecutiveShift_nurse;

    int s_consecutiveWorkingDays; //min e max numero de consecutivos dias trabalhando. Peso 30
    vector<int> s_consecutiveWorkingDays_nurse;

    int s_consecutiveDaysOff; //min e max numero de dias consecutivos de folga. Peso 30
    vector<int> s_consecutiveDaysOff_nurse;
    
    int s_preferences; //Cada atribuição de um turno indesejado é penalisado. Peso 10
    int s_completeWeekend; //Cada enfermeiro com a flag true dessa restrição???

    //Restrições Soft para o planejamento todo
    int s_totalAssignments; //Cada enfermeiros o numero min e max do total de dias trabalhados. Peso 20
    vector<int> s_totalAssignments_nurse;

    int s_totalWorkingWeekeends; //Cada enfermeiro o numero de fins de semana trabalhados deve ser menor/igual ao max. Peso 30
    vector<int> s_totalWorkedWeekend_nurse;                               

    //Delta para as violações do final da semana
    int s_delta_consecutiveShift;
    int s_delta_consecutiveWorkingDays;    
    int s_delta_consecutiveDaysOff; 
    vector<int> s_delta_consecutiveShift_nurse;
    vector<int> s_delta_consecutiveWorkingDays_nurse;    
    vector<int> s_delta_consecutiveDaysOff_nurse;    

    vector< vector < vector<int>>> s1_h1_incremental; 

	void verifySolution();

	bool isFeasible();

    void readSolverSolution(double* sol, double obj, ZenVar& x);

	void validator();
 
};

#endif





