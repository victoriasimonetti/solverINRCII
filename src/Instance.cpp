#include "../include/Instance.h"

Instance::Instance(string instancia){

	name = instancia;
	stringstream ss(instancia);
	vector<string> result;

	while( ss.good() ){
		string substr;
		getline( ss, substr, '-' );
		result.push_back( substr );
	}

	this->infoFile = result[0];
	this->scenarioFile = "hidden-TXT/Sc-" + result[0] + ".txt";
	this->historyFile = "hidden-TXT/H0-" + result[0] + "-" + result[1] + ".txt";
	for(int i=2; i<result.size(); i++){
		this->weekFile += ("hidden-TXT/WD-" +  result[0] + "-" + result[i] + ".txt" + " "); // lista de semanas
	}

	cout << this->scenarioFile << endl;
	cout << this->historyFile << endl;
	cout << this->weekFile << endl;
}

void Instance::readFiles(){

	/* Lendo arquivo de scenario */
    ifstream readScenarioFile;

    readScenarioFile.open (this->scenarioFile, ifstream::in);
    if (!readScenarioFile.is_open()) {
        cout << "Erro 1: ao abrir arquivo de Scenario." << endl;
        exit(0);    
    }

    string word;
    string nada;
    char nadinha;

    readScenarioFile >> word >> nada >> nada ; //nome do scenario
    readScenarioFile >> word >> nada >> this->n_weeks ; //número de weeks
    readScenarioFile >> word >> nada >> this->n_skills ; //número de skill

    for(int i=0; i<this->n_skills; i++){ //Lendo as skills e colocando no map.
        readScenarioFile >> word;
        this->skillIndex[word] = i;
        this->listNameSkill.push_back(word);
    }

    readScenarioFile >> word >> nada >> this->n_shifts ; //shift types
    for(int i=0; i<this->n_shifts; i++){ //Lendo os shifts e colocando no map.
        int minConsecutive, maxConsecutive;
        readScenarioFile >> word;
        this->shiftIndex[word] = i;
        this->listNameShift.push_back(word);
        readScenarioFile >> nadinha >> minConsecutive >> nadinha >> maxConsecutive >> nadinha;
        this->minShiftConsecutive.push_back(minConsecutive);
        this->maxShiftConsecutive.push_back(maxConsecutive);       
    }
    
    //Ex: 0 = late; 1=night; 2=Early; 3=any
    this->shiftIndex["Any"] = this->n_shifts; //FOLGA É O ULTIMO TURNO: ultimo turno adicionado é a folga.
    this->listNameShift.push_back("Any");

    //Inicializa matriz
    for(int i=0; i<this->n_shifts + 1; i++){
        this->forbiddenShift.push_back(vector<int>());
        for(int j=0; j<this->n_shifts + 1; j++){
            this->forbiddenShift[i].push_back(0);       
        }
    }

    int violacoes;
    string word2;
    readScenarioFile >> word; //FORBIDDEN_SHIFT_TYPES_SUCCESSIONS

    for(int i=0; i<n_shifts; i++){
        readScenarioFile >> word >> violacoes;
        for(int j=0; j<violacoes; j++){
            readScenarioFile >> word2;
            this->forbiddenShift[this->shiftIndex[word]][this->shiftIndex[word2]] = 1;  
        }  
    }

    //CONTRACTS
    readScenarioFile >> word >> nada >> this->n_contracts;
    for(int i=0; i<this->n_contracts; i++){
        int maxC, minC;
        readScenarioFile >> word >> nadinha >> minC >> nadinha >> maxC >> nadinha;

        this->contractIndex[word] = i;
        minDaysWorked.push_back(minC);
        maxDaysWorked.push_back(maxC);        
        readScenarioFile >> nadinha >> minC >> nadinha >> maxC >> nadinha;

        minDaysConsecutiveWorked.push_back(minC);   
        maxDaysConsecutiveWorked.push_back(maxC);
        readScenarioFile >> nadinha >> minC >> nadinha >> maxC >> nadinha;

        minDaysOff.push_back(minC);
        maxDaysOff.push_back(maxC);
        readScenarioFile >> minC >> maxC;

        maxWorkingWeekends.push_back(minC);   
        completeWeekendConstraint.push_back(maxC);                                     
    }

	//Número de enfermeiros e características de cada um: tipo de contrato e skill
    readScenarioFile >> word >> nada >> this->n_nurses; 
    int nC;
    for(int i=0; i<this->n_nurses; i++){
        Nurse n;
        readScenarioFile >> word >> word2 >> nC; 

        nurseIndex[word] = i;
        this->listNameNurse.push_back(word);
        n.name = word;
        n.contract = word2;

        for(int i=0; i<this->n_skills; i++){
            n.skill_i.push_back(0);
        }

        for(int j=0; j<nC; j++){
            readScenarioFile >> word;
            n.skill_s.push_back(word);
            n.skill_i[skillIndex[word]] = 1;    
        }
       
        nurses.push_back(n);
    }

    readScenarioFile.close();

	/******** Lendo arquivos das weeks */

	// Armazenando a lista dos nomes dos arquivos das semanas em um vector
	string s = string(this->weekFile);
	vector<string> listWeeks;
	istringstream iss(s);
	for(string s; iss >> s; ){
		listWeeks.push_back(s);
	}

	//Inicializa matriz tridimensional de minimo e ótimo requerido com zeros: turno x habilidade x dia (total das semanas)
    for(int k=0; k<this->n_shifts; k++){
        minRequerementsWeek.push_back(vector<vector<int> >());
        optRequerementsWeek.push_back(vector<vector<int> >());
        for(int i=0; i<this->n_skills; i++){
            minRequerementsWeek[k].push_back(vector<int>());
            optRequerementsWeek[k].push_back(vector<int>());    
            for(int j=0; j<7*listWeeks.size(); j++){
                minRequerementsWeek[k][i].push_back(-1);
                optRequerementsWeek[k][i].push_back(-1);   
            }
        }
    }

	//Inicializa matriz de preferencia dos enfermeiros:  enfermeiro x dia x turno (com folga).
    for(int n=0; n<this->n_nurses; n++){
        this->preferences.push_back(vector< vector <int> > ());
        for(int d=0; d<7*listWeeks.size(); d++){
            this->preferences[n].push_back(vector <int>  ());
            for(int s=0; s<= this->n_shifts; s++){
                this->preferences[n][d].push_back(0);
            }
        }
    }

	dayIndex["Mon"] = 0; //segunda
    this->listNameDay.push_back("Mon");
    dayIndex["Tue"] = 1;
    this->listNameDay.push_back("Tue");
    dayIndex["Wed"] = 2;
    this->listNameDay.push_back("Wed");
    dayIndex["Thu"] = 3;
    this->listNameDay.push_back("Thu");
    dayIndex["Fri"] = 4;
    this->listNameDay.push_back("Fri");
    dayIndex["Sat"] = 5;
    this->listNameDay.push_back("Sat");
    dayIndex["Sun"] = 6; //domingo
    this->listNameDay.push_back("Sun");

	/* AQUI começa a leitura dos arquivos para cada semana */

	if (this->n_weeks != listWeeks.size()){
		cout << "Número de semanas do scenario e número de arquivos de semanas passados no parâmetros não confere." << endl;
		exit(0);
	}
	for(int l=0; l<listWeeks.size(); l++){
		readScenarioFile.open (listWeeks[l], ifstream::in);
		if (!readScenarioFile.is_open()) {
		    cout << "Erro ao abrir arquivo de Week: " << listWeeks[l] << endl;
		    exit(0);    
		}

		readScenarioFile >> word >> word2 >> word; //WEEK_DATA, n005w4, REQUIREMENTS

	    //Preenche o mínimo e ótimo requerido: turno x habilidade x dia - com Early HeadNurse (0,0) (0,0) (0,0) (0,0) (0,0) (0,0) (0,1)
		int nM, nO;
		for(int i=0; i<(this->n_skills*this->n_shifts); i++){
			readScenarioFile >> word >> word2;
			for(int k=7*l; k<7*(l+1); k++){
			    readScenarioFile >> nadinha >> nM >> nadinha >> nO >> nadinha;
			    minRequerementsWeek[this->shiftIndex[word]] [this->skillIndex[word2]][k] = nM;
			    optRequerementsWeek[this->shiftIndex[word]] [this->skillIndex[word2]][k] = nO;
			} 
		}
		
		// Preenche a matriz de preferência (Stefaan Any Tue): enfermeiro x dia x turno
		// 0: Sem preferencia do enfermeiro naquele dia, caso exista preferencia entao == 1,
		//    lembrar que o ultimo turno é a folga, ou seja, seria uma preferencia por folga.
		readScenarioFile >> word >> word2 >> nC;
		string word3;
		for(int i=0; i<nC; i++){
		    readScenarioFile >> word >> word2 >> word3;       
		    this->preferences[ this->nurseIndex[word] ][this->dayIndex[word3]+(7*l)][ this->shiftIndex[word2]] = 1;         
		}    
		readScenarioFile.close();
	}


	/***** Lendo arquivo de histórico*/
	
    readScenarioFile.open (this->historyFile, ifstream::in);
    if (!readScenarioFile.is_open()) {
        cout << "Erro ao abrir arquivo de Histórico." << endl;
        exit(0);    
    }

    readScenarioFile >> word >> this->atualWeek >> word2 >> word; //HISTORY 0 n005w4 NURSE_HISTORY

    for(int i=0; i<this->n_nurses; i++){
        historyTotalDaysWorked.push_back(0); // Numero total de dias trabalhados
        historyTotalWorkedWeekends.push_back(0); //numero total de fins de semana trabalhando
        historyLastShiftWorked.push_back(0); //Qual foi o ultimo turno trabalhado
        historyLastShiftConsecutive.push_back(0); //Quantos dias consecutivos que trabalhou no turno do ultimo turno.
        historyTotalConsecutiveDays.push_back(0); //Total de dias consecutivos trabalhados
        historyTotalConsecutiveDaysOff.push_back(0); //Total de dias consecutivos de folga.    
    }

    int i1, i2, i3, i4, i5; 
    for(int i=0; i<this->n_nurses; i++){
        readScenarioFile >> word >> i1 >> i2 >> word2 >> i3 >> i4 >> i5;     
        historyTotalDaysWorked[this->nurseIndex[word]] = i1; 
        historyTotalWorkedWeekends[this->nurseIndex[word]] = i2;
        if(word2 == "None"){
            word2 = "Any";
        } 
        historyLastShiftWorked[this->nurseIndex[word]] = this->shiftIndex[word2]; 
        historyLastShiftConsecutive[this->nurseIndex[word]] = i3; 
        historyTotalConsecutiveDays[this->nurseIndex[word]] = i4; 
        historyTotalConsecutiveDaysOff[this->nurseIndex[word]] = i5; 
    }

    readScenarioFile.close();

}

