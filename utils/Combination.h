#include <vector> 
#include <iostream> 
#include <stdexcept>

using namespace std;

/**
   Classe para gerar combinações.

Exemplo de uso:

   Combination c(10,3);
   while (c.next()) {
      for (int i: c.combination()) {
         cout << i << " " << endl;
      }
      cout << endl;
   }

*/
class Combination {

   public:
      /** 
       * Cria o gerador de combinações  
       * 
       * @param nn Número de elementos do conjunto
       * @param kk Número de elementos da combinação
       */
       Combination(int nn, int kk) {
          random = false;
          if (kk>nn) {
             throw runtime_error("Invalid Combination with k>n");
          }
          start(nn,kk);
       };


       void fillRandom() {
          for (unsigned int i=0; i<c.size(); i++ ){
             r[i]=elements[c[i]];
          }
       }

       void shuffle() {
          random = true;
          elements.resize(n);
          for (int i=0; i < n; i++) {
             elements[i]=i;
          }
          random_shuffle(elements.begin(),elements.end());
          /*cout << "SHUFFLE " << n << "," << k << "  [";
          for (int i=0; i < n; i++) {
             cout << elements[i] << " ";
          }
          cout << endl;*/
       }


       /** 
        * Elementos da combinação atual.
        * 
        * @return vector<int>
        */
       const vector<int>& combination() const {
          if (!random) {
             return c;
          } else {
             return r;
          }
       };


       /** 
        * Gera a próxima combinação.
        * 
        * @return Verdadeiro se a combinação foi gerada.
        */
       bool next() {
          if (it==0) {
             c.resize(k);
             if (random) r.resize(k);
             c.reserve(k+3);
             for (int i=1; i <= k; i++) { 
                c[i-1] = i-1;
                c[k] = n;
                c[k+1] = -1;
                j = k;
             }
             if (random) fillRandom(); 
          } else {
             if (k==n) {
                return false; 
             }
             iter();
             if (random) fillRandom(); 
             if (c.empty()) {
               return false;
             } 
          }
          it++;
          return true;
       };


       /** 
        * Re/Inicia o gerador de combinações 
        * 
        * @param nn
        * @param kk
        */
       void start(int nn, int kk) {
          it = 0;
          n = nn;
          k = kk;
          j = 1;
          c.clear();
          computeTotal();
       }

       /** 
        * Imprime a combinação atual.
        */
       void print() {
          for (int i:c) {
              cout << i << " ";
          }
          cout << endl;
       };

       unsigned long long count() {
          return m_count;
       }

       bool wasShuffled() {
          return random;
       }


   private:
      int j, k, n, x;
      int it;
      bool random;
      unsigned long long m_count;
      vector<int> c;
      vector<int> r; //shuffled vector
      vector<int> elements;
      
      /** 
       * Gera outra combinação
       */
      void iter() {
          if (j > 0) {
             x = j+1;
             goto incr;
          }
          if (c[0] + 1 < c[1]) {
             c[0] += 1;
             return;
          }
          j = 2;

          do_more:
             c[j-2] = j-2;
             x = c[j-1]+2;
             if (x == c[j]+1) {
                j++;
                goto do_more;
             }
             if (j > k) {
                c.clear();
                return;
             } 

          incr:
             c[j-1] = x-1;
             j--;
      };

      friend ostream& operator << (ostream& out, const Combination& data) { 
         for (int i: data.combination()) {
            out << i << " ";
         }
         return out;
      };

      /** 
       * Calcula o número total de combinações
       */
      void computeTotal() {
          m_count = 1;
          unsigned long long m = (unsigned long long) n;
          for (unsigned long long d = 1; d <= (unsigned long long)k; ++d) {
             m_count *= m--;
             m_count /= d;
          }
      }
};

