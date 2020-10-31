#include "../include/Solution.h"


#define PESOHARD 1000

Solution::Solution(Instance* i){

    this->flagUndo = false;
    this->inst = i;
    for(int k=0; k<inst->n_nurses; k++){
        this->schedule.push_back(vector<Fulfilled>());
        for(int i=0; i<7*inst->n_weeks; i++){ 
            Fulfilled nurse;
            nurse.shift = 0;
            nurse.skill = "";          
            this->schedule[k].push_back(nurse);   
        }
    }

    this->h_underStaffing = 0;
    this->h_shiftTypeSuccessions = 0; 

    this->s_optimalCoverage = 0; 
    this->s_consecutiveShift = 0; 
    this->s_consecutiveWorkingDays = 0; 
    this->s_consecutiveDaysOff = 0;
    this->s_preferences = 0;
    this->s_completeWeekend = 0;
    this->s_totalAssignments = 0; 
    this->s_totalWorkingWeekeends = 0;
    this->costSolution = 0;

	this->s_maxconsecutiveShift = 0; 
    this->s_maxconsecutiveWorkingDays = 0; 
    this->s_maxconsecutiveDaysOff = 0;
	this->s_minconsecutiveShift = 0; 
    this->s_minconsecutiveWorkingDays = 0; 
    this->s_minconsecutiveDaysOff = 0;

    //Variáveis auxiliares para cálculo incremental
    for(int k=0; k<inst->n_nurses; k++){       
        this->s_consecutiveShift_nurse.push_back(0);  
        this->s_consecutiveDaysOff_nurse.push_back(0);
        this->s_consecutiveWorkingDays_nurse.push_back(0);
        this->s_totalAssignments_nurse.push_back(0); 
        this->s_totalWorkedWeekend_nurse.push_back(0);                          
    }  

    //Varivel auxiliar para cálculo incremental da S1 e H2: número de enfermeiros por turno/habilidade/dia
    for(int t=0; t<inst->n_shifts; t++){
        s1_h1_incremental.push_back(vector<vector<int>>());   
        for(int h=0; h<inst->n_skills; h++){
            s1_h1_incremental[t].push_back(vector<int>());   
            for(int d=0; d<7*inst->n_weeks; d++){
                s1_h1_incremental[t][h].push_back(0);
            }
        }
    }

}

/*-----------------------------------------------------------------------------*/
void Solution::s1_h2_optimalMinStaffing(){

    int total = 0;

    this->s_optimalCoverage = 0; 
    this->h_underStaffing = 0;
    for(int t=0; t<inst->n_shifts; t++){
        for(int h=0; h<inst->n_skills; h++){
            for(int d=0; d<7*inst->n_weeks; d++){
                int opt = inst->optRequerementsWeek[t][h][d];
                int min = inst->minRequerementsWeek[t][h][d];
                total = 0;
                this->s1_h1_incremental[t][h][d] = 0;
                for(int n=0; n<inst->n_nurses; n++){
                    if(this->schedule[n][d].shift == t && inst->skillIndex[this->schedule[n][d].skill] == h){
                        this->s1_h1_incremental[t][h][d]++;
                        total++; 
                    }
                }

                if(opt > total && total >= min){ // se já tem penalização para o minimo, então n penaliza com o ótimo tb.
                    this->s_optimalCoverage += (opt-total);
                }
				if(min > total){
                    this->h_underStaffing += min-total;
                }         
            }
        }
    }

   // cout << "S1: " << this->s_optimalCoverage << endl;
   // cout << "H2: " << this->h_underStaffing << endl;
}


/*-----------------------------------------------------------------------------*/
void Solution::h3_shiftTypeSucessions(int n){

    int turnoHistory = inst->historyLastShiftWorked[n]; //ultimo turno trabalhado no historico.
    int turnoAtual;
    int turnoProx;
    for(int d=-1; d<(7*inst->n_weeks)-1; d++){
        if(d == -1){
            turnoAtual = turnoHistory;
        }else{
            turnoAtual = this->schedule[n][d].shift;
        } 
        turnoProx = this->schedule[n][d+1].shift;
        if(inst->forbiddenShift[turnoAtual][turnoProx] == 1){
            this->h_shiftTypeSuccessions++;
        }           
    }
}

/*-----------------------------------------------------------------------------*/
void Solution::s3_consecutiveDaysOff(int n){

    int atual = inst->historyTotalConsecutiveDaysOff[n];
	int maxDayOff = inst->maxDaysOff[ inst->contractIndex[inst->nurses[n].contract] ];
	int minDayOff = inst->minDaysOff[ inst->contractIndex[inst->nurses[n].contract] ];
	this->s_consecutiveDaysOff_nurse[n] = 0;

	if(inst->historyTotalConsecutiveDaysOff[n] > maxDayOff){
    	this->s_consecutiveDaysOff_nurse[n] -= inst->historyTotalConsecutiveDaysOff[n] - maxDayOff;
    } 

    for(int d=0; d<7*inst->n_weeks; d++){   
        if(this->schedule[n][d].shift == inst->n_shifts){ //se na escala estiver a folga, então soma
            atual++; 
        }else{ //se não estiver com folga, verifica quantos dias de folga deu e se passou do limite max e min.
            if(atual > maxDayOff){
                this->s_consecutiveDaysOff_nurse[n] += atual - maxDayOff; 
				this->s_maxconsecutiveDaysOff += atual - maxDayOff;
            }

            if(atual!=0 && atual < minDayOff ){
                this->s_consecutiveDaysOff_nurse[n] += minDayOff - atual; 
				this->s_minconsecutiveDaysOff += minDayOff - atual; 
            }
            atual = 0;
        }
    } 

    if(atual > maxDayOff){
         this->s_consecutiveDaysOff_nurse[n] += atual - maxDayOff; 
		 this->s_maxconsecutiveDaysOff += atual - maxDayOff;
    } 
	this->s_consecutiveDaysOff += this->s_consecutiveDaysOff_nurse[n];
    //cout << "S3: " << this->s_consecutiveDaysOff << endl;
}

