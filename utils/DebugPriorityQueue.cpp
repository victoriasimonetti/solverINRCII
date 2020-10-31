#include "DebugPriorityQueue.h"
#include <iostream>

DebugPriorityQueue::DebugPriorityQueue(int max_key, bool showDebug):
   PriorityQueue::PriorityQueue(max_key),
   m_heap(max_key),m_linear(max_key),m_showDebug(showDebug){ 

   //empty
}


DebugPriorityQueue::~DebugPriorityQueue() { 
   //empty
}

int DebugPriorityQueue::getMinAndDelete(){
   int key1 = m_linear.getMinAndDelete();
   int key2 = m_heap.getMinAndDelete();
   if (m_showDebug) cout << "linear.getMinAndDelete(); //" << key1 << endl;
   if (m_showDebug) cout << "intest.getMinAndDelete(); //" << key2 << endl;
   assert(key1==key2);
   return key1;
}

int DebugPriorityQueue::getMin(){
   int key1 = m_linear.getMin();
   int key2 = m_heap.getMin();
   if (m_showDebug) cout << "linear.getMin(); //" << key1 << endl;
   if (m_showDebug) cout << "intest.getMin(); //" << key2 << endl;
   assert(key1==key2);
   return key1;
}

void DebugPriorityQueue::clear(){
   if (m_showDebug) cout << "linear.clear();" << endl;
   if (m_showDebug) cout << "intest.clear();" << endl;
   m_linear.clear();
   m_heap.clear();
}

void DebugPriorityQueue::insert(int key, double value){
   m_linear.insert(key,value);
   m_heap.insert(key,value);
   if (m_showDebug) cout << "linear.insert(" << key << ", " << value << ");" << endl;
   if (m_showDebug) cout << "intest.insert(" << key << ", " << value << ");" << endl;
}

void DebugPriorityQueue::insertOrDecrease(int key, double value){ 
   m_linear.insertOrDecrease(key,value);
   m_heap.insertOrDecrease(key,value);
   if (m_showDebug) cout << "linear.insertOrDecrease(" << key << ", " << value << ");" << endl;
   if (m_showDebug) cout << "intest.insertOrDecrease(" << key << ", " << value << ");" << endl;
}

bool DebugPriorityQueue::keyExists(int key) { 
   bool e1 = m_linear.keyExists(key);
   bool e2 = m_heap.keyExists(key);
   if (m_showDebug) cout << "linear.keyExists(" << key << "); //" << e1 << endl;
   if (m_showDebug) cout << "intest.keyExists(" << key << "); //" << e2 << endl;
   assert(e1==e2);
   return e1; 
}

void DebugPriorityQueue::decrease(int key, double value) { 
   m_linear.decrease(key,value);
   m_heap.decrease(key,value);
   if (m_showDebug) cout << "linear.decrease(" << key << ", " << value << ");" << endl;
   if (m_showDebug) cout << "intest.decrease(" << key << ", " << value << ");" << endl;
}

void DebugPriorityQueue::remove(int key) { 
   m_linear.remove(key);
   m_heap.remove(key);
   if (m_showDebug) cout << "linear.remove(" << key << ");" << endl;
   if (m_showDebug) cout << "intest.remove(" << key << ");" << endl;
}

int DebugPriorityQueue::size(){
   int s1 = m_linear.size();
   int s2 = m_heap.size();
   if (m_showDebug) cout << "linear.size(); //" << s1 << endl;
   if (m_showDebug) cout << "intest.size(); //" << s2 << endl;
   assert(s1 == s2);
   return s1;
}
