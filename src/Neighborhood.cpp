#include "../include/Neighborhood.h"

extern mt19937 gen;

Neighborhood::Neighborhood(Instance* inst){
    this->inst = inst;
}


Solution Neighborhood::trocaShifts(Solution s){

    int bestCost = s.costSolution;
    int bestNurse = 0;
    int bestDay = 0;
    int bestShift = 0;

    for(int n=0; n< this->inst->n_nurses; n++){
        for(int d=0; d<7*inst->n_weeks; d++){
            int turnoAntes = s.schedule[n][d].shift;

            for(int t=0; t <= this->inst->n_shifts; t++){  
    
				int turnoMod = t;
                s.calcIncremental(turnoAntes, n, turnoMod, d);             
                //s.schedule[n][d].shift = t;
                //s.forcaBruta();
                
                if(s.costSolution < bestCost){
                    bestCost = s.costSolution;
                    bestNurse = n;
                    bestDay = d;
                    bestShift = t;
                }
				s.calcIncremental(turnoMod, n, turnoAntes, d);               
            }
        }
    }
    s.calcIncremental(s.schedule[bestNurse][bestDay].shift, bestNurse, bestShift, bestDay);
    //s.schedule[bestNurse][bestDay].shift = bestShift; 
    //s.forcaBruta();
    return s;        
}

// --------------------------------------------------------------------------------------
Solution Neighborhood::trocaSkill(Solution s){

    int bestCost = s.costSolution;
    int bestNurse = 0;
    int bestDay = 0;
    int bestSkill = 0;

    for(int n=0; n< this->inst->n_nurses; n++){
        for(int d=0; d<7*inst->n_weeks; d++){
            int skillAntes = inst->skillIndex[s.schedule[n][d].skill]; //enfermeiro n no dia d com a skill: nurse
			int shift = s.schedule[n][d].shift;

            for(int skill=0; skill < inst->nurses[n].skill_s.size(); skill++){ //itera em todas as possibilidades de skill enf             
                //s.schedule[n][d].skill = inst->nurses[n].skill_s[skill];
                //s.forcaBruta();

        		int skillMod = inst->skillIndex[inst->nurses[n].skill_s[skill]];
				s.calcIncremental(shift, n, shift, d, skillAntes, skillMod);		
              
                if(s.costSolution < bestCost){
                    bestCost = s.costSolution;
                    bestNurse = n;
                    bestDay = d;
                    bestSkill = skillMod;
                }				
				s.calcIncremental(shift, n, shift, d, skillMod, skillAntes);	
            }             
        }
    }
    //s.schedule[bestNurse][bestDay].skill =  inst->nurses[bestNurse].skill_s[bestSkill]; 
	int shift = s.schedule[bestNurse][bestDay].shift;
	int skillAntes = inst->skillIndex[s.schedule[bestNurse][bestDay].skill];
	s.calcIncremental(shift, bestNurse, shift, bestDay, skillAntes, bestSkill);
    //s.forcaBruta();
    return s; 
}

// ALEATORIO --------------------------------------------------------------------------------------

// M1
void Neighborhood::trocaShiftsRand(Solution &s){

    std::uniform_int_distribution<> dis1(0, inst->n_nurses-1);
    std::uniform_int_distribution<> dis2(0, (7*inst->n_weeks)-1);
    std::uniform_int_distribution<> dis3(0, inst->n_shifts);
       
    int nurse = dis1(gen);
    int day = dis2(gen);
    int shift = dis3(gen);

    while( s.schedule[nurse][day].shift == shift){
        shift = dis3(gen);
    }

	int shiftAtual = s.schedule[nurse][day].shift;
	s.calcIncremental(shiftAtual, nurse, shift, day);
}

