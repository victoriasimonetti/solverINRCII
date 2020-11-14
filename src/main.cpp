#include <cstdlib>
#include <iostream>
#include <stdio.h>
#include <time.h> 
#include <string.h>  
#include <random>  
#include <glpk.h>
#include <iomanip>
#include "opt.h"
#include "OsiCpxSolverInterface.hpp"
#include "../utils/Crono.h"
#include "../utils/Convert.h"
#include "../utils/Util.h"
#include "../utils/DummiePriorityQueue.h"
#include "../utils/Combination.h"
#include "../zen/ZenSolver.h"
#include "localBranch/LocalBranching.h"
#include "../_opt/opt.h"
#include "../include/Instance.h"
#include "../include/Solution.h"
#include "../include/Neighborhood.h"

using namespace std;

vector<string> listInst = {"n035w2-0-0-0",
"n035w4-2-8-8-7-5",/*1*/
"n035w4-0-1-7-1-8", 
"n035w4-0-4-2-1-6", 
"n035w4-0-5-9-5-6",
"n035w4-0-9-8-7-7",/*5*/
"n035w4-1-0-6-9-2",
"n035w4-2-8-6-7-1",
"n035w4-2-9-2-2-6",
"n035w4-2-9-7-2-2",
"n035w4-2-9-9-2-1",
"n070w4-0-3-6-5-1",/*11*/
"n070w4-0-4-9-6-7",
"n070w4-0-4-9-7-6",
"n070w4-0-8-6-0-8",
"n070w4-0-9-1-7-5",
"n070w4-1-1-3-8-8",
"n070w4-2-0-5-6-8",
"n070w4-2-3-5-8-2",
"n070w4-2-5-8-2-5",
"n070w4-2-9-5-6-5",
"n110w4-0-1-4-2-8",/*21*/
"n110w4-0-1-9-3-5",
"n110w4-1-0-1-6-4",
"n110w4-1-0-5-8-8",
"n110w4-1-2-9-2-0",
"n110w4-1-4-8-7-2",
"n110w4-2-0-2-7-0",
"n110w4-2-5-1-3-0",
"n110w4-2-8-9-9-2",
"n110w4-2-9-8-4-9",
"n035w8-0-6-2-9-8-7-7-9-8",/*31*/
"n035w8-1-0-8-1-6-1-7-2-0",
"n035w8-1-0-8-4-0-9-1-3-2",
"n035w8-1-1-4-4-9-3-5-3-2",
"n035w8-1-7-0-6-2-1-1-1-6",
"n035w8-2-2-1-7-1-8-7-4-2",/*36*/
"n035w8-2-7-1-4-9-2-2-6-7",
"n035w8-2-8-8-7-5-0-0-6-9",
"n035w8-2-9-5-6-3-9-9-2-1",
"n035w8-2-9-7-2-2-5-7-4-3",
"n070w8-0-3-3-9-2-3-7-5-2",/*41*/
"n070w8-0-9-3-0-7-2-1-1-0",
"n070w8-1-5-6-8-5-7-8-5-6",
"n070w8-1-9-8-9-9-2-8-1-4",
"n070w8-2-4-9-2-0-2-7-0-6",/*45*/
"n070w8-2-5-1-3-0-8-0-5-8",
"n070w8-2-5-7-4-8-7-2-9-9",
"n070w8-2-6-3-0-1-8-1-5-9",
"n070w8-2-8-6-0-1-6-4-7-8",
"n070w8-2-9-3-5-2-2-9-2-0",
"n110w8-0-2-1-1-7-2-6-4-7",/*51*/
"n110w8-0-3-2-4-9-4-1-3-7",
"n110w8-0-5-5-2-2-5-3-4-7",
"n110w8-0-7-8-7-5-9-7-8-1",
"n110w8-0-8-8-0-2-3-4-6-3",
"n110w8-0-8-8-2-2-3-2-0-8",
"n110w8-1-0-6-1-0-3-2-9-1",
"n110w8-1-4-1-3-6-8-8-1-3",
"n110w8-2-2-9-5-5-1-8-4-0",
"n110w8-2-8-5-7-3-9-8-8-5"};

