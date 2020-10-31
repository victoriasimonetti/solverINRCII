#include "Crono.h"
#include <sstream>
#include <iomanip>
/**
* Compatibilidade para sistemas operacionais Windows e Linux
**/
#ifdef WIN32
   #include <windows.h>
   #define GET_TIME(x) x=((double)clock()/CLOCKS_PER_SEC);
#else
   #include <sys/time.h>
   #include <unistd.h>
   #define GET_TIME(x) struct timeval t_time; gettimeofday(&t_time, NULL); x=(double)(t_time.tv_sec)+(double)(t_time.tv_usec)/1000000.0;
#endif
Crono::Crono() {
   reset();
}

void
Crono::reset() {
   m_elapsed = 0.0;
	m_state = STOPPED;
}

void
Crono::run() {
   if (m_state == STOPPED) {
      GET_TIME(m_start);
      m_state = RUNNING;
   }
}

void
Crono::resetAndRun() {
   reset();
   run();
}

void
Crono::stop() {
   double t_now;
   if (m_state == RUNNING) {
      GET_TIME(t_now);
      m_elapsed += t_now - m_start;
      m_state = STOPPED;
   }
}

double
Crono::getTime() const {
   double t_now;
   if (m_state == STOPPED) {
      return m_elapsed;
   } else {
      GET_TIME(t_now);
      return m_elapsed + t_now - m_start;
   }
}

Crono::CronoState
Crono::getState() const {
   return m_state;
}

unsigned int
Crono::getRandom() {
#ifdef WIN32
   return GetTickCount();
#else
   struct timeval now;
   gettimeofday(&now, 0);      
   return (unsigned int)now.tv_usec;  
#endif
}

string
Crono::getTimeStr(bool showMs /*false*/) const {
   return getTimeStr(getTime(),showMs,true);
}

string
Crono::getTimeStr(double t, bool showMs /*false*/, bool padding) {
   int zzz = (int)((t-(int)t)*1000);
   int ss = (int)t%60;
   int mm = ((int)t/60)%60;   
   int hh = ((int)t/3600);     
   stringstream s;
   s.setf(ios::right);
   s.fill('0');  

   if (padding || hh>0) {
      s << setw(2) << hh << ":";
   }
   if (padding || mm+hh>0) {
      s << setw(2) << mm << ":";
   }
   if (padding || ss+mm+hh>0) {
      s << setw(2) << ss;
   }
   if (showMs) {
      s << "." << setw(3) << zzz;  
   }
   return s.str();
}

