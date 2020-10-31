#ifndef __ZEN_SOLVER__
#define __ZEN_SOLVER__

#include "../utils/Crono.h"
#include "../utils/Convert.h"
#include "OsiCbcSolverInterface.hpp"
#include "ZenCallback.h"
#include "ZenVar.h"
#include <cplex.h>
#include <glpk.h>
#include <tr1/unordered_map>

using namespace std;

enum SolverType {  
    Z_CPLEX = 0,
    Z_COIN_CBC = 1,
    Z_GLPK = 2
};

enum SolutionStatus {  
    Z_INFEASIBLE = 0,
    Z_TL_NOT_FOUND = 1,
    Z_TL_FEASIBLE = 2,
    Z_OPTIMAL = 3,
};

enum RowSense {
    Z_GT, //Maior ou igual
    Z_LT, //Menor ou igual
    Z_EQ  //igual
};

//TODO Adicionar opção para (des)habilitar cortes
//TODO Adicionar "Hard Cutoff" nos solver 

class ZenSolver {

   public:

      ZenSolver(SolverType type = Z_CPLEX, ZenCallback* callback = NULL, bool logEnabled = true);
      virtual ~ZenSolver();

      OsiSolverInterface* getSolver() const; 
      string loadProblem(string gmplFile, string datFile); /// Carrega um arquivo GMPL
      void loadProblem(string lpFile);                   /// Carrega um arquivo LP
      void setTimeLimit(double seconds);                    /// Define um tempo limite para o solver
      void setStopOnFirstSolution(bool stop);            /// Se stop = true o solver para na 1a solução factível
      bool getStopOnFirstSolution() const;               /// ...
      void setCallback(ZenCallback* callback);           /// Ajusta uma callback
      void removeLastRows(int count = 1);                 /// Remove as últimas \a count rows do modelo
      int addRow(CoinPackedVector& row, RowSense sense, double b); /// Adiciona uma restrição no modelo
      int addRow(CoinPackedVector& row, double a, double b); /// Adiciona uma restrição na forma a <= row <= b
      int addCol(CoinPackedVector& col, double a, double b, double objCoeff, const string& name = "" ); /// Adiciona uma coluna na forma a <= col <= b. O vetor inicial é o coefficiente das variáves.
      ZenCallback* getCallback() const;                  /// Obtém a callback atual.
      void setLogEnabled(bool enabled);                  /// Habilita ou não o log particular do solver
      bool isLogEnabled() const;                         /// ...
      double getObj() const;                             /// Obtém o valor da função objetivo
      double getBestObj() const;                        ///  Obtém um lower bound (minimização) 
      double getGap() const;                             /// Obtém o valor do gap
      int getNumCols() const;                            /// Obtém o número de colunas
      int getNumRows() const;                            /// Obtém o número de linhas
      const double* getSolution() const;                 /// Obtém um ponteiro somente leitura para a solução
      const double* getDuals() const;                 /// Obtém um ponteiro somente leitura para as variáveis duais
      void resetObjective();                       /// Remove a função objetivo
      void copyObjective(double* obj);                 ///Copia os coeficientes da função objetivo
      void setObjective(double* obj);                 ///Define os coeficientes da função objetivo
      void setMipTol(double tol); ///Define para o solver abortar a resolução quando o gap chegar na tolerância
      double getMipTol(); ///Obtém a  tolerância mip relativa
      void getSolution(double* solution) const;          /// Faz uma cópia da solução
      void getRC(double* rc) const;          /// Faz uma cópia dos custos reduzidos
      void getDuals(double* duals) const;          /// Faz uma cópia das variáveis duais
      void getSolution(double* solution, double& obj) const;  /// Faz uma cópia da solução e do valor objetivo
      bool saveSolutionToFile(string file, double* sol, double obj); ///Salva a solução para um arquivo
      bool saveDualToFile(string file, double * duals); ///Salva a solução dual para um arquivo
      void printDuals(); ///Imprime as variáveis duais
      double findObjective(double * solution, bool* mask = NULL); ///Descobre o valor de uma solução fixando suas variáveis no modelo
      double findObjective(double * solution, string varprefix); ///Descobre o valor de uma solução fixando as variáveis no modelo que começam com varprefix 
      double findObjective(double * solution, vector<int>& variables); ///Descobre o valor de uma solução fixando as variáveis no modelo do vetor variables 
      bool readDualFromFile(string file, double* duals); ///Le a solução dual de um arquivo
      bool readSolutionFromFile(string file, double* sol, double& obj); ///Le a solução de um arquivo
      void setColBounds(int col, double lb, double ub);  /// Define os bounds de uma variável 
      int addCutoffRow(double value);
      void relaxIntegrality(); /// Remove a integralidade de todas as variáveis
      void setIntegrality(const vector<int>& variables, bool value); ///Define se as variáveis são integrais ou não conforme value
      void setIntegrality(const set<int>& variables, bool value); ///Define se as variáveis são integrais ou não conforme value
      void fix(const vector<int>& variables, double* sol); ///Fixa as variáveis da solução
      void unfixBinary(const vector<int>& variables); ///Desfixa um conjunto de variáveis binárias 
      void unfixBinary(int col); ///Desfixa um conjunto de variáveis binárias 
      void fix(int col, double value); ///Fixa uma variável para um valor 
      int addLB(double value);
      void getCols(string varprefix, vector<int>& variables); ///Retorna uma lista de colunas das variáveis que começam com varprefix
      void setParallel(int value); //0=default 1=single >=2 multi
      const Crono& getCrono() const;                      /// Obtém uma cópia do ponteiro de tempo da resolução
      void setNumericalEmphasis(int boolean); ///Ativa um cuidado númerico bacana
      ZenVar getVar(const string& var);                  /// Obtém uma variável do problema
      int col(const string& name, const initializer_list<int>& indexes); ///Retornar o índice de uma coluna através do nome e indices
      int row(const string& name, const initializer_list<int>& indexes); ///Retornar o índice de uma linha através do nome e indices
      int col(const string& name); ///Retornar o índice de uma coluna através do nome 
      int row(const string& name); ///Retornar o índice de uma linha através do nome 
      SolutionStatus solve();                            /// Resolve um MIP problem.
      SolutionStatus solve(double* sol, double& obj, bool first = false, double timeLimit = 9999999); /// Resolve um MIP problem.
      SolutionStatus solveLP();                         /// Resolve um problema linear.
      SolutionStatus solveLP(double* sol, double& obj, double timeLimit = 9999999); /// Resolve um problema linear.
      bool isEqual1(double value); /// Verifica se o value == 1.0 conforme a tolerância do solver
      bool isEqual(double a, double b); /// Verifica se a == b conforme a tolerância do solver
      void shuffle(); ///Faz um shuffle na ordem das restrições 
      void setProbe(int value); ///Ativa probing após o pré-processamento
      void setHardToSolve(bool value); ///Ativa configurações para problemas hard to solve =)

   //Static:
      static bool useSingleCore; //Se ligado usa apenas um core (Default: false)
      static volatile int* exitSignal; //Ponteiro para sinal de interrupção.

   private:
      OsiSolverInterface* m_solver;
      double m_timeLimit;
      int m_stopOnFirstSolution;
      SolverType m_type;
      ZenCallback* m_callback;
      bool m_logEnabled;
      bool m_solCount; //numero de soluções
      double m_lastIncumbent; //ultima sol
      int m_cols; //Numero colunas;
      ZenVarMap m_varMap;
      ZenVarMap m_rowMap;
      Crono m_crono;
      int m_solveCount; //Número de vezes que solveLP foi chamado. Útil apenas para decidir entre initialSolve ou resolve

      void GMPL2LP(const char* model, const char* data, const char* lp);
      void indexVars();
      void indexRows();
      CbcModel* getCbcModel() const;
      CPXENVptr getCplexEnv() const;
      CPXLPptr getCplexModel() const;
      glp_iocp* getGlpkEnv() const;
      LPX* getGlpkModel() const;
      friend int cplex_cb (CPXCENVptr env, void *cbdata, int wherefrom, void *cbhandle);

      
};

void testZen();

#endif //__ZEN_SOLVER__