/*-----------------------------------------------------------------------------*/
void Solution::s4_preferences(int n){

    for(int d=0; d < 7*inst->n_weeks; d++){
        for(int s=0; s<= inst->n_shifts; s++){ //Ex: itera apenas nos turnos, com o any
            if(inst->preferences[n][d][s] == 1){ //tem alguma preferencia
                if(s == inst->n_shifts &&  this->schedule[n][d].shift != inst->n_shifts){ //quer uma folga
                    this->s_preferences++;  
                }else if(s != inst->n_shifts && s == this->schedule[n][d].shift){ // n quer aquele turno
                    this->s_preferences++;
                }
            }
        }
    }    
}


/*-----------------------------------------------------------------------------*/
void Solution::s2a_consecutiveShift(int n){

    int consecShift = inst->historyLastShiftConsecutive[n]; //começa com oq ja tem no historico
    int lastShift = inst->historyLastShiftWorked[n];
	this->s_consecutiveShift_nurse[n] = 0;

	if(lastShift != inst->n_shifts && inst->historyLastShiftConsecutive[n] > inst->maxShiftConsecutive[lastShift]){
    	this->s_consecutiveShift_nurse[n] -= inst->historyLastShiftConsecutive[n] - inst->maxShiftConsecutive[lastShift];
	//cout << this->s_consecutiveShift_nurse[n] << " " << inst->historyLastShiftConsecutive[n] << " " << inst->maxShiftConsecutive[lastShift] << endl;
    } 

    for(int d=0; d<7*inst->n_weeks; d++){   
        if(this->schedule[n][d].shift == lastShift){ //se na escala estiver o mesmo turno q o anterior, então soma
            consecShift++; 
        }else{ //se nao for folga, verifica quantos dias se passou do limite max e min.
            if(lastShift != inst->n_shifts && consecShift > inst->maxShiftConsecutive[lastShift]){
                this->s_consecutiveShift_nurse[n] += consecShift - inst->maxShiftConsecutive[lastShift]; 
				this->s_maxconsecutiveShift += consecShift - inst->maxShiftConsecutive[lastShift]; 
            }

            if(lastShift != inst->n_shifts && consecShift!=0 && consecShift < inst->minShiftConsecutive[lastShift]){
                this->s_consecutiveShift_nurse[n] += inst->minShiftConsecutive[lastShift] - consecShift; 
				this->s_minconsecutiveShift +=  inst->minShiftConsecutive[lastShift] - consecShift; 
            }
            lastShift = this->schedule[n][d].shift; //atualiza o turno buscado para o ultimo encontrado
            consecShift = 1;
        }
    }

    //caso seja a ultima semana, tem que acrescentar aqueles ultimos turnos nao add no calculo.

    if(lastShift != inst->n_shifts && consecShift > inst->maxShiftConsecutive[lastShift]){
        this->s_consecutiveShift_nurse[n] += consecShift - inst->maxShiftConsecutive[lastShift]; 
		this->s_maxconsecutiveShift += consecShift - inst->maxShiftConsecutive[lastShift]; 
    }      
    this->s_consecutiveShift += this->s_consecutiveShift_nurse[n];
    //cout << "S2a: " << this->s_consecutiveShift << endl;
}
    
/*-----------------------------------------------------------------------------*/
void Solution::s2b_consecutiveWorkingDays(int n){
    
    int atual = inst->historyTotalConsecutiveDays[n]; //começa com oq ja tem no historico
	int maxDay =  inst->maxDaysConsecutiveWorked[ inst->contractIndex[inst->nurses[n].contract] ];
	int minDay =  inst->minDaysConsecutiveWorked[ inst->contractIndex[inst->nurses[n].contract] ];
	this->s_consecutiveWorkingDays_nurse[n] = 0;

	if(inst->historyTotalConsecutiveDays[n] > maxDay){
        this->s_consecutiveWorkingDays_nurse[n] -= inst->historyTotalConsecutiveDays[n] - maxDay;  
    } 

    for(int d=0; d<7*inst->n_weeks; d++){   
        if(this->schedule[n][d].shift != inst->n_shifts){ //se na escala for dia trabalhado, então soma
            atual++; 
        }else{ //se estiver a folga, verifica quantos dias de trabalho deu e se passou do limite max e min.
            if(atual > maxDay){
                this->s_consecutiveWorkingDays_nurse[n] += atual - maxDay ; 
				this->s_maxconsecutiveWorkingDays += atual - maxDay ; 
            }

            if(atual!=0 && atual < minDay){
            	this->s_consecutiveWorkingDays_nurse[n] += minDay - atual;
			    this->s_minconsecutiveWorkingDays += minDay - atual;
            }
            atual = 0;
        }
    }        

	/* Penaliza apenas o máximo pois não se sabe os dias trabalhados do próximo mês para penalizar o mínimo */
    if(atual > maxDay){
        this->s_consecutiveWorkingDays_nurse[n] += atual - maxDay;   
		this->s_maxconsecutiveWorkingDays += atual - maxDay;  
    }        
	this->s_consecutiveWorkingDays += this->s_consecutiveWorkingDays_nurse[n];
   //cout << "S2b: " << this->s_consecutiveWorkingDays << endl;
}

