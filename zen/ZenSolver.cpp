#include "ZenSolver.h"
#include "CustomEventHandler.h"
#include "OsiClpSolverInterface.hpp"
#include "CustomOsiCbcSolverInterface.h"
#include "CustomOsiGlpkSolverInterface.h"
#include "OsiCpxSolverInterface.hpp"
#include "CbcCutGenerator.hpp"
#include "CbcHeuristicGreedy.hpp"
#include "CbcHeuristicDive.hpp"
#include "CglGomory.hpp"
#include "CglProbing.hpp"
#include "CglKnapsackCover.hpp"
#include "CglOddHole.hpp"
#include "CglClique.hpp"
#include "CglFlowCover.hpp"
#include "CglMixedIntegerRounding.hpp"
#include "CglMixedIntegerRounding2.hpp"
#include <cplex.h>
#include <glpk.h>
#include <exception>

bool ZenSolver::useSingleCore = false;
volatile int* ZenSolver::exitSignal = NULL;

//Callback do cplex
int cplex_cb (CPXCENVptr env,
       void    *cbdata,
       int     wherefrom,
       void    *cbhandle) {

  
   ZenSolver* solver = (ZenSolver*)cbhandle;
   ZenCallback* callback = solver->getCallback();
   double gap = 0;
   double obj = 0;
   int hasIncumbent = 0;
   CPXgetcallbackinfo(env,cbdata,wherefrom,CPX_CALLBACK_INFO_MIP_FEAS,&hasIncumbent);
   CPXgetcallbackinfo(env,cbdata,wherefrom,CPX_CALLBACK_INFO_BEST_INTEGER,&obj);
   CPXgetcallbackinfo(env,cbdata,wherefrom,CPX_CALLBACK_INFO_MIP_REL_GAP,&gap);

   if (hasIncumbent) {
      if (solver->m_solCount==0 || obj!=solver->m_lastIncumbent) {
         solver->m_solCount++;
         solver->m_lastIncumbent = obj;
         double sol[solver->m_cols];
         CPXgetcallbackincumbent(env,cbdata,wherefrom,sol,0,solver->m_cols-1);
         if (callback->onNewSolution(obj,gap,sol,solver->m_cols,solver->getCrono())) {
            return 1;
         }
      }
   }
   return 0;
}

//callback do glpk
void glpk_cb(glp_tree* tree, void* info) {
   if (glp_ios_reason(tree)==GLP_IBINGO) {
      ZenSolver* solver = ((ZenSolver*)info);
      ZenCallback* callback = solver->getCallback();
      glp_prob* model = glp_ios_get_prob(tree);
      double objval = glp_mip_obj_val(model);
      int cols = glp_get_num_cols(model);
      double x[cols];
      //Copia a solução
      for (int j = 0 ; j < cols ; j++) {
          x[j] = lpx_mip_col_val(model,j+1);
          if (fabs(x[j]) < GLP_ZERO_TOL) {
             x[j] = 0.0;
          }
      }
      double gap = glp_ios_mip_gap(tree);
      bool res = callback->onNewSolution(objval,gap,x,cols,solver->getCrono());
      if (res || solver->getStopOnFirstSolution()){
         glp_ios_terminate(tree); 
      }
   }
   return;
}


