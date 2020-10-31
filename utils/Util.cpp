#include "Util.h"
#include "Convert.h"
#include <algorithm>

bool Util::fileExists(string file) {
   ifstream f(file);
   if (!f) {
     return false;
   }
   f.close();
   return true;
};

vector<string> Util::split(string s, char delim) {
    vector<string> items;
    stringstream ss(s);
    string item;
    while (getline(ss, item, delim)) {
       items.push_back(trim(item));
    }
    return items;
}

int Util::sum(const initializer_list<int>& numbers){
   int total = 0;
   for (int i: numbers) {
     total+= i;
   }
   return total; 
}

 
string Util::join (const initializer_list<int>& items, string delim){
   string s = "";
   int k = 0;
   for (int i: items) {
      k++;
      s+= Convert::toStr(i) ;
      if (k!=(int)items.size()) {
         s += delim; 
      }
   }
   return s;
}

string Util::csv (const initializer_list<int>& items){
   return join(items,",");
}

double Util::hamming(double* v1, double* v2, int size){
   double sum = 0;
   for (int i = 0; i < size; i++) {
      double delta = fabs(v1[i] - v2[i]);
      sum += delta;
   }    
   return sum;
}

string& Util::rtrim(string& s, const string& delimiters) {
    return s.erase(s.find_last_not_of(delimiters) + 1 );
}

string& Util::ltrim(string& s, const string& delimiters) {
    return s.erase(0, s.find_first_not_of(delimiters));
}

string& Util::trim(string& s, const string& delimiters) {
    return ltrim(rtrim(s,delimiters), delimiters);
}

vector<int> Util::inter(vector<int> &v1, vector<int> &v2) {
   vector<int> v3;

   sort(v1.begin(), v1.end());
   sort(v2.begin(), v2.end());

   set_intersection(v1.begin(),v1.end(),v2.begin(),v2.end(),back_inserter(v3));

   return v3;
}


bool Util::startswith(const string& s, const string& prefix){
   return (s.find(prefix) == 0);
}