/*-----------------------------------------------------------------------------*/
void Solution::s5_completeWeekend(int n){

    int contrato = inst->contractIndex[inst->nurses[n].contract]; //numero do contrato do enfermeiro n
    if(inst->completeWeekendConstraint[contrato] == 1){ //esse enfermeiro possui restrição de fim de semana
		for(int w=0; w<inst->n_weeks; w++){
		    if(this->schedule[n][(w*7)+5].shift ==  inst->n_shifts && this->schedule[n][(w*7)+6].shift !=  inst->n_shifts ||
		        this->schedule[n][(w*7)+5].shift !=  inst->n_shifts && this->schedule[n][(w*7)+6].shift ==  inst->n_shifts ){
		        this->s_completeWeekend++;    
		    }
		}               
    }
}

/*-----------------------------------------------------------------------------*/
void Solution::s7_totalWorkingWeekends(int n){

	/* Esta variável indica quantos fins de semana o enfermeiro n trabalhou */
    this->s_totalWorkedWeekend_nurse[n] = 0;

	for(int w=0; w<inst->n_weeks; w++){
		if(this->schedule[n][(w*7)+5].shift != inst->n_shifts || this->schedule[n][(w*7)+6].shift !=  inst->n_shifts){
		    this->s_totalWorkedWeekend_nurse[n]++;
		}
	}
    
    int maxWorkingWeekendContract = inst->maxWorkingWeekends[ inst->contractIndex[inst->nurses[n].contract]];
    
    if( this->s_totalWorkedWeekend_nurse[n] > maxWorkingWeekendContract){
        this->s_totalWorkingWeekeends +=  this->s_totalWorkedWeekend_nurse[n] - maxWorkingWeekendContract;
    }  

}

/*-----------------------------------------------------------------------------*/
void Solution::s6_totalAssignments(int n){

    this->s_totalAssignments_nurse[n] = 0;
    for(int d=0; d<7*inst->n_weeks; d++){                 
        if(this->schedule[n][d].shift !=  inst->n_shifts){
            this->s_totalAssignments_nurse[n]++;
        } 
    }

    double minDayWorkedContract = inst->minDaysWorked[ inst->contractIndex[inst->nurses[n].contract]];
    double maxDayWorkedContract = inst->maxDaysWorked[ inst->contractIndex[inst->nurses[n].contract]];

    if(this->s_totalAssignments_nurse[n] > maxDayWorkedContract){
        this->s_totalAssignments  += (this->s_totalAssignments_nurse[n] - maxDayWorkedContract);
    }
    if(this->s_totalAssignments_nurse[n] < minDayWorkedContract){
        this->s_totalAssignments += (minDayWorkedContract - this->s_totalAssignments_nurse[n]);
    }      
}

/*-----------------------------------------------------------------------------*/
void Solution::forcaBruta(){

    this->s_consecutiveDaysOff = 0;
    this->s_consecutiveShift = 0;
    this->s_consecutiveWorkingDays = 0;   
    this->h_shiftTypeSuccessions = 0; 
    this->s_preferences = 0; 
    this->s_completeWeekend = 0;
    this->s_totalWorkingWeekeends = 0;
    this->s_totalAssignments = 0;

	this->s_maxconsecutiveShift = 0; 
    this->s_maxconsecutiveWorkingDays = 0; 
    this->s_maxconsecutiveDaysOff = 0;
	this->s_minconsecutiveShift = 0; 
    this->s_minconsecutiveWorkingDays = 0; 
    this->s_minconsecutiveDaysOff = 0;

    for(int n=0; n<inst->n_nurses; n++){  
        s3_consecutiveDaysOff(n);
        s2a_consecutiveShift(n);
        s2b_consecutiveWorkingDays(n);
        h3_shiftTypeSucessions(n);
        s4_preferences(n);
        s5_completeWeekend(n);
        s7_totalWorkingWeekends(n);
        s6_totalAssignments(n);
    }

    s1_h2_optimalMinStaffing();
   
    this->costSolution = this->s_totalAssignments*20 + this->s_consecutiveShift*15 + this->s_consecutiveDaysOff*30 + 
                            this->s_preferences*10 + this->s_consecutiveWorkingDays*30 + this->s_totalWorkingWeekeends*30 + 
                            this->s_completeWeekend*30 + this->s_optimalCoverage*30 + 
                            (this->h_underStaffing + this->h_shiftTypeSuccessions)*PESOHARD;
  
}



