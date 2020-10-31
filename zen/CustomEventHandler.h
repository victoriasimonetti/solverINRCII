#ifndef __CUSTOM_EVENT_HANDLER__
#define __CUSTOM_EVENT_HANDLER__

using namespace std;

#include "CbcEventHandler.hpp"
#include "CbcModel.hpp"
#include "ZenSolver.h"

class CustomEventHandler: public CbcEventHandler {
   public:
      CustomEventHandler(ZenSolver* zenSolver = NULL): CbcEventHandler(), m_zenSolver(zenSolver) {};
      CustomEventHandler(CbcModel* model): CbcEventHandler(model) {};
      CustomEventHandler(const CustomEventHandler& rhs) {*this=rhs;};
      CustomEventHandler& operator=(const CustomEventHandler& rhs) {
         if (this!=&rhs) 
           CbcEventHandler::operator=(rhs); 
           m_zenSolver = rhs.m_zenSolver;
         return *this;
      };
      CustomEventHandler* clone() const {
         return new CustomEventHandler(*this);
      };
      virtual ~CustomEventHandler() {};
      virtual CbcAction event(CbcEvent eventCode) {
         if (m_zenSolver!=NULL) {
            if (eventCode == solution || eventCode==heuristicSolution || (eventCode==endSearch && model_->getMaximumSolutions()==1 && model_->getSolutionCount()>0)) {
               //O Coin não oferece gap? Calculamos manualmente o gap então..
               double best = model_->getBestPossibleObjValue();
               double obj = model_->getObjValue();
               double gap = 1;
               if (best!=0) {
                  gap = (best-obj)/best;
               } else if (best==obj) {
                  gap = 0;
               }
               if (m_zenSolver->getCallback()->onNewSolution(obj, fabs(gap), model_->bestSolution(),model_->getNumCols(), m_zenSolver->getCrono())) {
                  return stop; //Ação de stop
               }   
            } 
         }
         return CbcEventHandler::event(eventCode);
      };

   private:
      ZenSolver* m_zenSolver;

};

#endif //__CUSTOM_EVENT_HANDLER__

