#ifndef _NEIGHBORHOOD_H
#define _NEIGHBORHOOD_H	

#include <iostream>
#include "../include/Instance.h"
#include "../include/Solution.h"
#include <time.h>
#include <random>

class Neighborhood {
protected:	
	Instance* inst;
	
	
public:
	Neighborhood(Instance* inst);
	Solution trocaShifts(Solution s);
    Solution trocaSkill(Solution s);
    void trocaShiftsRand(Solution &s);
    void trocaSkillRand(Solution &s);
    void swapShiftRand(Solution &s);
    void giveOffRand(Solution &s);
    void swapRand(Solution &s);
    void swapRandBlock(Solution &s);
	void weekendComplete(Solution &s);
	void swapGiveOffgiveShiftRand(Solution &s);
	void swapTwoBlock(Solution &s);
	void multiSwap(Solution &s);
	void change(Solution &s);
	void doubleChange(Solution &s);


};


#endif