/*-----------------------------------------------------------------------------*/
void Solution::printFinalSolution(string cusOutFile){

    int total_workingweekend=0;
    int total_assignments=0;
    int consecutiveAssignments = 0;
    int consecutiveDaysOff = 0;
    int preferences = 0;
    int completeweekend = 0;
    int optimalcoverage = 0;

    ifstream infile(cusOutFile);

    infile >> total_workingweekend;
    infile >> total_assignments;
    infile >> consecutiveAssignments;
    infile >> consecutiveDaysOff;
    infile >> preferences;
    infile >> completeweekend;
    infile >> optimalcoverage;

    infile.close();

    cout << endl << "--------------- VALOR FINAL DA SOLUCAO: --------------------------------------" << endl << endl;
    cout << "Total assignment constraints (total days worked): " << total_assignments << endl;
    cout << "Consecutive constraints (consecutive shift/worked days): " <<  consecutiveAssignments <<" "<< this->s_delta_consecutiveShift<< endl;
    cout << "Non working days constraints (consecutive days off): " << consecutiveDaysOff << endl;
    cout << "Preferences: " << preferences << endl;
    cout << "Max working weekend (total worked weekend): " << total_workingweekend << endl;
    cout << "Complete weekends: " << completeweekend << endl;
    cout << "Optimal coverage constraints: " <<  optimalcoverage << endl;
    cout << "Total cost**: " << total_assignments + consecutiveAssignments + consecutiveDaysOff + preferences + 
                              total_workingweekend + completeweekend + optimalcoverage << endl;
  
}

/*-----------------------------------------------------------------------------*/
void Solution::geraSolutionFile(){
    
	for(int l=0; l<inst->n_weeks; l++){
	
		string solFile = "Solucao/" + inst->name +"_w" + to_string(l) + ".txt";
		ofstream outfile (solFile);
		outfile << "SOLUTION" << endl << l << " " << inst->infoFile << endl << endl;

		int totalAssignments = 0;
		for(int i=0; i<inst->n_nurses; i++){
		    for(int j=l*7; j<7*(l+1); j++){
		        if(this->schedule[i][j].shift != inst->n_shifts){ //se n for folga do enfermeiro, então soma
		            totalAssignments++;            
		        }
		    }
		}

		outfile << "ASSIGNMENTS = " << totalAssignments << endl;

		for(int i=0; i<inst->n_nurses; i++){
		    for(int j=l*7; j<7*(l+1); j++){
		        if(this->schedule[i][j].shift != inst->n_shifts){ //se n for folga do enfermeiro, então soma
		            outfile << inst->listNameNurse[i] << " " << inst->listNameDay[j%7] << " " << inst->listNameShift[this->schedule[i][j].shift] << 
		            " " << this->schedule[i][j].skill << endl;
		        }
		    }
		}
		outfile << endl;
		outfile.close();
	}	  
}

//----------------------------------- CALCULO INCREMENTAL -------------------------------------------------------
//***************************************************************************************************************
//***************************************************************************************************************
//---------------------------------------------------------------------------------------------------------------


void Solution::incr_s6_totalAssignments(int shiftAtual, int nurseMod, int shiftMod, int dayMod){

    double minWorked = inst->minDaysWorked[ inst->contractIndex[inst->nurses[nurseMod].contract]] ;
    double maxWorked = inst->maxDaysWorked[ inst->contractIndex[inst->nurses[nurseMod].contract]] ;

	// Retira as penalidade de antes de modificar
	if(this->s_totalAssignments_nurse[nurseMod] > maxWorked){
    	this->s_totalAssignments -= this->s_totalAssignments_nurse[nurseMod] - maxWorked;
    }
    if(this->s_totalAssignments_nurse[nurseMod] < minWorked){
        this->s_totalAssignments -= minWorked - this->s_totalAssignments_nurse[nurseMod];
    }      

	// Estava trabalhando e passou a folgar
	if(shiftAtual != inst->n_shifts && shiftMod == inst->n_shifts){
		this->s_totalAssignments_nurse[nurseMod]--;
	}  

	// Estava de folga e passou a trabalhar
	if(shiftAtual == inst->n_shifts && shiftMod != inst->n_shifts){
		this->s_totalAssignments_nurse[nurseMod]++;
	}
	
	if(this->s_totalAssignments_nurse[nurseMod] > maxWorked){
    	this->s_totalAssignments += this->s_totalAssignments_nurse[nurseMod] - maxWorked;
    }
    if(this->s_totalAssignments_nurse[nurseMod] < minWorked){
        this->s_totalAssignments += minWorked - this->s_totalAssignments_nurse[nurseMod];
    } 
}

