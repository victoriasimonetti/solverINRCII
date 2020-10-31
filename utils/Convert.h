#ifndef __CONVERT_H__
#define __CONVERT_H__
using namespace std;
#include <string>
#include "Util.h"

namespace Convert{
      string toString(double value);
      string toString(int value);
      string toStr(double value);
      string toStr(double value, int decimals);
      string toStr(int value);
      string toStr(long value);

      /** 
       * Converte um número para string se for não negativo, senão retorna NA.
       * Esse formato de dados é compatível com o gnu R
       * 
       * @param value
       * 
       * @return 
       */
      string NA(double value);

      string NA(const MapStrDbl& values);
};

#endif//__CONVERT_H__