vector<double> BKS ={0,1085, 1415, 1615, 1530, 1365, 1360, 1315, 1525, 1480, 1485, 2415, 2125, 2195, 2315, 2100, 2530, 2340, 2380, 2335, 2395, 2345, 2525, 2570, 2555, 2975, 2495, 2730, 2685, 2980, 2775, 2555, 2305, 0,0,0,0,0,0,0,0,4595,4760, 0,0,0,0,0,0,0,0,4010,3650,0,0,0,0,0,0,0,0};

char* optWorkDir = NULL; 
char* optScenarioFile = NULL;
char* optInitialHistoryFile = NULL;
char* optWeekDataFile = NULL;
char* optSolutionFileName = NULL;
char* optCustomInputFile = NULL;
char* optCustomOutputFile = NULL;
char* optInst = NULL;
int optLAFOlist = 100;
int optCplex = 0;
int optLAHClist = 10000;
int optRandomSeed = 0;
double optTimeOut = 1000000000;
double optTimeInitSol = 60;
time_t g_time1;
int optV1 = 19;
int optV2 = 21;
int optV3 = 19;
int optV4 = 23;
int optV5 = 16;
int optV6 = 0;
int optV7 = 0;

int optN1 = 5;
int optN2 = 4;
int optN3 = 3;
int optN4 = 2;
int optN5 = 1;

int optP = 1;
int optCirculaVizinho = 0;

string wd;

using namespace Convert;

int optVerbose = 0;
int optSTL = 5; //tempo limite do subproblema 
SolverType optMipBackend = Z_CPLEX;
int optAlg =1;
double optLB = 0;
int optNeighborhood = 0;
char* optSol = NULL;

char *optFilename = NULL;
char *optModel = strdup("modeloUV.mod"); //modeloUV.mod
char *optSolInitModel = NULL; 
string optDatafile = "saida.dat";
string optLPFile;

Instance *inst = NULL;
random_device rd;
mt19937 gen(rd());

double tempo_restante(){
	time_t time2;
	double dif_sec = 0;
	time (&time2);
	dif_sec = difftime (time2,g_time1);
	return dif_sec;
}

void construtivoAleatorio(Solution& s){

    for(int i=0; i<inst->n_nurses; i++){
        for(int j=0; j<7*inst->n_weeks; j++){
            std::uniform_int_distribution<> dis(0, inst->n_shifts);
            int turno = dis(gen);
            s.schedule[i][j].shift = turno;

            std::uniform_int_distribution<> dis1(0, inst->nurses[i].skill_s.size()-1);
            int skill =  dis1(gen);
            s.schedule[i][j].skill = inst->nurses[i].skill_s[skill];    
        }
    }
}

Solution buscaLocal(Solution s){

	Solution bestSolution(inst);
	bestSolution = s;
	Solution currentSolution(inst);
	currentSolution = s;
    Solution currentSolution2(inst);
	currentSolution2 = s;
	Neighborhood* nb = new Neighborhood(inst);
    
    int flag = 1;
    do{
        flag = 1;
        currentSolution = nb->trocaShifts(bestSolution);
        currentSolution2 = nb->trocaSkill(bestSolution);
        if(currentSolution2.costSolution < currentSolution.costSolution){
            currentSolution = currentSolution2; 
        }

        if(currentSolution.costSolution < bestSolution.costSolution){
            bestSolution = currentSolution;
            if(bestSolution.h_underStaffing + bestSolution.h_shiftTypeSuccessions == 0 ){
                cout << "BL: \e[32m " << bestSolution.costSolution  << "\e[39m" << endl;
            }else{
                cout << "BL: \e[31m " << bestSolution.costSolution  << "\e[39m" << endl;
            }
            flag = 0;
        } 

    }while(flag == 0 && tempo_restante() < optTimeOut);
			
	delete nb;
    currentSolution = bestSolution;
    currentSolution.forcaBruta();  
	return bestSolution;
}