//--------------------------------------------------------------------------------------------------------------------
void Solution::incr_s7_totalWorkingWeekends(int shiftAtual, int nurseMod, int shiftMod, int dayMod){

	int maxWorkingWeekend = inst->maxWorkingWeekends[ inst->contractIndex[inst->nurses[nurseMod].contract]];
	/* Dia modificado é um fim de semana */	

	if(dayMod % 7 == 5 || dayMod % 7 == 6){
		int flag = 1; // domingo
		if(dayMod % 7 == 6){ //dayMod==domingo
			flag = -1; //sábado
		}

		/* Se anteriomente existia uma penalização, então subtrai */
		if(this->s_totalWorkedWeekend_nurse[nurseMod] > maxWorkingWeekend){
			this->s_totalWorkingWeekeends -= (this->s_totalWorkedWeekend_nurse[nurseMod] - maxWorkingWeekend);
		}

		//Antes de modificar o sábado
		if(shiftAtual != inst->n_shifts || this->schedule[nurseMod][dayMod+flag].shift != inst->n_shifts){
		    this->s_totalWorkedWeekend_nurse[nurseMod]--;
		}

		//Depois de modificar
		if(shiftMod != inst->n_shifts || this->schedule[nurseMod][dayMod+flag].shift != inst->n_shifts){
		    this->s_totalWorkedWeekend_nurse[nurseMod]++;
		}
		
		if(this->s_totalWorkedWeekend_nurse[nurseMod] > maxWorkingWeekend){
		    this->s_totalWorkingWeekeends += (this->s_totalWorkedWeekend_nurse[nurseMod] - maxWorkingWeekend);
		}  
	}	
}


//--------------------------------------------------------------------------------------------------------------------
void Solution::incr_h3_shiftTypeSucessions(int shiftAtual, int nurseMod, int shiftMod, int dayMod){

    int turnoAnterior;
    int turnoPosterior;
    if(dayMod == 0){
        turnoAnterior = inst->historyLastShiftWorked[nurseMod]; //ultimo turno trabalhado no historico.
    }else{
        turnoAnterior = this->schedule[nurseMod][dayMod-1].shift;
    }

    //retirar violacoes com o dia anterior
    if(inst->forbiddenShift[turnoAnterior][shiftAtual] == 1){
        this->h_shiftTypeSuccessions--;
    }

    if(inst->forbiddenShift[turnoAnterior][shiftMod] == 1){
        this->h_shiftTypeSuccessions++;
    }
    
    if(dayMod < (inst->n_weeks*7)-1){ //retirar violacoes com o prox dia
        turnoPosterior = this->schedule[nurseMod][dayMod+1].shift;
        if(inst->forbiddenShift[shiftAtual][turnoPosterior] == 1){
            this->h_shiftTypeSuccessions--;
        } 

        if(inst->forbiddenShift[shiftMod][turnoPosterior] == 1){
            this->h_shiftTypeSuccessions++;
        }  
    }
}

//--------------------------------------------------------------------------------------------------------------------
void Solution::incr_s1_h2(int shiftAtual, int shiftMod, int dayMod, int skillAtual, int skillMod){

    int total;
	int optAtual, minAtual;
	int optMod, minMod;

   
		//cout << "entrei: " << shiftAtual << " " << shiftMod << " " << skillAtual << " " << skillMod << endl;
		if(shiftAtual != inst->n_shifts){
			optAtual = inst->optRequerementsWeek[shiftAtual][skillAtual][dayMod];
    		minAtual = inst->minRequerementsWeek[shiftAtual][skillAtual][dayMod];
		    total = this->s1_h1_incremental[shiftAtual][skillAtual][dayMod] ;

			// retira
		    if(optAtual > total && total >= minAtual){ // se já tem penalização para o minimo, então n penaliza com o ótimo tb.
		        this->s_optimalCoverage -= (optAtual-total);
		    } 
		    if(minAtual > total){
		        this->h_underStaffing -= minAtual - total;
		    }  

			this->s1_h1_incremental[shiftAtual][skillAtual][dayMod]--;
		    total = this->s1_h1_incremental[shiftAtual][skillAtual][dayMod];
			if(optAtual > total && total >= minAtual){ // se já tem penalização para o minimo, então n penaliza com o ótimo tb.
		        this->s_optimalCoverage += (optAtual-total);
		    } 
		    if(minAtual > total){
		        this->h_underStaffing += minAtual - total;
		    }
		}

		if(shiftMod != inst->n_shifts){
			optMod = inst->optRequerementsWeek[shiftMod][skillMod][dayMod];
    		minMod = inst->minRequerementsWeek[shiftMod][skillMod][dayMod];
		    total = this->s1_h1_incremental[shiftMod][skillMod][dayMod];

		    if(optMod > total && total >= minMod){ 
		        this->s_optimalCoverage -= (optMod-total);
		    } 
		    if(minMod > total){
		        this->h_underStaffing -= minMod - total ;
		    }  
			
			this->s1_h1_incremental[shiftMod][skillMod][dayMod]++;
			total = this->s1_h1_incremental[shiftMod][skillMod][dayMod];
		    if(optMod > total && total >= minMod){ 
		        this->s_optimalCoverage += (optMod-total);
		    } 
		    if(minMod > total){
		        this->h_underStaffing += minMod - total;
		    }  
		}
    
}

