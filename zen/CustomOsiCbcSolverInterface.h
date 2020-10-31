#include "OsiCbcSolverInterface.hpp"
#include "../utils/Convert.h"

class CustomOsiCbcSolverInterface : virtual public OsiCbcSolverInterface {
  
   public:
     double *lo;
     double *up;
     double objLimit;
     virtual void branchAndBound() {
        int m_cols = getNumCols();

        double lo[m_cols];
        double up[m_cols];
        for (int x=0; x<m_cols; x++) {
            lo[x] = getColLower()[x];
            up[x] = getColUpper()[x];
        }
        getDblParam(OsiDualObjectiveLimit,objLimit);

         modelPtr_->setSolutionCount(0);

         const char * argv2[]={
            "CustomOsiCbcSolverInterface",
            "-heur","on",
            "-presolve","more",
            "-preprocess","sos",
            "-cuts","off",
            "-gomory","on",
            "-probing","on",
            //"-mixed","on",
            //"-cutoff",Convert::toString(modelPtr_->getCutoff()).c_str(),
            "-log",modelPtr_->logLevel()==0?"0":"2",
            "-slog",modelPtr_->logLevel()==0?"0":"2",
            "-solve",
            "-quit"
         };
         CbcMain0(*modelPtr_);
         CbcMain1(19, argv2, *modelPtr_);

         for (int x=0; x<m_cols; x++) {
            setColLower(x,lo[x]);
            setColUpper(x,up[x]);
         }
         setDblParam(OsiDualObjectiveLimit,objLimit);

     };
     //Função redefinida pra corrigir um bug. Cbc Não carrega os nomes de arquivo.  
     virtual int readLp (const char *filename, const double epsilon=1e-5) {
        int status = modelPtr_->solver()->readLp(filename,epsilon);
        return status;
     };



};