int ultimaMelhora=0;
void print(Solution &best, Solution &current, int tempo, unsigned long long int it, int delta){

    const string colorRed = "\e[31m";
    const string colorGreen = "\e[32m";
    const string colorFim = "\e[39m";
	double gapBKS = ((double)best.costSolution-BKS[stoi(optInst)]) / min(BKS[stoi(optInst)], (double)best.costSolution)*100;

    if(delta < 0){
        string improved = "*";
        string colorImproved = colorRed;
		int inf = best.h_underStaffing + best.h_shiftTypeSuccessions;

        if(inf == 0 ){
            colorImproved = colorGreen; 
			ultimaMelhora = tempo;  
        }

        printf("%s  (%d)  best=%s%5d%s%s  cur=%5d   t=%3d   it=%llu    GAP:%.2f\%\n", inst->infoFile.c_str(), inf,
                colorImproved.c_str(),best.costSolution, improved.c_str(), colorFim.c_str(), current.costSolution, tempo, it, gapBKS);
		fflush(stdout);
    }  
}

Solution LAHC(Solution s, bool first=false){

    Solution bestSolution(inst);
	bestSolution = s;
	Solution currentSolution(inst);
	currentSolution = s;

	Solution aux(inst);
	aux = s;

	Neighborhood* nb = new Neighborhood(inst);
    int LAHClist = optLAHClist;

    vector<int> list;
    for(int i=0; i<LAHClist; i++){
        list.push_back(s.costSolution);
    }    

    unsigned long long int I = 0;
	int total=0;
	vector<int> probV;
	probV.push_back(optV1); total += optV1;
	probV.push_back(optV2); total += optV2;
	probV.push_back(optV3); total += optV3;
	probV.push_back(optV4); total += optV4;
	probV.push_back(optV5); total += optV5;
	/*probV.push_back(optV6); total += optV6;
	probV.push_back(optV7); total += optV7;*/
	

    std::uniform_int_distribution<> dis(1, total);
    while(tempo_restante() < optTimeInitSol){

        int previousCostSolution = currentSolution.costSolution;        
		//aux = currentSolution;
		currentSolution.clearChanges();

		int p=0;
        int r = dis(gen);
		for(int i=0; i<5; i++){
			p += probV[i];
			if(p >= r){
				switch(i){
					//case 0: nb->multiSwap(currentSolution); break; 
					//case 1: nb->change(currentSolution); break; 
					//case 2: nb->doubleChange(currentSolution); break; 
					case 0: nb->trocaShiftsRand(currentSolution); break;
					case 1: nb->swapRandBlock(currentSolution); break;
					case 2: nb->trocaSkillRand(currentSolution); break;
					case 3: nb->giveOffRand(currentSolution); break;
					//case 4: nb->weekendComplete(currentSolution); break;
					//case 5: nb->swapGiveOffgiveShiftRand(currentSolution); break;
					case 4: nb->swapTwoBlock(currentSolution); break;			
					default: cout << "eu não deveria estar aqui" << endl; exit(0);		
				}
				break;	
			}
		}

        int v = I % LAHClist;
        int delta = 0;
        if(currentSolution.costSolution <= list[v] || currentSolution.costSolution <= previousCostSolution){
            if(currentSolution.costSolution <= bestSolution.costSolution){
                delta = currentSolution.costSolution - bestSolution.costSolution;
                bestSolution = currentSolution; 
            }  
        } else {
            currentSolution.undoLastMove();
        }
        print(bestSolution, currentSolution, (int)tempo_restante(), I, delta);      
        list[v] = currentSolution.costSolution;         
        I++;

		if(first && bestSolution.isFeasible()){
			cout << "Saindo do LAHC: primeira factível encontrada. " << endl;
			break;
		}
    }

    printf("BESTVICK: %s   best=%5d  cur=%5d   t=%3d   it=%lld\n", inst->infoFile.c_str(), bestSolution.costSolution, currentSolution.costSolution, (int)tempo_restante(), I);
    bestSolution.print();

	delete nb;
    return bestSolution;
}


