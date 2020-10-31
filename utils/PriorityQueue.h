#ifndef __PRIORITYQUEUE_H__
#define __PRIORITYQUEUE_H__

class PriorityQueue {

   public:
      PriorityQueue(int maxSize);
      virtual ~PriorityQueue();

      virtual int getMinAndDelete()=0;
      virtual int getMin()=0;
      virtual void insert(int key, double value)=0;
      virtual void insertOrDecrease(int key, double value)=0;
      virtual void decrease(int key, double value)=0;
      virtual void remove(int key)=0;
      virtual bool keyExists(int key)=0;
      virtual void clear()=0;
      virtual int size()=0;
      bool isEmpty();

   protected:

      int m_maxSize;
      

};

#endif // __PRIORITYQUEUE_H__
