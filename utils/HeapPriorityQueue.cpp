#include "HeapPriorityQueue.h"
#include <iostream>

const int NOT_IN_HEAP = -1;

HeapPriorityQueue::HeapPriorityQueue(int max_key):
   PriorityQueue::PriorityQueue(max_key) { 
   keyIndex = new int[max_key+1];
   data = new KeyValue[max_key+1];

   clear();
   ordCount = 0;
}


HeapPriorityQueue::~HeapPriorityQueue() { 
   delete [] keyIndex;
   delete [] data;
}

int HeapPriorityQueue::getMinAndDelete(){
   int key = data[1].key;
   remove(key); 
   return key;
}

int HeapPriorityQueue::getLeft(int i) { 
   return 2*i; 
}

int HeapPriorityQueue::getRight(int i) { 
   return 2*i+1;
}

int HeapPriorityQueue::getParent(int i) { 
   return i/2;
}

void HeapPriorityQueue::bubbleUp(int i) { 
   while (i!=1 && data[i] < data[getParent(i)]) { 
      exchange(getParent(i),i);  
      i = getParent(i);
   }  
}

void HeapPriorityQueue::exchange(int i, int j) { 
   swap(keyIndex[data[i].key],keyIndex[data[j].key]);
   swap(data[i],data[j]);
}

void HeapPriorityQueue::bubbleDown(int i) { 
   while (1) { 
      int left = getLeft(i); 
      int right = getRight(i);
      int minChild = left; //Indice do menor
      if (right > m_size) { 
          if (left> m_size) { 
             return; 
          }  
      } else { 
          if (data[right] < data[left]) { 
             minChild = right;
          }  
      }
      if (data[minChild] < data[i]) { 
          exchange(minChild, i);
          i = minChild; 
      } else { 
          return;
      }
   }
}

int HeapPriorityQueue::getMin(){
   return data[1].key;
}

void HeapPriorityQueue::clear(){
   m_size = 0;
   fill_n(keyIndex,m_maxSize+1,NOT_IN_HEAP);
   ordCount = 0;
}

void HeapPriorityQueue::insert(int key, double value){
   assert(!keyExists(key));
   m_size++;
   data[m_size] = KeyValue(key,value,ordCount++);
   keyIndex[key] = m_size;
   assert(keyIndex[key]>-1);
   bubbleUp(m_size);
}

void HeapPriorityQueue::insertOrDecrease(int key, double value){ 
   if (keyExists(key)) { 
      decrease(key,value);
   } else { 
      insert(key,value);
   }
}

bool HeapPriorityQueue::keyExists(int key) { 
   return keyIndex[key] != NOT_IN_HEAP; 
}

void HeapPriorityQueue::decrease(int key, double value) { 
   int i = keyIndex[key];
   //TODO adicionar um teste genérico de debug em relação a inserção de valor maior
   data[i].value = value;
   bubbleUp(i);
}

void HeapPriorityQueue::check(string log) { 
   for (int i=1; i<=m_size; i++) { 
      int key = data[i].key; 
      if (keyIndex[key] != i) { 
          cout << "keyIndex[key]" << keyIndex[key] << endl;
          cout << "i: " << i << endl;
          cout << "Key: " << key << endl;
          cout << "Fonte: " << log << endl; 
          cout << "Size: " << m_size << endl; 
      }
      assert(keyIndex[key]==i);
   }
}

void HeapPriorityQueue::remove(int key) { 
   assert(keyExists(key));
   int i = keyIndex[key];
   exchange(i,m_size); 
   keyIndex[key] = NOT_IN_HEAP;
   m_size--;
   if (m_size > 0) { 
      bubbleDown(i);
   }
}

int HeapPriorityQueue::size(){
   return m_size;
}
