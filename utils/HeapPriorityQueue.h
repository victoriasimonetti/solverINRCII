#ifndef __HEAPPRIORITYQUEUE_H__
#define __HEAPPRIORITYQUEUE_H__


#include "PriorityQueue.h"
#include <queue>
#include <utility>
#include <iostream>
#include <assert.h>

using namespace std;

struct KeyValue { 
    int key;
    double value;
    unsigned long int ordem;

    KeyValue(int _key = 0, double _value = 0, unsigned long int _ordem = 0):
        key(_key), value(_value), ordem(_ordem) { 
       //empty
    }
    
    bool operator <(KeyValue& right) const { 
       if (right.value == value) { 
          return ordem <right.ordem;
       }
       return  value <right.value;    
    }
};

class HeapPriorityQueue: public PriorityQueue {

   public:
      HeapPriorityQueue(int max_size);
      virtual ~HeapPriorityQueue();

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

      int getLeft(int i); 
      int getRight(int i); 
      int getParent(int i); 
      void bubbleUp(int i); 
      void exchange(int i, int j); 
      void bubbleDown(int i);  
      void check(string log);
       
      KeyValue* data;
      int m_size;
      int* keyIndex;
      unsigned long int ordCount;

};

#endif // __HEAPPRIORITYQUEUE_H__
