#ifndef __DummiePriorityQueue_H__
#define __DummiePriorityQueue_H__


#include "PriorityQueue.h"
#include <list>
#include <utility>

using namespace std;

class DummiePriorityQueue: public PriorityQueue {

   public:
      DummiePriorityQueue(int max_key);
      virtual ~DummiePriorityQueue();

      virtual int getMinAndDelete(); //n
      virtual int getMin(); //n
      virtual void insert(int key, double value);//1
      virtual void remove(int key);//1
      virtual void insertOrDecrease(int key, double value);//n
      virtual void decrease(int key, double value);//n
      virtual bool keyExists(int key);//n
      virtual void clear();
      virtual int size();

   private:

      list< pair<double,int> >::iterator find(int key);
      list< pair<double,int> > m_pq;
       

};

#endif // __DummiePriorityQueue_H__