void Instance::dataSolver(string datafile){

	ofstream outfile (datafile);

	outfile << "data;" << endl;
	outfile << "set N := ";
	for(int i=0; i<n_nurses; i++){
		outfile << i+1 << " ";
	}
	outfile << ";" << endl << endl;
	
	outfile << "set D := ";
	for(int i=0; i<7*n_weeks; i++){
		outfile << i+1 << " ";
	}
	outfile << ";" << endl << endl;
	
	outfile << "set S := ";
	for(int i=0; i<n_shifts+1; i++){
		outfile << i+1 << " ";
	}
	outfile << ";" << endl << endl;

	outfile << "set SD := ";
	for(int i=0; i<n_shifts; i++){
		outfile << i+1 << " ";
	}
	outfile << ";" << endl << endl;

	outfile << "set K := ";
	for(int i=0; i<n_skills; i++){
		outfile << i+1 << " ";
	}
	outfile << ";" << endl << endl;

	outfile << "set W := " << endl;
	for(int i=0; i<n_weeks; i++){
		outfile << (i*7)+6 << " " << (i*7)+7 << endl;
	}
	outfile << ";" << endl << endl;
	
	outfile << "set F := " << endl;
	for(int i=0; i<n_shifts; i++){
		for(int j=0; j<n_shifts; j++){
			if(forbiddenShift[i][j]==1){
				outfile << i+1 << " " << j+1 << endl;
			}
		}
	}
	outfile << ";" << endl << endl;

	outfile << "set B := " << endl;
	for(int i=0; i<n_weeks*7; i++){
		for(int j=0; j<n_weeks*7; j++){
			if(i<=j){
				outfile << i+1 << " " << j+1 << endl;
			}
		}
	}
	outfile << ";" << endl << endl;
	
	outfile << "param q := " << endl;
	for(int i=0; i<n_nurses; i++){
		for(int j=0; j<n_skills; j++){
			outfile << i+1 << " " << j+1  << " " << nurses[i].skill_i[j] << endl;					
		}
	}
	outfile << ";" << endl << endl;

	outfile << "param ps := " << endl;
	for(int i=0; i<n_nurses; i++){
		for(int j=0; j<n_weeks*7; j++){
			for(int s=0; s<n_shifts; s++){ // sem a folga
				outfile << i+1 << " " << j+1  << " " << s+1 << " " << preferences[i][j][s] << endl;							
			}
		}
	}
	outfile << ";" << endl << endl;

	outfile << "param pd := " << endl;
	for(int i=0; i<n_nurses; i++){
		for(int j=0; j<n_weeks*7; j++){
			outfile << i+1 << " " << j+1 << " " << preferences[i][j][n_shifts] << endl;				
		}
	}
	outfile << ";" << endl << endl;

	outfile << "param Vl := " << endl;
	for(int i=0; i<n_weeks*7; i++){
		for(int j=0; j<n_shifts; j++){
			for(int k=0; k<n_skills; k++){
				outfile << i+1 << " " << j+1 << " " << k+1 << " " << minRequerementsWeek[j][k][i] << endl;	
			}		
		}
	}
	outfile << ";" << endl << endl;

	outfile << "param Vo := " << endl;
	for(int i=0; i<n_weeks*7; i++){
		for(int j=0; j<n_shifts; j++){
			for(int k=0; k<n_skills; k++){
				outfile << i+1 << " " << j+1 << " " << k+1 << " " << optRequerementsWeek[j][k][i] << endl;	
			}		
		}
	}
	outfile << ";" << endl << endl;

	outfile << "param Lgd := " << endl;
	for(int i=0; i<n_nurses; i++){
		outfile << i+1 << " " <<  maxDaysConsecutiveWorked[ contractIndex[nurses[i].contract] ] << endl;
	}
	outfile << ";" << endl << endl;
	
	outfile << "param Lld := " << endl;
	for(int i=0; i<n_nurses; i++){
		outfile << i+1 << " " <<  minDaysConsecutiveWorked[ contractIndex[nurses[i].contract] ] << endl;
	}
	outfile << ";" << endl << endl;

	outfile << "param Lgs := " << endl;
	for(int i=0; i<n_nurses; i++){
		for(int j=0; j<n_shifts+1; j++){
			if(j == n_shifts){
				outfile << i+1 << " " << j+1 << " " <<  maxDaysOff[ contractIndex[nurses[i].contract] ] << endl;
			}else{
				outfile << i+1 << " " << j+1 << " " <<  maxShiftConsecutive[j] << endl;
			}
		}
	}
	outfile << ";" << endl << endl;

	outfile << "param Lls := " << endl;
	for(int i=0; i<n_nurses; i++){
		for(int j=0; j<n_shifts+1; j++){
			if(j == n_shifts){
				outfile << i+1 << " " << j+1 << " " <<  minDaysOff[ contractIndex[nurses[i].contract] ] << endl;
			}else{
				outfile << i+1 << " " << j+1 << " " <<  minShiftConsecutive[j] << endl;
			}
		}
	}
	outfile << ";" << endl << endl;


	outfile << "param V := " << endl;
	for(int i=0; i<n_nurses; i++){
		outfile << i+1 << " " <<  completeWeekendConstraint[ contractIndex[nurses[i].contract] ] << endl;
	}
	outfile << ";" << endl << endl;

	outfile << "param Qg := " << endl;
	for(int i=0; i<n_nurses; i++){
		outfile << i+1 << " " <<  maxDaysWorked[ contractIndex[nurses[i].contract] ] << endl;
	}
	outfile << ";" << endl << endl;
	
	outfile << "param Ql := " << endl;
	for(int i=0; i<n_nurses; i++){
		outfile << i+1 << " " <<  minDaysWorked[ contractIndex[nurses[i].contract] ] << endl;
	}
	outfile << ";" << endl << endl;

	outfile << "param Rg := " << endl;
	for(int i=0; i<n_nurses; i++){
		outfile << i+1 << " " <<  maxWorkingWeekends[ contractIndex[nurses[i].contract] ] << endl;
	}
	outfile << ";" << endl << endl;

	outfile << "param hu := " << endl;
	for(int i=0; i<n_nurses; i++){
		outfile << i+1 << " " <<  historyLastShiftWorked[i]+1 << endl;
	}
	outfile << ";" << endl << endl;

	outfile << "param hd := " << endl;
	for(int i=0; i<n_nurses; i++){
		outfile << i+1 << " " <<  historyTotalConsecutiveDays[i] << endl;
	}
	outfile << ";" << endl << endl;


// incluir a consecutividade da folga
	outfile << "param hs := " << endl;
	for(int i=0; i<n_nurses; i++){
		for(int j=0; j<n_shifts+1; j++){
			if(j == historyLastShiftWorked[i]){ // é um dos ultimos turnos trabalhados
				if(historyLastShiftWorked[i] == n_shifts){ // se for uma folga
					outfile << i+1 << " " << j+1 << " " <<  historyTotalConsecutiveDaysOff[i] << endl;
				}else{
					outfile << i+1 << " " << j+1 << " " <<  historyLastShiftConsecutive[i] << endl;
				}
			}else{
				outfile << i+1 << " " << j+1 << " 0" << endl;
			}
		}
	}
	outfile << ";" << endl << endl;
  
	outfile << "param wS1 := 30;" << endl;
	outfile << "param wS2ab := 30;" << endl;
	outfile << "param wS2cd := 15;" << endl;
	outfile << "param wS3 := 30;" << endl;
	outfile << "param wS4 := 10;" << endl;
	outfile << "param wS5 := 30;" << endl;
	outfile << "param wS6ab := 20;" << endl;
	outfile << "param wS7 := 30;" << endl;

	outfile << "end;" << endl;

	outfile.close();



}




