// M3
void Neighborhood::trocaSkillRand(Solution &s){

    std::uniform_int_distribution<> dis1(0, inst->n_nurses-1);
    int nurse = dis1(gen);
    std::uniform_int_distribution<> dis2(0, (7*inst->n_weeks)-1);
    int day = dis2(gen);
    std::uniform_int_distribution<> dis3(0, inst->nurses[nurse].skill_s.size()-1);
    int skill = dis3(gen);


    if(inst->nurses[nurse].skill_s.size() > 1){
        while(s.schedule[nurse][day].skill == inst->nurses[nurse].skill_s[skill]){
            skill = dis3(gen);
        }

        int skillAntes = inst->skillIndex[s.schedule[nurse][day].skill];
        int skillMod = inst->skillIndex[inst->nurses[nurse].skill_s[skill]];
        s.calcIncremental(s.schedule[nurse][day].shift, nurse, s.schedule[nurse][day].shift, day, skillAntes, skillMod);		
    }
}

// Atribuição de uma folga
//M4
void Neighborhood::giveOffRand(Solution &s){

    std::uniform_int_distribution<> dis1(0, inst->n_nurses-1); //Seleciona enfermeiro
    std::uniform_int_distribution<> dis2(0, (7*inst->n_weeks)-1); //Seleciona dia

    int nurse = dis1(gen);
    int day = dis2(gen);

    for(int i=0; i<3; i++){ //gambi, 3 tentativas  
        if(s.schedule[nurse][day].shift == inst->n_shifts){
            nurse = dis1(gen);
        }else{
            break;
        }
    }
    s.calcIncremental(s.schedule[nurse][day].shift, nurse, inst->n_shifts, day);      
}

// Vizinhança que troca os turnos/bloco de turno entre dois enfermeiros do mesmo dia : APENAS TURNOS, SKILL NAO
// M2
void Neighborhood::swapRandBlock(Solution &s){

    std::uniform_int_distribution<> dis4(1, inst->n_weeks*7); //seleciona tamanho do bloco
    int block = dis4(gen);

    std::uniform_int_distribution<> dis1(0, inst->n_nurses-1); // seleciona enfermeiro 1
    int nurse1 = dis1(gen);

    std::uniform_int_distribution<> dis2(0, (7*inst->n_weeks)-block); // seleciona dia do inicio do bloco
    int day = dis2(gen); // seleciona dia que começa, valor entre [0,6] sempre

    std::uniform_int_distribution<> dis3(0, inst->n_nurses-1); // seleciona enfermeiro 2
    int nurse2 = dis3(gen);

    while(nurse1 == nurse2){
        nurse2 = dis3(gen);
    }

    for(int d=day; d < day+block; d++){
        int aux_shift1 = s.schedule[nurse1][d].shift;
        s.calcIncremental(s.schedule[nurse1][d].shift, nurse1, s.schedule[nurse2][d].shift, d);   
        s.calcIncremental(s.schedule[nurse2][d].shift, nurse2, aux_shift1, d);      
    }   
}

// Vizinhança que verifica se um enfermeiro aleatorio possui apenas um dos dois dias trabalhados no fim de semana
// caso isso ocorra, existe 50% de chance que ele ou tenha folga todo o fim de semana, ou trabalhe todo o fim de semana
// M5
void Neighborhood::weekendComplete(Solution &s){

    std::uniform_int_distribution<> dis1(0, inst->n_nurses-1);
    std::uniform_int_distribution<> dis2(0, 1);
    std::uniform_int_distribution<> dis3(0, inst->n_shifts-1); //escluir folga para garantir que vai trabalhar
    int nurse = dis1(gen); //seleciona um enfermeiro
	int escolha = dis2(gen); //0=folga, 1=trabalhar

	int sab = 5;
	int dom = 6;
	int count = 0;

	if(s.schedule[nurse][sab].shift != inst->n_shifts){ count++; } 
	if(s.schedule[nurse][dom].shift != inst->n_shifts){ count++; }

	if(count == 0 || (count ==1 && escolha ==1)){//poe p trabalhar
		int turnoSab = dis3(gen); //seleciona um turno de trabalho, caso seja preciso
		int turnoDom = dis3(gen); 
		s.calcIncremental(s.schedule[nurse][sab].shift, nurse, turnoSab, sab);
		s.calcIncremental(s.schedule[nurse][dom].shift, nurse, turnoDom, dom);      

	//dois dias trabalhando || um dia trabalhando e a escolha é do fim de semana com folga
	}else{ //poe folga
		s.calcIncremental(s.schedule[nurse][sab].shift, nurse, inst->n_shifts, sab);
		s.calcIncremental(s.schedule[nurse][dom].shift, nurse, inst->n_shifts, dom); 	
	}

}

