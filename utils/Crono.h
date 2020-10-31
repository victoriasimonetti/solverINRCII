#ifndef __CRONO_H__
#define __CRONO_H__

#include <sys/time.h>
#include <string>
using namespace std;

/**
 * Cronometro
 **/  
class Crono {
   public:
      enum CronoState {   //Direções usadas no método move
	       STOPPED = 0,
	       RUNNING = 1,
      };
      /** 
       * Cria um cronometro parado
       */
      Crono(); //Cria um cronometro parado
      void resetAndRun(); //Reseta e roda;
      void reset(); //Para o cronometro e zera o tempo
      void run(); //Inicia ou continua um cronometro
      void stop(); //Pausa um cronometro
      double getTime() const; //Exibe o tempo total desde o inicio
      CronoState getState() const;  //retorna o estado
      string getTimeStr(bool showMs = true) const; //Retorna representação inteligível em uma string
      static string getTimeStr(double duration, bool showMs = true, bool padding = true); //Retorna representação inteligível em uma string
      static unsigned int getRandom(); //Retorna um número pseudo aleatório baseado no clock
   private:
      double m_elapsed; //tempo transcorrido
	   double m_start; // tempo inicial
      CronoState m_state; //Estado do cronometro      
};

#endif

