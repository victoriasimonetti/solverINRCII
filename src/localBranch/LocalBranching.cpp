
#include "LocalBranching.h"
#include <iostream>
#include <fstream>
#include <iomanip>
#include <sstream>

LocalBranching::LocalBranching(Instance* inst, ZenSolver* solver, ZenVar& x, double* sol, double obj,  int totalTimeLimit, int nodeTimeLimit, int maxDiversifications, int k, double alpha, bool assymetric, bool exact):
   m_solver(solver),
   m_k(k),
   m_alpha(alpha),
   m_assymetric(assymetric),
   m_totalTimeLimit(totalTimeLimit),
   m_nodeTimeLimit(nodeTimeLimit),
   m_maxDiversifications(maxDiversifications),
   m_phase1Time(0),
   m_usedDiversifications(0),
   m_exact(exact){

   for(int n=0; n<inst->n_nurses; n++){
      for(int d=0; d<inst->n_weeks*7; d++){
		for(int s=0; s<inst->n_shifts; s++){ 
			for(int k=0; k<inst->n_skills; k++){
				m_branchVars.push_back(x(n+1,d+1,s+1,k+1));
			}				
		}	
	  }	
   }
   
   m_startRows = solver->getNumRows();

   m_bestSol = new double[m_solver->getNumCols()];
   m_currSol = new double[m_solver->getNumCols()];
   m_refSol = new double[m_solver->getNumCols()];

   //Copia a solução incumebente
   m_bestObj = obj;
   copy(m_bestSol, m_bestObj, sol, m_bestObj);
   copy(m_currSol, m_currObj, sol, m_bestObj);
   copy(m_refSol , m_refObj, sol, m_bestObj);

   if (nodeTimeLimit==0) {
      m_nodeTimeLimit = numeric_limits<int>::max();
   }

}

void LocalBranching::copy(double* solTo, double& objTo,  double* solFrom, double& objFrom) {
   int cols = m_solver->getNumCols();
   for(int i=0; i < cols; i++) {
      solTo[i] = solFrom[i];
   }
   objTo = objFrom;
}

void LocalBranching::addLocalbranchConstraint(double* sol, RowSense op, int k) {

   CoinAbsFltEq eq;
   CoinPackedVector row;

   double b = k;
   for (int index: m_branchVars) { 
      if (eq(sol[index],1.0)) {
         b--; 
         row.insert(index,-1);
      } else if (!m_assymetric){
         row.insert(index,1);
      }
   }
   m_solver->addRow(row,op,b);
   cout << "[" << m_crono.getTimeStr(true) << "] " CL1 "(+) " CL << "row " << (op==Z_LT? "<= ":">= ") << k << endl;
}

void LocalBranching::addLocalbranchConstraint(double* sol, int k1, int k2) {
   cout << "[" << m_crono.getTimeStr(true) << "] " CL1 "(+) " CL << k1 << " <= " << "k" << " <= " << k2 << endl;

   CoinAbsFltEq eq;
   CoinPackedVector row;

   double b1 = k1;
   double b2 = k2;
   for (int index: m_branchVars) { 
      if (eq(sol[index],1.0)) {
         b1--; 
         b2--; 
         row.insert(index,-1);
      } else if (!m_assymetric){
         row.insert(index,1);
      }
   }
   m_solver->addRow(row,b1,b2);
}