// Vizinhança que troca dois turno de dois enfermeiros aleatorios no mesmo dia. Porém, um enfermeiro deve estar trabalhando, e o
// outro deve estar de folga, assim dando folga para o que estava trabalhando e trabalho para o que estava de folga.
// M6
void Neighborhood::swapGiveOffgiveShiftRand(Solution &s){

    std::uniform_int_distribution<> dis1(0, 7*inst->n_weeks-1);
    int day = dis1(gen);

	vector<int> enfFolga;
	vector<int> enfTrab;

	for(int n=0; n < inst->n_nurses; n++){
		if(s.schedule[n][day].shift ==  inst->n_shifts){
			enfFolga.push_back(n);
		}else{
			enfTrab.push_back(n);
		}
	}

    if (enfFolga.size() > 0 && enfTrab.size() > 0){
		std::uniform_int_distribution<> folga(0, enfFolga.size()-1);	
		std::uniform_int_distribution<> trab(0, enfTrab.size()-1);	

		int nurse1 = enfFolga[folga(gen)];
		int nurse2 = enfTrab[trab(gen)];

		s.calcIncremental(s.schedule[nurse1][day].shift, nurse1, s.schedule[nurse2][day].shift, day);   
		s.calcIncremental(s.schedule[nurse2][day].shift, nurse2, inst->n_shifts, day);  
	} 
}

// Vizinhança que troca os dois blocos da semana (sem o fim de semana) de um mesmo enfermeiro. ex: M M | T N N -> T N N | M M
// M7
void Neighborhood::swapTwoBlock(Solution &s){

    std::uniform_int_distribution<> dis1(0, 3); //Apenas os dias da semana, menos o ultimo para poder cortar depois do bloco
    int block = dis1(gen);
	//vai cortar depois do bloco, ex: block = 1:  M M | T N N -> T N N | M M

    std::uniform_int_distribution<> dis2(0, inst->n_nurses-1);
    int nurse = dis2(gen);

	std::uniform_int_distribution<> dis3(0, inst->n_weeks-1);
	int w = dis3(gen);
	w = w*7;

	vector<int> shift;
	// Guarda os valores de turno antes de modificar
	for(int d=w; d<=4+w; d++){
		shift.push_back(s.schedule[nurse][d].shift);
	}

	int d=w;
    for(int i=block+1; i<=4; i++){
		s.calcIncremental(s.schedule[nurse][d].shift, nurse, shift[i], d); 
		d++;  
    }   
	for(int i=0; i<=block; i++){
		s.calcIncremental(s.schedule[nurse][d].shift, nurse, shift[i], d); 
		d++;  
    }
}

// -----------------------------------------------------------------------------------------------------
// VIZINHANÇAS DO ARTIGO CESCHIA