bool is_number(const std::string& s){
    return !s.empty() && std::find_if(s.begin(), s.end(), [](char c) { return !std::isdigit(c); }) == s.end();
}


void copy(double* solTo, double& objTo, double* solFrom, double& objFrom, int cols){

	for(int i=0; i<cols; i++){
		solTo[i] = solFrom[i];
	}
	objTo = objFrom;
}

double diffSol(ZenVar& x, double* s1, double* s2){

	int cols = inst->n_nurses*inst->n_weeks*7*(inst->n_shifts+1)*inst->n_skills;
	int count = 0;
	for(int n=0; n<inst->n_nurses; n++){
		for(int d=0; d<inst->n_weeks*7; d++){
			for(int s=0; s<=inst->n_shifts; s++){
				for(int k=0; k<inst->n_skills; k++){
					int index = x(n+1,d+1,s+1,k+1);
					bool v1 = s1[index] > 0.5;
					bool v2 = s2[index] > 0.5;
					if(v1 == v2){
						count++;
					}                   
				}
			}
		}
	}
	return ((double)count/(double)cols)*100.0;
}


void fixAll(ZenSolver* solver, double* sol, ZenVar& x){

	for(int n=0; n<inst->n_nurses; n++){
		for(int d=0; d<inst->n_weeks*7; d++){
			for(int s=0; s<=inst->n_shifts; s++){
				for(int k=0; k<inst->n_skills; k++){
					int index = x(n+1,d+1,s+1,k+1);
					double v = sol[index];
					if(v>0.5){
					    solver->setColBounds(index,1.0,1.0);
					}else{
					    solver->setColBounds(index,0.0,0.0);
					}
					//solver->setColBounds(index, v, v); //fixar
				}
			}
		}
	}
}

// n: índice do enfermeiro que será desfixado
void unfixNurse(ZenSolver* solver, double* sol, ZenVar& x, int n){

	for(int d=0; d<inst->n_weeks*7; d++){
		for(int s=0; s<=inst->n_shifts; s++){
			for(int k=0; k<inst->n_skills; k++){
				int index = x(n+1,d+1,s+1,k+1);
				solver->setColBounds(index, 0.0, 1.0); //desfixar
			}
		}
	}	
}


