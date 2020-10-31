#ifndef __ZEN_CALLBACK__
#define __ZEN_CALLBACK__

#include "ZenCallback.h"
#include "../utils/Colors.h"

using namespace std;

class ZenCallback {
   
   public:

      ZenCallback(){
         clear();
      }

      virtual ~ZenCallback(){
         //
      }

      void clear() {
         m_firstTime=-1;
         m_firstObj=-1;
         m_lastTime=-1;
         m_lastObj=-1;
      }

      /** 
       * @brief Evento invocado quando o solver encontra uma nova solução factível.
       * 
       * @param obj
       * @param gap
       * @param sol
       * @param cols
       * @param crono
       * 
       * @return True para terminar a resolução, False para continuar.
       */
      virtual bool onNewSolution(double obj, double gap, const double* sol, int cols, const Crono& crono) {
         printf( CL8 "[ZEN] " CL "%s " CLC "%10.2f" CLA " %7.2f%%\n" CL, crono.getTimeStr(false).c_str(), obj, gap*100); 
         if (m_firstTime < 0) {
            m_firstTime = crono.getTime();
            m_firstObj = obj;
         }
         m_lastTime = crono.getTime();
         m_lastObj = obj;
         return false;
      } 

      /** 
       * Retorna o tempo em que a primeira solução factível foi encontrada ou
       * -1 caso nenhum solução tenha sido encontrada
       * 
       * @return double
       */
      double getFirstTime() const {
         return m_firstTime;
      }

      /** 
       * Retorna o valor objetivo da primeira solução factível encontrada ou
       * -1 caso nenhum solução tenha sido encontrada
       * 
       * @return double
       */
      double getFirstObj() const {
         return m_firstObj;
      }

      /** 
       * Retorna o tempo em que a última solução factível foi encontrada ou
       * -1 caso nenhum solução tenha sido encontrada
       * 
       * @return double
       */
      double getLastTime() const {
         return m_lastTime;
      }

      /** 
       * Retorna o valor da última solução factível 
       * -1 caso nenhum solução tenha sido encontrada
       * 
       * @return double
       */
      double getLastObj() const {
         return m_lastObj;
      }

   protected:
      double m_firstTime;
      double m_firstObj;
      double m_lastTime;
      double m_lastObj;

};

#endif //__ZEN_CALLBACK__

