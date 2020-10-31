#ifndef __ZEN_VAR__
#define __ZEN_VAR__

#include "../utils/Crono.h"
#include "../utils/Convert.h"
#include "../utils/Util.h"
#include <tr1/unordered_map>

using namespace std;

class ZenSolver;
typedef tr1::unordered_map<string,int> ZenVarMap;

class ZenVar {

   public:

      ZenVar(){};
      inline int operator()(int i1) const {return get({i1});};
      inline int operator()(int i1, int i2) const {return get({i1,i2});};
      inline int operator()(int i1, int i2, int i3) const {return get({i1,i2,i3});};
      inline int operator()(int i1, int i2, int i3, int i4) const {return get({i1,i2,i3,i4});};
      inline int operator()(int i1, int i2, int i3, int i4, int i5) const {return get({i1,i2,i3,i4,i5});};
      inline int operator()(int i1, int i2, int i3, int i4, int i5, int i6) const {return get({i1,i2,i3,i4,i5,i6});};

   private:
      string m_name;
      ZenVarMap* m_varMap;

      ZenVar(string name, ZenVarMap* varMap):
         m_name(name),
         m_varMap(varMap){
         //empty
      };

      int get(const initializer_list<int>& indexes) const {
         string key = m_name + "(" + Util::csv(indexes) + ")";
         int size = (int) m_varMap->size();
         int index = (*m_varMap)[key];
         if (size != (int) m_varMap->size()) {
            cout << "ZenVar " << key << " não existe." << endl;
            exit(1); 
         }
         return index;
      }; 

      friend class ZenSolver;
};

#endif //__ZEN_VAR__