//--------------------------------------------------------------------------------------------------------------------
void Solution::incr_s3_consecutiveDaysOff(int shiftAtual, int nurseMod, int shiftMod, int dayMod){

	int n = nurseMod;
	int atual = inst->historyTotalConsecutiveDaysOff[n];
	int maxDayOff = inst->maxDaysOff[ inst->contractIndex[inst->nurses[n].contract] ];
	int minDayOff = inst->minDaysOff[ inst->contractIndex[inst->nurses[n].contract] ];
	this->s_consecutiveDaysOff -= this->s_consecutiveDaysOff_nurse[n];
	this->s_consecutiveDaysOff_nurse[n] = 0;

	if(inst->historyTotalConsecutiveDaysOff[n] > maxDayOff){
    	this->s_consecutiveDaysOff_nurse[n] -= inst->historyTotalConsecutiveDaysOff[n] - maxDayOff;
    } 


    for(int d=0; d<7*inst->n_weeks; d++){   
        if(this->schedule[n][d].shift == inst->n_shifts){ //se na escala estiver a folga, então soma
            atual++; 
        }else{ //se não estiver com folga, verifica quantos dias de folga deu e se passou do limite max e min.
            if(atual > maxDayOff){
                this->s_consecutiveDaysOff_nurse[n] += atual - maxDayOff; 
            }

            if(atual!=0 && atual < minDayOff ){
                this->s_consecutiveDaysOff_nurse[n] += minDayOff - atual;  
            }
            atual = 0;
        }
    } 

    if(atual > maxDayOff){
         this->s_consecutiveDaysOff_nurse[n] += atual - maxDayOff; 
    } 
	this->s_consecutiveDaysOff += this->s_consecutiveDaysOff_nurse[n];

	
}

//--------------------------------------------------------------------------------------------------------------------
void Solution::incr_s4_preferences(int shiftAtual, int nurseMod, int shiftMod, int dayMod){

    // Duas opções de criterios: OU
    // - Queria folga mas está trabalhando
    // - Não queria trabalhar nesse turno

    //retira a penalização anterior (se houver)
    if(inst->preferences[nurseMod][dayMod][inst->n_shifts] == 1 && shiftAtual != inst->n_shifts){ //queria folga mas está trabalhando
        this->s_preferences--;
    }
    if(inst->preferences[nurseMod][dayMod][shiftAtual] == 1 && shiftAtual != inst->n_shifts){ //Não queria trabalhar no turno específico mas está trabalhando
        this->s_preferences--;
    }

    //Adiciona nova penalização
    if(inst->preferences[nurseMod][dayMod][inst->n_shifts] == 1 && shiftMod != inst->n_shifts){ //queria folga mas está trabalhando
        this->s_preferences++;
    }
    if(inst->preferences[nurseMod][dayMod][shiftMod] == 1 && shiftMod != inst->n_shifts){ //Não queria trabalhar no turno específico mas está trabalhando
        this->s_preferences++;
    }
}

//--------------------------------------------------------------------------------------------------------------------
void Solution::incr_s2a_consecutiveShift(int shiftAtual, int nurseMod, int shiftMod, int dayMod){

	int n = nurseMod;
    int consecShift = inst->historyLastShiftConsecutive[n]; //começa com oq ja tem no historico
    int lastShift = inst->historyLastShiftWorked[n];
    this->s_consecutiveShift -= this->s_consecutiveShift_nurse[n];
	this->s_consecutiveShift_nurse[n] = 0;

	if(inst->historyLastShiftConsecutive[n] > inst->maxShiftConsecutive[lastShift]){
    	this->s_consecutiveShift_nurse[n] -= inst->historyLastShiftConsecutive[n] - inst->maxShiftConsecutive[lastShift];
    }

    for(int d=0; d<7*inst->n_weeks; d++){   
        if(this->schedule[n][d].shift == lastShift){ //se na escala estiver o mesmo turno q o anterior, então soma
            consecShift++; 
        }else{ //se nao for folga, verifica quantos dias se passou do limite max e min.
            if(lastShift != inst->n_shifts && consecShift > inst->maxShiftConsecutive[lastShift]){
                this->s_consecutiveShift_nurse[n] += consecShift - inst->maxShiftConsecutive[lastShift]; 
            }

            if(lastShift != inst->n_shifts && consecShift!=0 && consecShift < inst->minShiftConsecutive[lastShift]){
                this->s_consecutiveShift_nurse[n] += inst->minShiftConsecutive[lastShift] - consecShift; 
            }
            lastShift = this->schedule[n][d].shift; //atualiza o turno buscado para o ultimo encontrado
            consecShift = 1;
        }
    }

    //caso seja a ultima semana, tem que acrescentar aqueles ultimos turnos nao add no calculo.

    if(lastShift != inst->n_shifts && consecShift > inst->maxShiftConsecutive[lastShift]){
        this->s_consecutiveShift_nurse[n] += consecShift - inst->maxShiftConsecutive[lastShift]; 
    }      
    this->s_consecutiveShift += this->s_consecutiveShift_nurse[n];
             
}