void LAFO(Solution* s,int optSTL, double optTimeOut){

	Crono crono;
	crono.run();
    double currentObj = s->costSolution;
    double costSolInicial = currentObj; // custo solução inicial apenas para depois sair no log
   
	//Inicia o solver e carrega o problema
	ZenCallback* zcb = new ZenCallback();
	ZenSolver* solver = new ZenSolver(optMipBackend,zcb,optVerbose);
	solver->loadProblem(optModel,optDatafile);
    ZenVar x = solver->getVar("x");

    double bestSol[solver->getNumCols()]; //array com solução
    double currentSol[solver->getNumCols()];
	double backupSol[solver->getNumCols()];
    fill(currentSol,currentSol+solver->getNumCols(),0); // inicializando o vetor current com zeros

	/* Inicializando a current com a solução factível */
	if(optSol == NULL){
		for(int n=0; n<inst->n_nurses; n++){
		  for(int d=0; d<inst->n_weeks*7; d++){
			int t = s->schedule[n][d].shift;
			int k = inst->skillIndex[s->schedule[n][d].skill];	
		    currentSol[x(n+1,d+1,t+1,k+1)] = 1;			
		  }	
		}
	}else{
		solver->readSolutionFromFile(optSol, currentSol, currentObj);
	}

	double bestObj = currentObj;  // custo do array
	double backupObj = currentObj;
	copy(bestSol, bestObj, currentSol, currentObj, solver->getNumCols());
	vector<double> list;
    for(int i=0; i<optLAFOlist; i++){
        list.push_back(currentObj);
    }

	int n = 0;
	int p = optP;
	int noImprovements = 0;
    int it = 1;
	int I = 0;

	Combination c(inst->n_nurses,p);
	c.next();

    double gapBKS;

	while(crono.getTime() < optTimeOut){
	
		int v = I % optLAFOlist;
		// bloqueia todas as x
		fixAll(solver, currentSol, x);

		gapBKS = (bestObj-BKS[stoi(optInst)]) / min(BKS[stoi(optInst)], bestObj)*100;		
		cout << "LAFO " << (int)crono.getTime() << " (" << it << "/" << c.count()  << ")   " << p << "/" << noImprovements << "   BKS: " << (int)BKS[stoi(optInst)] << "   list: " << list[v] <<"   cur: " << (int)currentObj << "   best: " << (int)bestObj << std::fixed << std::setprecision(1) << "   GAP: " << gapBKS << " %   dif: " << std::setprecision(2) <<diffSol(x, bestSol, currentSol) << endl;	

		// testa a combinação de p enfermeiros sequencialmente e a cada melhora continua da partição que teve melhora e recomeça para testar todos da partição. 
		if(optNeighborhood == 0 && optCirculaVizinho == 1){
			for (int i: c.combination()) { // desfixando os enfermeiros da partição
				unfixNurse(solver, currentSol, x, i);
			}
		}

		// testa a combinação de p enfermeiros sequencialmente e a cada melhora continua da partição que teve melhora, quando termina já começa uma nova partição. 
		if(optNeighborhood == 0 && optCirculaVizinho == 0){
			if(c.next() == false){
				p++; 
		        it = 1;
				c.start(inst->n_nurses, p);
				c.next();
			}
			for (int i: c.combination()) {
				unfixNurse(solver, currentSol, x, i);
			}
		}

  
		copy(backupSol, backupObj, currentSol, currentObj, solver->getNumCols()); // backup = current
		solver->addCutoffRow(list[v]-5.0); // corte para aceitar apenas soluções melhores que a da lista 
		SolutionStatus status = solver->solve(currentSol, currentObj, false /*first improvement*/, optSTL);		
        solver->removeLastRows(1); // remove o cutoff

		switch(status){
			case Z_TL_FEASIBLE: 
			case Z_OPTIMAL:{
				double delta = currentObj - bestObj;
				if(delta < -0.1){
					//cout << "Solução melhorou: " << currentObj << endl;
					copy(bestSol, bestObj, currentSol, currentObj, solver->getNumCols()); // best = current
					solver->saveSolutionToFile(wd+"/"+optInst+".sol", bestSol, bestObj);
					noImprovements = 0;
				}else{
					noImprovements ++;
				}
				list[v] = currentObj; // atualiza a lista	
				break;
			}
			// Se for infactível ou NF é necessário restaurar a sol corrente com backup pois a solução vem corrompida	
			case Z_INFEASIBLE:
			case Z_TL_NOT_FOUND:{
                copy(currentSol, currentObj, backupSol, backupObj, solver->getNumCols()); // current = backup 
				noImprovements ++;
				break;
			}
		}

		if(optNeighborhood == 0 && optCirculaVizinho == 1){
			if(c.next() == false){ // aumenta o tamanho da partição
				if(noImprovements>=it){
					p++; 
				}
				it = 0;
				c.start(inst->n_nurses, p);
				c.next();
			}
		}
        it++;
		I++;
	}

	cout << endl;
	
	double gapInit = (bestObj-costSolInicial) / bestObj*100;	
	cout << "End-LAFO " << "   CostInit: " << costSolInicial << "   GAPInit: " << gapInit << " %" << "   BKS: " << (int)BKS[stoi(optInst)] << "   best: " << (int)bestObj << std::fixed << std::setprecision(1) << "   GAP: " << gapBKS << " %" << endl << endl;
	
    s->readSolverSolution(bestSol, bestObj, x);
	s->geraSolutionFile();
	s->print();
	s->validator();
}

void solveCplex(Solution* s, int timeLimit, char * model){

	double currentObj = 0.0;

	inst->dataSolver(optDatafile);
	ZenCallback* zcb = new ZenCallback();
	ZenSolver* solver = new ZenSolver(optMipBackend,zcb,5);
	solver->loadProblem(model,optDatafile);
	double currentSol[solver->getNumCols()];
	SolutionStatus status = solver->solve(currentSol, currentObj, false /*first improvement*/, timeLimit);
	ZenVar x = solver->getVar("x");
	s->readSolverSolution(currentSol, currentObj, x);

	s->geraSolutionFile();
	s->print();
	s->validator();		

}


