#ifndef __DEBUGPRIORITYQUEUE_H__
#define __DEBUGPRIORITYQUEUE_H__


#include "HeapPriorityQueue.h"
#include "DummiePriorityQueue.h"
#include <queue>
#include <utility>
#include <iostream>
#include <assert.h>

using namespace std;

class DebugPriorityQueue: public PriorityQueue {

   public:
      DebugPriorityQueue(int max_size, bool showDebug = true);
      virtual ~DebugPriorityQueue();

      virtual int getMinAndDelete();
      virtual int getMin();
      virtual void insert(int key, double value);
      virtual void insertOrDecrease(int key, double value);
      virtual void decrease(int key, double value);
      virtual void remove(int key);
      virtual bool keyExists(int key);
      virtual void clear();
      virtual int size();

   private:
      HeapPriorityQueue m_heap;
      DummiePriorityQueue m_linear;
      bool m_showDebug;

};

#endif // __DEBUGPRIORITYQUEUE_H__