void Neighborhood::multiSwap(Solution &s){


	std::uniform_int_distribution<> dis1(0, inst->n_nurses-1);
    int nurse1 = dis1(gen); //enf1
    int nurse2 = dis1(gen); //enf2

    while(nurse1 == nurse2){
        nurse2 = dis1(gen);
    }
	
	std::uniform_int_distribution<> dis2(0, 23); // tamanho max de k
	int k = dis2(gen);

    std::uniform_int_distribution<> dis3(0, (7*inst->n_weeks-1)-k); //quando começa a sequencia k de dias
	int day = dis3(gen);

	//verificar se são compatíveis antes de modificar efetivamente. Também está verificando quando é folga...
	bool compatible = false;
	while(compatible == false){
		compatible = true;
		//cout << "estou aqui " ;
		for(int i=day; i<day+k; i++){
			// se os dois enf nao tiverem folga no dia i, entao pode comparar as skills
			if(s.schedule[nurse1][i].shift != inst->n_shifts && s.schedule[nurse2][i].shift != inst->n_shifts){
				int skillEnf1 = inst->skillIndex[s.schedule[nurse1][i].skill];
				int skillEnf2 = inst->skillIndex[s.schedule[nurse2][i].skill];
			
				if(skillEnf1 != skillEnf2){ // se forem diferentes, seleciona novos enfermeiros
					nurse1 = dis1(gen); //enf1
					nurse2 = dis1(gen); //enf2
					while(nurse1 == nurse2){
						nurse2 = dis1(gen);
					}
					compatible = false;
					break;
				}
			}
		}
	}

	// troca efetivamente os turnos
	for(int d=day; d < day+k; d++){
		    int aux_shift1 = s.schedule[nurse1][d].shift;
		    s.calcIncremental(s.schedule[nurse1][d].shift, nurse1, s.schedule[nurse2][d].shift, d);   
		    s.calcIncremental(s.schedule[nurse2][d].shift, nurse2, aux_shift1, d);      
    }   

}

void Neighborhood::change(Solution &s){

	std::uniform_int_distribution<> dis1(0, inst->n_nurses-1);
	int nurse = dis1(gen);
	std::uniform_int_distribution<> dis2(0, (7*inst->n_weeks)-1);
	int day = dis2(gen);
	std::uniform_int_distribution<> dis3(0, inst->n_shifts);
	int shift = dis3(gen);
	std::uniform_int_distribution<> dis4(0, inst->nurses[nurse].skill_s.size()-1);
	int skill = dis4(gen);
		  

	// força modificar turno e/ou habilidade
	while(s.schedule[nurse][day].shift == shift && s.schedule[nurse][day].skill == inst->nurses[nurse].skill_s[skill]){
		shift = dis3(gen);
		skill = dis4(gen);
	}
		
	int skillAntes = inst->skillIndex[s.schedule[nurse][day].skill];
	int skillMod = inst->skillIndex[inst->nurses[nurse].skill_s[skill]];
	s.calcIncremental(s.schedule[nurse][day].shift, nurse, shift, day, skillAntes, skillMod);		

}

void Neighborhood::doubleChange(Solution &s){

	std::uniform_int_distribution<> dis1(0, inst->n_nurses-1);
	int nurse = dis1(gen);
	std::uniform_int_distribution<> dis2(0, (7*inst->n_weeks)-2); // dois dias seguidos
	int day = dis2(gen);
	std::uniform_int_distribution<> dis3(0, inst->n_shifts);
	int shift = dis3(gen);
	std::uniform_int_distribution<> dis4(0, inst->nurses[nurse].skill_s.size()-1);
	int skill = dis4(gen);
		  
	for(int i=0; i<2; i++){
		day = day + i;
		shift = dis3(gen);
		skill = dis4(gen);

		// força modificar turno e/ou habilidade
		while(s.schedule[nurse][day].shift == shift && s.schedule[nurse][day].skill == inst->nurses[nurse].skill_s[skill]){
			shift = dis3(gen);
			skill = dis4(gen);
		}
		
		int skillAntes = inst->skillIndex[s.schedule[nurse][day].skill];
		int skillMod = inst->skillIndex[inst->nurses[nurse].skill_s[skill]];
		s.calcIncremental(s.schedule[nurse][day].shift, nurse, shift, day, skillAntes, skillMod);	
	}	

}