int main2(int argc, char** argv){  
    
    time(&g_time1);
    time_t seconds;
	time(&seconds);

	if (optRandomSeed == 0){
        gen.seed(seconds);
        cout << "Seed: " << seconds << endl;
    }else{
        gen.seed(optRandomSeed);
        cout << "Seed: " <<  optRandomSeed << endl;        
    }

	if(optInst == NULL){
		cout << "Erro: instância não foi passada como parâmetro." << endl;
	}

	// Verifica se instancia é número (pré estabelecida) ou string
	if(is_number(optInst) == false){ // --i 1
		inst = new Instance(optInst);	 
	}else{ // --1 nw350
    	inst = new Instance(listInst[stoi(optInst)]); 
	}
	
	wd = optWorkDir;
    optDatafile = wd+"/"+optInst+".dat";

    inst->readFiles(); //inicializa
	Solution s(inst);

	if(optAlg == 1){ // gera arquivo de dados .dat (para o solver) 
		solveCplex(&s, optTimeOut, optModel);	

	}else if(optAlg == 2){ 	// Verifica se solução da cplex está correta
		s.verifySolution();
		s.forcaBruta();
		s.print();
		s.geraSolutionFile();
	
	}else if(optAlg == 3){ // LAHC
	 	cout << "LAHClist: " << optLAHClist << " " << optV1 << " " << optV2 << " " << optV3 << " " << optV4 << " " << optV5 << endl;
		construtivoAleatorio(s);
		s.forcaBruta();
		cout << "Solucao inicial: " << s.costSolution << endl;
		s = LAHC(s);	
		s.geraSolutionFile();
		s.validator();
	
	}else if(optAlg == 5){ // LAHC (sol inicial) + LAFO
		inst->dataSolver(optDatafile);
		construtivoAleatorio(s);
		s.forcaBruta();
		if(optSol == NULL){
			s = LAHC(s, false);
		}
    	LAFO(&s,optSTL, optTimeOut);

	}else if(optAlg == 6){ // cplex + lafo
		inst->dataSolver(optDatafile);
		solveCplex(&s, optTimeInitSol, optSolInitModel);
		s.forcaBruta();
    	LAFO(&s,optSTL, optTimeOut);

	}else if(optAlg == 7){ // cplex
		inst->dataSolver(optDatafile);
		solveCplex(&s, optTimeInitSol, optSolInitModel);
	}
    
	return 0;
}

