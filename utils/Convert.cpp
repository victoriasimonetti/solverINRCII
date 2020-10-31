#include "Convert.h"
#include <sstream>
#include <iomanip>

string Convert::toString(double value){
   stringstream ss;
   ss << value;
   return ss.str();
}

string Convert::toString(int value){
   stringstream ss;
   ss << value;
   return ss.str();
}

string Convert::toStr(long value){
   stringstream ss;
   ss << value;
   return ss.str();
}

string Convert::toStr(double value){
   return toString(value);
}

string Convert::toStr(int value){
   return toString(value);
}

string Convert::toStr(double value, int decimals){
   stringstream ss;
   ss.precision(decimals);
   ss.setf(ios::fixed);
   ss << value;
   return ss.str();
}

string Convert::NA(double value){
   if (value < -0.5) {
      return "NA"; 
   } else {
      return toStr(value);
   }
}

string Convert::NA(const MapStrDbl& values){
   string s;
   for (auto kv: values) {
      s += NA(kv.second) + " "; 
   } 
   return s;
}


