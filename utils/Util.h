#ifndef __UTIL_H__
#define __UTIL_H__
using namespace std;
#include <string>
#include <sstream>
#include <iostream>
#include <vector>
#include <tuple>
#include <fstream>
#include <math.h>
#include <map>

typedef map<string,double> MapStrDbl;
typedef map<string,int> MapStrInt;

namespace Util {

      /** 
       * Verifica se um arquivo existe
       * 
       * @param file
       * 
       * @return existe?
       */
      bool fileExists(string file);

      /** 
       * Separa uma string por um delimitador
       * 
       * @param &s
       * @param delim
       * 
       * @return vector<string>
       */
      vector<string> split(string s, char delim = ',');

      
      /** 
       * Soma os argumentos, simples assim.
       * 
       * @param numbers
       * 
       * @return int 
       */
      int sum (const initializer_list<int>& numbers);

      /** 
       * Concatena os elementos usando o delimitador
       * 
       * @param items
       * 
       * @return string 
       */
      string join (const initializer_list<int>& items, string delim = "");

      /** 
       * Concatena os elementos usando virgulas 
       * 
       * @param items
       * 
       * @return string 
       */
      string csv (const initializer_list<int>& items);


      /** 
       * Retorna a distância de Hamming entre 2 vetores de mesmo tamanho.
       *
       * @author Vick
       * 
       * @param v1 Vetor de double
       * @param v2 Vetor de double
       * @param size Tamanho dos vetores.
       * 
       * @return retorna a distância de hamming sempre positiva.
       */
      double hamming(double* v1, double* v2, int size);

      string& rtrim(string& s, const string& delimiters = " \f\n\r\t\v" );

      string& ltrim(string& s, const string& delimiters = " \f\n\r\t\v" ); 

      string& trim(string& s, const string& delimiters = " \f\n\r\t\v" );

      /** 
       * Retorna a intersecção dos vetores
       * 
       * @param &v1
       * @param &v2
       * 
       * @return vetores intersectados.
       */
      vector<int> inter(vector<int> &v1, vector<int> &v2);


      bool startswith(const string& s, const string& prefix);


      
};


#endif//__UTIL_H__