int main(int argc, char** argv){
   
   cout << "Argumentos: " << endl;
    for(int i=0; i<argc; i++){
        cout << argv[i] << " ";
    }
    cout << endl;
   
    ZenSolver::useSingleCore = true;

    //retirar aspas
    for(int i=0; i<argc; i++){
        int flag = 0;
        for(int s=0; s < strlen(argv[i]); s++){
            if(argv[i][s] == '"'){
                flag = 1;    
            }   
        }
        if(flag == 1){
            for(int s=0; s < strlen(argv[i])-2; s++){
                argv[i][s] = argv[i][s+1];          
            }
            argv[i][strlen(argv[i])-2] = '\0';
        }
    }
	
	OptRegister(&optScenarioFile, const_cast<char*>("sce"), const_cast<char*>("Scenario File"));
	OptRegister(&optInitialHistoryFile, const_cast<char*>("his"), const_cast<char*>("Initial History File"));
	OptRegister(&optWeekDataFile, const_cast<char*>("week"), const_cast<char*>("Week Data File"));
	OptRegister(&optSolutionFileName, const_cast<char*>("sol"), const_cast<char*>("Solution File Name"));
	OptRegister(&optRandomSeed,'r', const_cast<char*>("rand"), const_cast<char*>("Random Seed"));
	OptRegister(&optTimeOut, 't', const_cast<char*>("timeout"), const_cast<char*>("Timeout In Seconds"));
    OptRegister(&optLAHClist,'L', const_cast<char*>("LAHClist"), const_cast<char*>("Tamanho da lista do LAHC"));
    OptRegister(&optInst, 'i', const_cast<char*>("inst"), const_cast<char*>("Número da instância")); 
    OptRegister(&optVerbose, 'v', const_cast<char*>("verbose"), const_cast<char*>("Nivl de verbosidade")); 
    OptRegister(&optTimeInitSol, 'h', const_cast<char*>("timeInitSol"), const_cast<char*>("Tempo limite para gerar soluçao inicial")); 
    OptRegister(&optCplex, const_cast<char*>("c"), const_cast<char*>("1 Gera arquivo para modelo"));   
    OptRegister(&optV1, const_cast<char*>("v1"), const_cast<char*>("Probabilidade da vizinhança 1"));  
    OptRegister(&optV2, const_cast<char*>("v2"), const_cast<char*>("Probabilidade da vizinhança 2"));  
    OptRegister(&optV3, const_cast<char*>("v3"), const_cast<char*>("Probabilidade da vizinhança 3"));  
    OptRegister(&optV4, const_cast<char*>("v4"), const_cast<char*>("Probabilidade da vizinhança 4"));  
    OptRegister(&optV5, const_cast<char*>("v5"), const_cast<char*>("Probabilidade da vizinhança 5"));  
    OptRegister(&optV6, const_cast<char*>("v6"), const_cast<char*>("Probabilidade da vizinhança 6"));  
    OptRegister(&optV7, const_cast<char*>("v7"), const_cast<char*>("Probabilidade da vizinhança 7"));
    OptRegister(&optAlg, 'a', const_cast<char*>("alg"), const_cast<char*>("Opcao 1: MIP, 2: First, 3: First+LB"));
    OptRegister(&optSTL, 's', const_cast<char*>("subproblem"), const_cast<char*>("Tempo limite do subproblema"));  
    OptRegister(&optNeighborhood,'n', const_cast<char*>("neighborhood"), const_cast<char*>("0=rand, 1=combinação"));
    OptRegister(&optLAFOlist,'l', const_cast<char*>("LAFOlist"), const_cast<char*>("Tamanho da lista do LAFOlist"));
	OptRegister(&optN1, const_cast<char*>("n1"), const_cast<char*>("Probabilidade da vizinhança 1"));  
    OptRegister(&optN2, const_cast<char*>("n2"), const_cast<char*>("Probabilidade da vizinhança 2"));  
    OptRegister(&optN3, const_cast<char*>("n3"), const_cast<char*>("Probabilidade da vizinhança 3"));  
    OptRegister(&optN4, const_cast<char*>("n4"), const_cast<char*>("Probabilidade da vizinhança 4"));  
    OptRegister(&optN5, const_cast<char*>("n5"), const_cast<char*>("Probabilidade da vizinhança 5")); 
    OptRegister(&optSol, 'S', const_cast<char*>("solution"), const_cast<char*>("Solução inicial"));  
    OptRegister(&optP, 'p', const_cast<char*>("tamParticao"), const_cast<char*>("Tamanho inicial da partição"));  
    OptRegister(&optWorkDir, 'w', const_cast<char*>("workdir"), const_cast<char*>("Diretório de arquivos temporários"));   
	OptRegister(&optSolInitModel, 'm', const_cast<char*>("solInitModel"), const_cast<char*>("Nome do arquivo do modelo para solução inicial."));    
 	OptRegister(&optCirculaVizinho, 'z', const_cast<char*>("circulaVizinho"), const_cast<char*>("0: não circular, 1: circular"));    
	OptRegister(&optModel, 'o', const_cast<char*>("solModel"), const_cast<char*>("Nome do arquivo do modelo para solução."));    

	optVersion(const_cast<char*>("0.1"));
	optDefaultFile(const_cast<char*>("opcoes.conf"));
	optMain(main2);
	opt(&argc,&argv);
	opt_free();
	return main2(argc, argv);
}







