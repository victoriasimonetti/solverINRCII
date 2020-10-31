#include "DummiePriorityQueue.h"
#include <algorithm>
#include <iostream>

DummiePriorityQueue::DummiePriorityQueue(int max_key):  
   PriorityQueue::PriorityQueue(max_key) { 

   //empty
}


DummiePriorityQueue::~DummiePriorityQueue() { 
   //empty
}

int DummiePriorityQueue::getMinAndDelete(){
   list< pair<double,int> >::iterator minIt = min_element(m_pq.begin(),m_pq.end());
   int key = minIt->second;
   m_pq.erase(minIt);
   return key;
}


void DummiePriorityQueue::remove(int key) { 
   list< pair<double,int> >::iterator it = find(key);
   if (it!=m_pq.end()) { 
      m_pq.erase(it);
   }
}

void DummiePriorityQueue::clear() { 
   m_pq.clear();
}

bool DummiePriorityQueue::keyExists(int key) { 
   return find(key)!=m_pq.end();
}

int DummiePriorityQueue::getMin(){
   list< pair<double,int> >::iterator minIt = min_element(m_pq.begin(),m_pq.end());
   return minIt->second;
}

void DummiePriorityQueue::insert(int key, double value){
   m_pq.push_back(pair<double,int>(value,key));
}

void DummiePriorityQueue::insertOrDecrease(int key, double value){
   list< pair<double,int> >::iterator it = find(key);
   if (it==m_pq.end()) { 
      insert(key,value);
   } else {
      it->first = value;
   }
}

void DummiePriorityQueue::decrease(int key, double value){
   list< pair<double,int> >::iterator it = find(key);
   it->first = value; 
}

list< pair<double,int> >::iterator DummiePriorityQueue::find(int key) {
   list< pair<double,int> >::iterator it;
   for (it = m_pq.begin(); it!= m_pq.end(); it++) { 
      if (it->second == key) {
         return it;
      }
   }
   return m_pq.end();
}

int DummiePriorityQueue::size(){
   return (int)m_pq.size();
}