ZenSolver::ZenSolver(SolverType type/*Z_CPLEX*/, ZenCallback* callback/*NULL*/, bool logEnabled):
   m_timeLimit(0),
   m_stopOnFirstSolution(false),
   m_type(type),
   m_callback(callback),
   m_solCount(0),
   m_lastIncumbent(0),
   m_cols(0),
   m_solveCount(0) {
   
   switch (m_type) {

      case Z_CPLEX:
         try {
            m_solver = new OsiCpxSolverInterface();
         } catch (CoinError e) {
            cout << "Erro: A licença do CPLEX expirou ou não está instalado corretamente." << endl;
            exit(1);
         }

         //Atualiza o número de núcleos default conforme variável static
         if (useSingleCore) {
            setParallel(1);
         } 

         if (exitSignal!=NULL) {
            CPXsetterminate(getCplexEnv(), exitSignal);
         }
         //CPXsetintparam(getCplexEnv(), CPX_PARAM_MIPEMPHASIS,CPX_MIPEMPHASIS_FEASIBILITY);
         //CPXsetintparam(getCplexEnv(), CPX_PARAM_MIPEMPHASIS,CPX_MIPEMPHASIS_OPTIMALITY);
         //CPXsetintparam(getCplexEnv(), CPX_PARAM_CLIQUES, 3); //Cliques agressivos
         //CPXsetintparam(getCplexEnv(), CPX_PARAM_LPMETHOD, CPX_ALG_BARRIER); //Barrier
         break;

      case Z_COIN_CBC:
         m_solver = new CustomOsiCbcSolverInterface();
         break;

      case Z_GLPK:
         m_solver = new CustomOsiGlpkSolverInterface();
         //getGlpkEnv()->br_tech  = GLP_BR_DTH; //Branching
         //getGlpkEnv()->bt_tech  = GLP_BT_BLB; //backtracking
         getGlpkEnv()->pp_tech  = GLP_PP_ROOT; //Preprocessamento somente no root
         getGlpkEnv()->presolve  = GLP_ON; //MIP Presolve
         //getGlpkEnv()->fp_heur  = GLP_ON; //Feasibility pump
         getGlpkEnv()->gmi_cuts = GLP_ON; //Gomory's cuts
         //getGlpkEnv()->mir_cuts = GLP_ON; //MIR cuts
         getGlpkEnv()->cov_cuts = GLP_ON; //Cover cuts
         //getGlpkEnv()->clq_cuts = GLP_ON; //Clique cuts
         break;

      default:
         cout << "Error! Uncknown solver type!" << endl;
         exit(1);
   }

   m_solver->setHintParam(OsiDoPresolveInInitial,OsiHintDo);
   m_solver->setIntParam(OsiNameDiscipline,2);
   setLogEnabled(logEnabled);

   if (m_callback) {
      setCallback(m_callback);
   }
}

ZenSolver::~ZenSolver(){
   delete m_solver;
}

void ZenSolver::setTimeLimit(double seconds){
   m_timeLimit = seconds;

   switch (m_type) {

      case Z_CPLEX: {
         CPXsetdblparam(getCplexEnv(), CPX_PARAM_TILIM, seconds);
         break;
      }

      case Z_COIN_CBC: {
         getCbcModel()->setMaximumSeconds(seconds);
         break;            
      }

      case Z_GLPK:
         getGlpkEnv()->tm_lim = min((int)seconds*1000,360000000);
         break;

   }

}

OsiSolverInterface* ZenSolver::getSolver() const{
   return m_solver;
}

void ZenSolver::setCallback(ZenCallback* callback){
   m_callback = callback; 

   switch (m_type) {

      case Z_CPLEX: 
         //TODO ver bem o que isso significa
         CPXsetintparam(getCplexEnv(), CPX_PARAM_MIPCBREDLP, CPX_OFF);
         CPXsetinfocallbackfunc(getCplexEnv(), cplex_cb, this);
         break;

      case Z_COIN_CBC: {
         CustomEventHandler handler(this);
         getCbcModel()->passInEventHandler(&handler);
         break;            
      }

      case Z_GLPK:
         getGlpkEnv()->cb_func = glpk_cb;
         getGlpkEnv()->cb_info = this;
         break;
   }
}

void ZenSolver::loadProblem(string lpFile){
   m_solver->readLp(lpFile.c_str());        
   indexVars();
   indexRows();
}

string ZenSolver::loadProblem(string gmplFile, string dataFile){
   string lpFile = dataFile;
   lpFile.replace(lpFile.find_last_of('.'),4,".lp");
   GMPL2LP(gmplFile.c_str(),dataFile.c_str(),lpFile.c_str());
   loadProblem(lpFile);
   return lpFile;
}

void ZenSolver::GMPL2LP(const char* model, const char* data, const char* lp) {
   glp_prob *p = glp_create_prob();
   glp_tran *tran = glp_mpl_alloc_wksp();
   glp_mpl_read_model(tran, model, 1);
   glp_mpl_read_data(tran, data);
   glp_mpl_generate(tran, NULL);
   glp_mpl_build_prob(tran, p);
   glp_write_lp(p,NULL,lp);
   glp_mpl_free_wksp(tran);
   glp_delete_prob(p);
}

