#ifndef __LOCAL_BRANCHING__
#define __LOCAL_BRANCHING__

#include "../zen/ZenSolver.h" 
#include "../../include/Instance.h"

using namespace std;

extern int optMaxIterations;
extern volatile int gGotExitSignal;

class LocalBranching {
   
   public:
      /** 
       * @brief Inicializa o algoritmo de local branching
       * 
       * @param solver ZenSolver já inicializado
       * @param ZenVar variável foco da restrição de LocalBranch.
       * @param sol Vetor com uma solução inicial factível.
       * @param obj Valor objetivo da solução inicial 
       * @param totalTimeLimit Tempo limite do processo
       * @param nodeTimeLimit Tempo limite por nó no local branch
       * @param maxDiversificationLimit Número máximo de diversificações
       * @param k Tamanho da vizinhança
       * @param alpha Fator de intensificação ou diversificação soft usado ao atualizar o valor de k na equação: k = k ± k/alpha +1 
       * @param assymetric Se desativado a restrição de local branch considera apenas as mudanças de 0 para 1 nas variáveis. Se ativado inclui também as mudanças de 1 0.
       * @param exact Se ativo, ao exceder o limite de diversificação, lança o solver com o tempo restante para resolver exatamente. 
       */
      LocalBranching(Instance* inst, ZenSolver* solver, ZenVar& x, double* sol, double obj, int totalTimeLimit, int nodeTimeLimit, int maxDiversificationLimit, int k = 4, double alpha = 2, bool assymetric = true, bool exact = true);
      bool solve(double* solution, double& obj, double& lb);
      void getCurrentSolution(double* solution, double& obj);
      double getPhase1Time() const;
      int getUsedDiversifications() const;

   private:
      ZenSolver* m_solver;
      Crono m_crono;
      int m_k; 
      double m_alpha;
      int m_startRows;
      vector<int> m_branchVars;
      bool m_assymetric;
      int m_totalTimeLimit;
      int m_nodeTimeLimit;
      int m_maxDiversifications;
      double m_phase1Time;
      int m_usedDiversifications;
      bool m_exact;

      double* m_bestSol;
      double* m_currSol;
      double* m_refSol;
      double m_bestObj;
      double m_currObj;
      double m_refObj;

      void addLocalbranchConstraint(double* sol, RowSense op, int k); 
      void addLocalbranchConstraint(double* sol, int k1, int k2); 
      void removeAllAddedConstraints();
      void copy(double* solTo, double& objTo,  double* solFrom, double& objFrom);
  
};

#endif //__LOCAL_BRANCHING__