bool LocalBranching::solve(double* solution, double& obj, double& lb){

   m_crono.resetAndRun();

   const int INF = 9999999;

   int it = 0;
   int k = m_k;
   int dv = 0;
   int diversify = 0;
   bool first = false;
   int timeLimit = m_nodeTimeLimit;
   double upperBound = m_bestObj;
   int improv = 0;

   do { //MainLoop
      it++;
      printf("\n[%s] Iter: %5d    Dv: %3d/%d    Best: %10.2f\n", m_crono.getTimeStr(true).c_str(), it, dv, m_maxDiversifications, m_bestObj);

      addLocalbranchConstraint(m_refSol,Z_LT,k);

      timeLimit = min(timeLimit, m_totalTimeLimit-(int)m_crono.getTime());
      //m_solver->addCutoffRow(upperBound-1);
      SolutionStatus st = m_solver->solve(m_currSol, m_currObj, first , timeLimit);
      timeLimit = m_nodeTimeLimit;
      m_solver->removeLastRows(1); // remove local branch constraint and cuttof rows

      double delta = m_currObj-m_bestObj;

      switch (st) {
         case Z_TL_FEASIBLE:
         case Z_OPTIMAL: 
            cout << "[" << m_crono.getTimeStr(false) << " " CLB << (st==Z_OPTIMAL?"OPT":" TL") << CL << "] Obj: " << CL2 << m_currObj << CL;
            if (delta<-0.1) {
               cout << CLA " New! (" << delta << ")" CL;
               copy(m_bestSol,m_bestObj,m_currSol,m_currObj);
               improv++;
            }
            cout << endl;
            if (st == Z_OPTIMAL) { 
               addLocalbranchConstraint(m_refSol,Z_GT, k+1); //Ao provar o ótimo garante uma distancia da referencia.
            } else {
               //Feasible
               if (!first) {
                  addLocalbranchConstraint(m_refSol,Z_GT, 1); //Ao terminar por tempo, precisa garantir uma solução diferente da atual
               }
            }
            copy(m_refSol,m_refObj,m_currSol,m_currObj);
            k = m_k;
            diversify = 0;
            first = false;
            upperBound = m_currObj;
            break;
         
         case Z_INFEASIBLE:
            //Quando a solução é infactível
            cout << "[" << m_crono.getTimeStr(false) << CLB" INF" CL << "] " << (diversify?"":"Soft diversification ON") << endl;
            addLocalbranchConstraint(m_refSol,Z_GT, k+1); //Ao provar infeasible garante uma distancia da referencia.
            if (diversify) {
               dv++;
               cout << "[" << m_crono.getTimeStr(true) << "] " << CL1" Hard Diversification " << dv << CL << endl;
               upperBound = timeLimit = INF;
               first = true;
            }
            k = k + (int)((double)k/m_alpha)+1;
            diversify++;
            break;
         case Z_TL_NOT_FOUND:
            //Quando não encontra, tenta intensificar diminuindo a vizinhança.
            cout << "[" << m_crono.getTimeStr(false) << CLB"  NF" CL << "] " << (diversify?"":"Soft intensification ON") << endl;
            if (diversify) {
               dv++;
               cout << "[" << m_crono.getTimeStr(true) << "] " << CL1" Hard Diversification " << dv << CL << endl;
               upperBound = timeLimit = INF;
               first = true;
               addLocalbranchConstraint(m_refSol,Z_GT, 1); //Garante uma solução diferente da atual referencia
               k = k + (int)((double)k/m_alpha)+1;
            } else {
               k = k - (int)((double)k/m_alpha);
            }
            diversify++; //Embora não estejamos diversificando, esta flag é ativada para ativar a diversificação.
            break;
      } 
   } while ((int)m_crono.getTime()<m_totalTimeLimit and dv<=m_maxDiversifications and (m_bestObj-lb)>0.1);

   m_phase1Time = m_crono.getTime();
   m_usedDiversifications = min(dv,m_maxDiversifications);
   
   copy(solution,obj,m_bestSol,m_bestObj);

   cout << "Main Loop Exit." << endl;

   //Aqui finalizou pelo máximo de diversificação.
   timeLimit = m_totalTimeLimit - (int)m_crono.getTime();
   SolutionStatus st = Z_TL_FEASIBLE; 
   
   if (!m_exact) {
      cout << "Enhancement phase passed via configuration." << endl;
   } else if (timeLimit > 0) {
      cout << "Starting enhancement phase." << endl;
      m_solver->setLogEnabled(true);
      m_solver->addCutoffRow(m_bestObj-1);
      st = m_solver->solve(m_bestSol, m_bestObj, false, timeLimit);
   } else {
      cout << "Enhancement phase passed because there is not enough time." << endl;
   }
   
   m_crono.stop();

   if (st==Z_OPTIMAL || st==Z_TL_FEASIBLE) {
      copy(solution,obj,m_bestSol,m_bestObj);
   }

   removeAllAddedConstraints();

   return (improv>0);
}

void LocalBranching::getCurrentSolution(double* solution, double& obj) {
   copy(solution,obj,m_refSol,m_refObj);
}



void LocalBranching::removeAllAddedConstraints(){
   m_solver->removeLastRows(m_solver->getNumRows()-m_startRows);
}



double LocalBranching::getPhase1Time() const{
   return m_phase1Time;
}

int LocalBranching::getUsedDiversifications() const{
   return m_usedDiversifications;
}