SolutionStatus ZenSolver::solve(double* sol, double& obj, bool first/*false*/, double timeLimit/*9999999*/){
   if (m_callback) {
      m_callback->clear();
   }
   setTimeLimit(timeLimit);
   setStopOnFirstSolution(first);
   SolutionStatus status = solve();
   obj = getObj();
   getSolution(sol);
   return status;
}

SolutionStatus ZenSolver::solveLP(double* sol, double& obj, double timeLimit/*9999999*/){
   if (m_callback) {
      m_callback->clear();
   }
   setTimeLimit(timeLimit);
   SolutionStatus status = solveLP();
   obj = getObj();
   getSolution(sol);
   return status;
}

SolutionStatus ZenSolver::solve(){

   CoinAbsFltEq eq;
   m_solCount = 0;
   m_lastIncumbent = 0;
   m_cols = m_solver->getNumCols();

   m_crono.resetAndRun();
   try {
      m_solver->branchAndBound();
   } catch (CoinError e) {
      cout << "Coin Error!"<< endl;
   }
   m_crono.stop();

   switch (m_type) {
      case Z_CPLEX: {
         int status = CPXgetstat(getCplexEnv(),getCplexModel());
         if (m_solver->isProvenOptimal() || status==CPXMIP_TIME_LIM_FEAS || status==CPXMIP_SOL_LIM || status==CPXMIP_ABORT_FEAS) { 
            if (m_callback && (m_solCount == 0 || getObj()!=m_callback->getLastObj())) {
               m_callback->onNewSolution(getObj(),getGap()/100.0,getSolution(),m_cols,getCrono());
            } 
         }
         if (m_solver->isProvenOptimal()) {
            return Z_OPTIMAL;
         } 
         if (status==CPXMIP_TIME_LIM_FEAS || status==CPXMIP_SOL_LIM || status==CPXMIP_ABORT_FEAS) { //Quando um limite de soluçòes é imposto, considera-se que terminou por limite
            return Z_TL_FEASIBLE;
         }
         if (status==CPXMIP_TIME_LIM_INFEAS) {
            return Z_TL_NOT_FOUND;        
         }
         if (status==CPXMIP_UNBOUNDED) {
            cout << "MIP UNBOUNDED STATUS = " << status << ". It could be a bug in CPLEX." << endl;
            exit(1);
         }
         
         break;
      }
      case Z_COIN_CBC: {
         CbcModel* model = getCbcModel();
         if (model->isProvenOptimal()) {
            return Z_OPTIMAL;
         } 
         if (model->isSecondsLimitReached() || model->isSolutionLimitReached() || model->status()==5/*stop on user event*/) {
            if (model->getSolutionCount()>0) {
               return Z_TL_FEASIBLE; 
            } 
            return Z_TL_NOT_FOUND;
         }
         break;            
      }
      case Z_GLPK:
         int status = glp_mip_status(getGlpkModel());
         if (status == GLP_OPT) {
            return Z_OPTIMAL;
         } 
         if (status == GLP_FEAS) {
            return Z_TL_FEASIBLE; 
         }
         if (status == GLP_UNDEF) {
            return Z_TL_NOT_FOUND;
         }
         break;
   }

   return Z_INFEASIBLE;
}

SolutionStatus ZenSolver::solveLP(){

   CoinAbsFltEq eq;
   m_solCount = 0;
   m_lastIncumbent = 0;
   m_cols = m_solver->getNumCols();

   m_crono.resetAndRun();
   if (m_solveCount == 0) {
      m_solver->initialSolve();
   } else {
      m_solver->resolve();
   }
   m_solveCount++;
   m_crono.stop();

   if (m_solver->isProvenOptimal()) {
      return Z_OPTIMAL;
   }

   return Z_INFEASIBLE;
}

void ZenSolver::setLogEnabled(bool enabled){
   m_logEnabled = enabled;
   switch (m_type) {
      case Z_CPLEX:
         m_solver->messageHandler()->setLogLevel(enabled?2:0);
         break;
      case Z_COIN_CBC:
         //No coin o log precisa ser (des)ativado no modelo.
         getCbcModel()->setLogLevel(enabled?2:0);
         break;
      case Z_GLPK:
         getGlpkEnv()->msg_lev = enabled?GLP_MSG_ON:GLP_MSG_OFF;
         break;
   }
}