//--------------------------------------------------------------------------------------------------------------------
void Solution::incr_s2b_consecutiveWorkingDays(int shiftAtual, int nurseMod, int shiftMod, int dayMod){

	int n = nurseMod;
	int atual = inst->historyTotalConsecutiveDays[n]; //começa com oq ja tem no historico
	int maxDay =  inst->maxDaysConsecutiveWorked[ inst->contractIndex[inst->nurses[n].contract] ];
	int minDay =  inst->minDaysConsecutiveWorked[ inst->contractIndex[inst->nurses[n].contract] ];
	this->s_consecutiveWorkingDays -= this->s_consecutiveWorkingDays_nurse[n];
	this->s_consecutiveWorkingDays_nurse[n] = 0;

	if(inst->historyTotalConsecutiveDays[n] > maxDay){
        this->s_consecutiveWorkingDays_nurse[n] -= inst->historyTotalConsecutiveDays[n] - maxDay;  
    }

    for(int d=0; d<7*inst->n_weeks; d++){   
        if(this->schedule[n][d].shift != inst->n_shifts){ //se na escala for dia trabalhado, então soma
            atual++; 
        }else{ //se estiver a folga, verifica quantos dias de trabalho deu e se passou do limite max e min.
            if(atual > maxDay){
                this->s_consecutiveWorkingDays_nurse[n] += atual - maxDay ; 
            }

            if(atual!=0 && atual < minDay){
               this->s_consecutiveWorkingDays_nurse[n] += minDay - atual;
            }
            atual = 0;
        }
    }        

	/* Penaliza apenas o máximo pois não se sabe os dias trabalhados do próximo mês para penalizar o mínimo */
    if(atual > maxDay){
        this->s_consecutiveWorkingDays_nurse[n] += atual - maxDay ;           
    }        
	this->s_consecutiveWorkingDays += this->s_consecutiveWorkingDays_nurse[n];
   //cout << "S2b: " << this->s_consecutiveWorkingDays << endl;

}
    
//--------------------------------------------------------------------------------------------------------------------
void Solution::incr_s5_completeWeekend(int shiftAtual, int nurseMod, int shiftMod, int dayMod){

    int contrato = inst->contractIndex[inst->nurses[nurseMod].contract]; //numero do contrato do enfermeiro n
    if(inst->completeWeekendConstraint[contrato] == 1 && (dayMod % 7 == 5 || dayMod % 7 == 6)){ //restrição de fim de semana  

        if(dayMod % 7 == 5){
            if((shiftAtual == inst->n_shifts && this->schedule[nurseMod][dayMod+1].shift !=  inst->n_shifts) || 
                (shiftAtual != inst->n_shifts && this->schedule[nurseMod][dayMod+1].shift ==  inst->n_shifts)){
                 this->s_completeWeekend--;  
            }
           
            if((shiftMod == inst->n_shifts && this->schedule[nurseMod][dayMod+1].shift !=  inst->n_shifts) ||
                (shiftMod != inst->n_shifts && this->schedule[nurseMod][dayMod+1].shift ==  inst->n_shifts)){
                 this->s_completeWeekend++;  
            }
           
        }else if(dayMod % 7 == 6){
            if((this->schedule[nurseMod][dayMod-1].shift !=  inst->n_shifts && shiftAtual ==  inst->n_shifts) ||
                (this->schedule[nurseMod][dayMod-1].shift ==  inst->n_shifts && shiftAtual !=  inst->n_shifts)){
               this->s_completeWeekend--; 
            }

            if((this->schedule[nurseMod][dayMod-1].shift !=  inst->n_shifts && shiftMod ==  inst->n_shifts) || 
                (this->schedule[nurseMod][dayMod-1].shift ==  inst->n_shifts && shiftMod !=  inst->n_shifts)){
               this->s_completeWeekend++; 
            }
        } 
    }   
}

//--------------------------------------------------------------------------------------------------------------------
void Solution::printSolution(){

	for(int n=0; n<inst->n_nurses; n++){
		cout << "Enf " << n << ": ";
		for(int d=0; d<7*inst->n_weeks; d++){
			cout << schedule[n][d].shift << " ";
		}
		cout << endl;
	}
}


//--------------------------------------------------------------------------------------------------------------------
void Solution::undoLastMove() {
   flagUndo = true;
   while (!listChanges.empty()) {
      Change c = listChanges.top();
      calcIncremental(c.shift,  c.nurse, c.oldShift, c.day, c.skill, c.oldSkill); 
      listChanges.pop();
   }
   flagUndo = false;
}

//--------------------------------------------------------------------------------------------------------------------
void Solution::clearChanges() {
   while (!listChanges.empty()) {
      listChanges.pop();
   }
}

//--------------------------------------------------------------------------------------------------------------------
void Solution::calcIncremental(int shiftAtual, int nurseMod, int shiftMod, int dayMod){
    int skill = inst->skillIndex[schedule[nurseMod][dayMod].skill];
    calcIncremental(shiftAtual, nurseMod, shiftMod, dayMod, skill, skill);
}