bool ZenSolver::isLogEnabled() const{
   return m_logEnabled;
}

double ZenSolver::getObj() const {
   return m_solver->getObjValue();
}

ZenCallback* ZenSolver::getCallback() const{
   return m_callback;
}

bool ZenSolver::getStopOnFirstSolution() const{
   return m_stopOnFirstSolution;
}

void ZenSolver::setStopOnFirstSolution(bool stop){
   m_stopOnFirstSolution = stop;
   switch (m_type) {
      case Z_CPLEX: 
         CPXsetintparam(getCplexEnv(), CPX_PARAM_INTSOLLIM, stop?1:9999999);
         break;
      case Z_COIN_CBC: {
         getCbcModel()->setMaximumSolutions(stop?1:9999999);
         break;            
      }
      case Z_GLPK:
         //No glpk isso é tratado na callback mas no caso de querer uma sol factível é
         //melhor ativar o feasibility pump
         getGlpkEnv()->fp_heur  = stop?GLP_ON:GLP_OFF; //Feasibility pump
         break;
   }
}

CbcModel* ZenSolver::getCbcModel() const{
   OsiCbcSolverInterface* solver = dynamic_cast<OsiCbcSolverInterface*> (m_solver);
   return solver->getModelPtr();
}

CPXENVptr ZenSolver::getCplexEnv() const{
   OsiCpxSolverInterface* solver = dynamic_cast<OsiCpxSolverInterface*> (m_solver);
   return solver->getEnvironmentPtr();
}

CPXLPptr ZenSolver::getCplexModel() const{
   OsiCpxSolverInterface* solver = dynamic_cast<OsiCpxSolverInterface*> (m_solver);
   return solver->getLpPtr();
}

glp_iocp* ZenSolver::getGlpkEnv() const{
   CustomOsiGlpkSolverInterface* solver = dynamic_cast<CustomOsiGlpkSolverInterface*> (m_solver);
   return solver->getEnvPtr();
}

LPX* ZenSolver::getGlpkModel() const{
   CustomOsiGlpkSolverInterface* solver = dynamic_cast<CustomOsiGlpkSolverInterface*> (m_solver);
   return solver->getModelPtr();
}

void ZenSolver::removeLastRows(int count/*1*/){
   int del[count];
   for (int i=1; i<=count; i++) {
       del[i-1] = m_solver->getNumRows()-i;
   }
   m_solver->deleteRows(count,del);
}

int ZenSolver::addRow(CoinPackedVector& row, RowSense sense, double b){
   switch (sense) {
      case Z_EQ:
         m_solver->addRow(row,b,b);
         break;
      case Z_LT: // <=
         m_solver->addRow(row,-1.0*m_solver->getInfinity(),b);
         break;
      case Z_GT: // >=
         m_solver->addRow(row,b,m_solver->getInfinity());
         break;
   }
   return m_solver->getNumRows()-1;
}

void ZenSolver::indexVars() {
   m_varMap.clear();
   int cols = m_solver->getNumCols();
   for (int i=0; i < cols; i++) {
      string name = m_solver->getColName(i);
      m_varMap[name] = i; 
   }
}

void ZenSolver::indexRows() {
   m_rowMap.clear();
   int rows = m_solver->getNumRows();
   for (int i=0; i < rows; i++) {
      string name = m_solver->getRowName(i);
      m_rowMap[name] = i; 
   }
}

ZenVar ZenSolver::getVar(const string& var) {
   return ZenVar(var,&m_varMap);
}

const double* ZenSolver::getSolution() const{
   return m_solver->getColSolution();
}

const double* ZenSolver::getDuals() const{
   return m_solver->getRowPrice();
}

void ZenSolver::getSolution(double* solution) const{
   int cols = m_solver->getNumCols();
   for (int i=0; i < cols; i++) {
      solution[i] = m_solver->getColSolution()[i];
   }
}

void ZenSolver::getRC(double* rc) const{
   int cols = m_solver->getNumCols();
   for (int i=0; i < cols; i++) {
      rc[i] = m_solver->getReducedCost()[i];
   }
}

void ZenSolver::getDuals(double* duals) const{
   int rows = m_solver->getNumRows();
   for (int i=0; i < rows; i++) {
      duals[i] = m_solver->getRowPrice()[i];
   }
}