//--------------------------------------------------------------------------------------------------------------------
void Solution::calcIncremental(int shiftAtual, int nurseMod, int shiftMod, int dayMod, int skill, int skillMod){ 

    //Gravando modificação para poder desfazer depois
    if (flagUndo == false) {
       Change change;
       change.nurse = nurseMod;
       change.shift = shiftMod;
       change.day = dayMod;
       change.oldShift = shiftAtual;
       change.skill = skillMod;
       change.oldSkill = skill;
       listChanges.push(change);
    }

	//mod skill
    schedule[nurseMod][dayMod].skill =  inst->listNameSkill[skillMod];
	schedule[nurseMod][dayMod].shift = shiftMod; 
 
    if(shiftAtual != shiftMod){  
        incr_h3_shiftTypeSucessions(shiftAtual, nurseMod, shiftMod, dayMod); //ok
        incr_s4_preferences(shiftAtual, nurseMod, shiftMod, dayMod); //ok
        incr_s5_completeWeekend(shiftAtual, nurseMod, shiftMod, dayMod); //ok

		incr_s7_totalWorkingWeekends(shiftAtual, nurseMod, shiftMod, dayMod); //ok
        incr_s6_totalAssignments(shiftAtual, nurseMod, shiftMod, dayMod); //ok

        incr_s3_consecutiveDaysOff(shiftAtual, nurseMod, shiftMod, dayMod);
        incr_s2a_consecutiveShift(shiftAtual, nurseMod, shiftMod, dayMod);
        incr_s2b_consecutiveWorkingDays(shiftAtual, nurseMod, shiftMod, dayMod);
 
    }

    incr_s1_h2(shiftAtual, shiftMod, dayMod, skill, skillMod); //ok
	//s1_h2_optimalMinStaffing();
   
    this->costSolution = this->s_totalAssignments * 20 + this->s_consecutiveShift*15 + this->s_consecutiveDaysOff*30 + 
                            this->s_preferences*10 + this->s_consecutiveWorkingDays*30 + this->s_totalWorkingWeekeends*30 + 
                            this->s_completeWeekend*30 + this->s_optimalCoverage*30 + 
                            (this->h_underStaffing + this->h_shiftTypeSuccessions)*PESOHARD;

  
}

void Solution::readSolverSolution(double* sol, double obj, ZenVar& x) {

    for(int n=0; n<inst->n_nurses; n++){
		for(int d=0; d<inst->n_weeks*7; d++){
			for(int s=0; s<=inst->n_shifts; s++){
				for(int k=0; k<inst->n_skills; k++){
					if(sol[x(n+1,d+1,s+1,k+1)] >= 0.5){
						 schedule[n][d].shift = s;
	                     schedule[n][d].skill = inst->listNameSkill[k];
					} 
				}
			}							
		}	
	}
    forcaBruta();
}

void Solution::verifySolution(){

	ifstream infile;
	infile.open ("x.log");

	string um, dois;
	while (!infile.eof()){

		//x(34,3,1,4)
		infile >> um >> dois;

		int a=0;
		vector <string> aux(4);
		for(int i=2; i<um.size(); i++){
			if(um[i] == ',' || um[i]==')'){
				a=a+1;
			}else{
				aux[a] += (um[i]);
			}		
		}

		int enf, dia, turno, hab;
		enf = stoi(aux[0])-1;
		dia = stoi(aux[1])-1;
		turno = stoi(aux[2])-1;
		hab = stoi(aux[3])-1;
	
		schedule[enf][dia].shift = turno;      
		schedule[enf][dia].skill = inst->listNameSkill[hab];
    }
	infile.close();

}

//--------------------------------------------------------------------------------------------------------------------
void Solution::print() {
   cout << "H2 understaffing: " << this->h_underStaffing << endl << 
           "H3 shiftTypeSuccessions: " << this->h_shiftTypeSuccessions <<  endl << 
           "S1 optimalCoverage: " << this->s_optimalCoverage*30 << endl << 

           "S2a consecutiveShift: " << this->s_consecutiveShift*15 << " = (" << this->s_minconsecutiveShift << "," << this->s_maxconsecutiveShift << " )" << endl <<
           "S2b consecutiveWorkingDays: " << this->s_consecutiveWorkingDays*30 << " = ( " << this->s_minconsecutiveWorkingDays << "," << this->s_maxconsecutiveWorkingDays << " )" << endl <<
           "S3 consecutiveDaysOff: " << this->s_consecutiveDaysOff*30 << " = ( " << this->s_minconsecutiveDaysOff << "," << this->s_maxconsecutiveDaysOff << " )" << endl <<
           "S4 preferences: " << this->s_preferences*10 <<  endl << 
           "S5 completeWeekend: " << this->s_completeWeekend*30 << endl << 
           "S6 totalAssignments: " << this->s_totalAssignments*20 << endl << 
           "S7 totalWorkingWeekeends: " << this->s_totalWorkingWeekeends*30 << endl <<
           " " << endl << endl << "Cost Solution: " << this->costSolution << endl << endl;

}

bool Solution::isFeasible(){

	if(this->h_underStaffing + this->h_shiftTypeSuccessions == 0){
		return true;
	}
	return false;
}

void Solution::validator(){

	char comando[400];
	sprintf(comando, "java -jar validator.jar --sce %s --his %s --weeks %s --sols  Solucao/%s_w?.txt", inst->scenarioFile.c_str(),inst->historyFile.c_str(),inst->weekFile.c_str(), inst->name.c_str());

	system(comando);

}




