const Crono& ZenSolver::getCrono() const{
   return m_crono;
}

int ZenSolver::getNumCols() const{
   return m_solver->getNumCols();
}


int ZenSolver::addRow(CoinPackedVector& row, double a, double b){
   m_solver->addRow(row,a,b);
   return m_solver->getNumRows()-1;
}


int ZenSolver::getNumRows() const{
   return m_solver->getNumRows();
}

void ZenSolver::getSolution(double* solution, double& obj) const{
   getSolution(solution);
   obj = getObj();
}

bool ZenSolver::isEqual1(double value){
   return isEqual(value,1.0);
}

bool ZenSolver::isEqual(double a, double b){
   double tol = m_solver->getIntegerTolerance();
   CoinAbsFltEq eq(tol);
   return eq(a, b);
}

void ZenSolver::resetObjective(){
   for (int i=0; i<getNumCols(); i++) {
      m_solver->setObjCoeff(i,0);
   }
}

void ZenSolver::copyObjective(double *obj) {
   for (int i=0; i<getNumCols(); i++) {
      obj[i] = m_solver->getObjCoefficients()[i];
   }
}

void ZenSolver::setObjective(double* obj){
   for (int i=0; i<getNumCols(); i++) {
      m_solver->setObjCoeff(i,obj[i]);
   }
}

int ZenSolver::addCutoffRow(double value){
   CoinPackedVector row;
   for (int i=0; i<getNumCols(); i++) {
      row.insert(i,m_solver->getObjCoefficients()[i]);
   }
   return addRow(row, Z_LT, value);
}

int ZenSolver::addLB(double value){
   CoinPackedVector row;
   for (int i=0; i<getNumCols(); i++) {
      row.insert(i,m_solver->getObjCoefficients()[i]);
   }
   return addRow(row, Z_GT, value);
}

void ZenSolver::setColBounds(int col, double lb, double ub){
   if (m_type==Z_COIN_CBC) {
      m_solver->setColBounds(col,lb,ub);
      getCbcModel()->solver()->setColBounds(col,lb,ub);
   } else {
      m_solver->setColBounds(col,lb,ub);
   }
}

bool ZenSolver::saveSolutionToFile(string file, double* sol, double obj) {
   ofstream f(file);
   if (f.is_open()) { 
      f << obj << endl;
      for (int i=0; i<getNumCols(); i++) {
         f << sol[i] << endl;
      }
      f.close();
      cout << "Solution of value " << obj << " saved to " << file << endl;
      return true;
   } else {
      return false;
   }
}

bool ZenSolver::saveDualToFile(string file, double* duals) {
   ofstream f(file);
   if (f.is_open()) { 
      for (int i=0; i<getNumRows(); i++) {
         f << duals[i] << endl;
         printf("Dual %s = %f\n",getSolver()->getRowName(i).c_str(),duals[i]);
      }
      f.close();
      cout << "Duals saved to " << file << endl;
      return true;
   } else {
      return false;
   }
}

bool ZenSolver::readDualFromFile(string file, double* duals) {
   ifstream f(file);
   if (f.is_open()) { 
      for (int i=0; i<getNumRows(); i++) {
         f >> duals[i];
      }
      f.close();
      cout << "Duals loaded from " << file << endl;
      return true;
   } else {
      return false;
   }
}

bool ZenSolver::readSolutionFromFile(string file, double* sol, double& obj) {
   ifstream f(file);
   if (f.is_open()) { 
      f >> obj;
      for (int i=0; i<getNumCols(); i++) {
         f >> sol[i];
      }
      f.close();
      cout << "Solution of value " << obj << " loaded from " << file << endl;
      return true;
   } else {
      return false;
   }
}
double ZenSolver::getGap() const{
   switch (m_type) {
      case Z_CPLEX: {
         double gap;
         int status = CPXgetmiprelgap (getCplexEnv(), getCplexModel(), &gap);
         if (status==0) {
            return gap*100;
         }
         break;
      }
      case Z_COIN_CBC:
         cerr << "ZenSolver::getGap() not implemented for COIN CBC yet!" << endl;
         exit(1);
         break;
      case Z_GLPK:
         cerr << "ZenSolver::getGap() not implemented for GLPK yet!" << endl;
         exit(1);
         break;
   }
   return -1;
}

double ZenSolver::findObjective(double * solution, bool* mask){
   double obj = 0;
   int added = 0;
   for (int i=0; i<getNumCols(); i++) {
      if (mask==NULL || mask[i] ) { //Só fixa as variáveis cuja mascara é true
         CoinPackedVector row;
         row.insert(i,1);
         addRow(row,Z_EQ,solution[i]);
         added++;
      }
   }
   if (solve(solution, obj)!=Z_OPTIMAL) {
      cerr << "findObjective() erro!" << endl; 
      exit(1);
   }  
   if (added>0) {
       removeLastRows(added);
   }
   return obj;
}


int ZenSolver::col(const string& name, const initializer_list<int>& indexes){
   string key = name + "(" + Util::csv(indexes) + ")";
   int size = (int)m_varMap.size();
   int index = m_varMap[key];
   if (size != (int) m_varMap.size()) {
      cout << "ZenVar " << key << " não existe." << endl;
      exit(1); 
   }
   return index;
}

int ZenSolver::row(const string& name, const initializer_list<int>& indexes){
   string key = name + "(" + Util::csv(indexes) + ")";
   int size = (int)m_rowMap.size();
   int index = m_rowMap[key];
   if (size != (int) m_rowMap.size()) {
      cout << "ZenRow " << key << " não existe." << endl;
      exit(1); 
   }
   return index;
}

int ZenSolver::row(const string& name){
   int size = (int)m_rowMap.size();
   int index = m_rowMap[name];
   if (size != (int) m_rowMap.size()) {
      cout << "ZenRow '" << name << "' não existe." << endl;
      exit(1); 
   }
   return index;
}

int ZenSolver::col(const string& name){
   int size = (int)m_varMap.size();
   int index = m_varMap[name];
   if (size != (int) m_varMap.size()) {
      cout << "ZenVar '" << name << "' não existe." << endl;
      exit(1); 
   }
   return index;
}

int ZenSolver::addCol(CoinPackedVector& col, double a, double b, double objCoeff, const string& name/*"" */){
   m_solver->addCol(col,a,b,objCoeff,name);
   if (name.length()>0) {
      m_varMap[name]=m_solver->getNumCols()-1;   
   }
   return m_solver->getNumCols()-1;
}

void ZenSolver::setMipTol(double tol){
   switch (m_type) {
      case Z_CPLEX: {
         CPXsetdblparam(getCplexEnv(),CPX_PARAM_EPGAP,tol);
         break;
      }
      case Z_COIN_CBC:
         cerr << "ZenSolver::setMipTol() not implemented for COIN CBC yet!" << endl;
         exit(1);
         break;
      case Z_GLPK:
         getGlpkEnv()->mip_gap = tol; 
         break;
   }
}

void ZenSolver::setNumericalEmphasis(int boolean) {
   CPXsetintparam(getCplexEnv(), CPX_PARAM_NUMERICALEMPHASIS,boolean);
}

double ZenSolver::getMipTol(){
   double tol = 0;
   switch (m_type) {
      case Z_CPLEX: {
         CPXgetdblparam(getCplexEnv(),CPX_PARAM_EPGAP,&tol);
         break;
      }
      case Z_COIN_CBC:
         cerr << "ZenSolver::setMipTol() not implemented for COIN CBC yet!" << endl;
         exit(1);
         break;
      case Z_GLPK:
         tol = getGlpkEnv()->mip_gap; 
         break;
   }
   return tol;
}


double ZenSolver::getBestObj() const{
   double objval = 0;
   CPXgetbestobjval (getCplexEnv(), getCplexModel(), &objval);
   return objval;
}

void ZenSolver::setParallel(int value){
   CPXsetintparam(getCplexEnv(),CPX_PARAM_THREADS,value);
}

void ZenSolver::shuffle(){
   int rows = getNumRows();
   int cols = getNumCols();
   double rowUpper[rows];
   double rowLower[rows];
   CoinPackedMatrix m = *getSolver()->getMatrixByRow();
   vector<int> indexes;
   for (int r = 0; r < rows; r++) {
      indexes.push_back(r);
      rowUpper[r] = getSolver()->getRowUpper()[r];
      rowLower[r] = getSolver()->getRowLower()[r];
   }
   random_shuffle(indexes.begin(),indexes.end());
   removeLastRows(rows);
   for (int r: indexes) {
      CoinPackedVector row;
      for (int c = 0; c < cols; c++) {
         row.insert(c,m.getCoefficient(r,c));
      }
      m_solver->addRow(row,rowLower[r],rowUpper[r]);
   }
   
   
}

void ZenSolver::setProbe(int value){
   CPXsetintparam(getCplexEnv(), CPX_PARAM_PROBE, value); //Probing agressivo
}

void ZenSolver::setHardToSolve(bool value){
   if (value) {
      CPXsetintparam(getCplexEnv(), CPX_PARAM_PROBE, 3); //Probing agressivo
      CPXsetintparam(getCplexEnv(), CPX_PARAM_CLIQUES, 3); //Cliques agressivo
      CPXsetintparam(getCplexEnv(), CPX_PARAM_HEURFREQ, -1); //Desabilita heurísticas
      CPXsetintparam(getCplexEnv(), CPX_PARAM_ZEROHALFCUTS, 2); //Desabilita heurísticas
   } else {
      CPXsetintparam(getCplexEnv(), CPX_PARAM_PROBE, 0); //Probing agressivo
      CPXsetintparam(getCplexEnv(), CPX_PARAM_CLIQUES, 0); //Cliques agressivo
      CPXsetintparam(getCplexEnv(), CPX_PARAM_HEURFREQ, 0); //Desabilita heurísticas
      CPXsetintparam(getCplexEnv(), CPX_PARAM_ZEROHALFCUTS, 0); //Desabilita heurísticas
   }
}

void ZenSolver::relaxIntegrality(){
   int cols = getNumCols();
   for (int col=0; col<cols; col++) {
      getSolver()->setContinuous(col); 
   }
}

void ZenSolver::setIntegrality(const vector<int>& variables, bool active){
   for (int col: variables) {
      if (active) {
         getSolver()->setInteger(col); 
      } else {
         getSolver()->setContinuous(col); 
      }
   }
}

void ZenSolver::fix(const vector<int>& variables, double* sol){
   for (int index: variables) {
      m_solver->setColBounds(index,sol[index],sol[index]);
   }
}

void ZenSolver::fix(int col, double value){
   m_solver->setColBounds(col,value,value);
}

void ZenSolver::unfixBinary(const vector<int>& variables){
   for (int index: variables) {
      m_solver->setColBounds(index,0.0,1.0);
   }
}

void ZenSolver::unfixBinary(int col){
   m_solver->setColBounds(col,0.0,1.0);
}

void ZenSolver::setIntegrality(const set<int>& variables, bool value){
   for (int col: variables) {
      if (value) {
         getSolver()->setInteger(col); 
      } else {
         getSolver()->setContinuous(col); 
      }
   }
}

void ZenSolver::getCols(string varprefix, vector<int>& variables){
   int cols = m_solver->getNumCols();
   for (int i=0; i < cols; i++) {
      string name = m_solver->getColName(i);
      if (Util::startswith(name,varprefix)) {
         variables.push_back(i);
      }
   }
}

double ZenSolver::findObjective(double * solution, string varprefix){
   vector<int> variables;
   getCols(varprefix,variables);
   return findObjective(solution,variables);
}

double ZenSolver::findObjective(double * solution, vector<int>& variables){
   double obj = 0;
   int added = 0;
   for (int i: variables) {
      CoinPackedVector row;
      row.insert(i,1);
      addRow(row,Z_EQ,solution[i]);
      added++;
   }
   if (solve(solution, obj)!=Z_OPTIMAL) {
      cerr << "findObjective() erro!" << endl; 
      exit(1);
   }  
   //getSolver()->writeLp("/tmp/fixed");
   if (added>0) {
       removeLastRows(added);
   }
   return obj;
}

void ZenSolver::printDuals(){
   int rows = getNumRows();
   for (int i=0; i<rows; i++) {
      double dual = getDuals()[i];
      if (!isEqual(dual,0)) {
         printf("Dual %s = %f\n",getSolver()->getRowName(i).c_str(),dual);
      }
   }
}


