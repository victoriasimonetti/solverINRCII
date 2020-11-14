set N;  /* conjunto de enfermeiros */
set D;  /* conjunto de dias */
set S;  /* conjunto de turnos incluindo a folga */
set SD; /* conjunto de turnos excluindo a folga */
set K;  /* conjunto de qualificações */

set W dimen 2;          /* +- conjunto de dias dos fins de semanas (w1,w2) */ 
set F dimen 2;          /* +- conjunto de sucessões proibidas (s1, s2) */
set B dimen 2;          /* +- conjunto de tuplas onde cada par representa um começo possível de bloco de alocações consecutivas (u,v) */

set ST dimen 2;   		/* conjunto com dias de segundas e terças */
set QS dimen 2; 		/* conjunto com dias de quintas e sextas */

param q{n in N, k in K};            /* ??? diferente do modelo do tcc!!! conjunto de qualificações do enfermeiro n */
param ps{n in N, d in D, s in SD};  /* 1 se enfermeiro não prefere ser alocado no turno s do dia d */
param pd{n in N, d in D};           /* 1 se enfermeiro não quer se alocado em nenhum turno do dia d  */
param Vl{d in D, s in SD, k in K};  /* Cobertura mínima */
param Vo{d in D, s in SD, k in K};  /* Cobertura ótima */
param Lgd{n in N};                  /* Número máximo de dias consecutivos trabalhados, depende do contrato do enf, por isso in N */
param Lld{n in N};                  /* Número mínimo de dias consecutivos trabalhados */
param Lgs{n in N, s in S};         /* Número máximo de dias consecutivos trabalhados no turno s, independe do contrato do enf, por isso em SD */
param Lls{n in N, s in S};         /* Número mínimo de dias consecutivos trabalhados no turno s */
param V{n in N};                    /* Assume 1 se o enfermeiro precisa trabalhar todo o fim de semana */
param hd{n in N};                   /* Histórico do número de dias consecutivos trabalhados no estágio anterior */
param hs{n in N, s in S};          /* Histórico do número de alocações consecutivas no turno s no estágio anterior, incluindo a folga */
param hu{n in N};                   /* Histórico do último turno trabalhado no estágio anterior */
param Qg{n in N};                   /* Número máximo de dias trabalhandos no total */
param Ql{n in N};                   /* Número mínimo de dias trabalhandos no total */
param Rg{n in N};                   /* Número máximo de fins de semana trabalhados */
param wS1;
param wS2ab;
param wS2cd;
param wS3;
param wS4;
param wS5;
param wS6ab;
param wS7;                         /* Peso das restrições de S1 a S7 */

var x{n in N, d in D, s in S, k in K}, binary ; 
var z{n in N, (i,j) in W}, binary;     /* Var auxiliar fim de semana completo */
var r{n in N, (i,j) in W}, binary;     /* Var auxiliar fim de semana completo */

var bd{n in N, (i,j) in B}, binary;    /* 1 se enfermeiro começa um bloco de alocação iniciando no dia i e terminando dia j */
var bs{n in N, (i,j) in B, s in S}, binary; /* 1 se enfermeiro começa um bloco de alocação de mesmo turno s iniciando no dia i e terminando dia j */

var gs{n in N, d in D, s in S}, binary; #maximo
var gshist{n in N, s in S, i in 1..min(hs[n,s], Lgs[n,s])}, binary;  #maximo
var gd{n in N, d in D}, binary;  #maximo
var gdhist{n in N, 1..min(hd[n], Lgd[n])}, binary;  #maximo

var ad{n in N, d in D union {0}}, binary; # minimo dias consecutivos ==2
var as{n in N, d in D union {0}, s in S}, binary; # minimo turnos consecutivos ==2
var td{n in N, d in D}, binary; # inicio do bloco de dia consecutivo
var ts{n in N, d in D, s in S}, binary; # inicio do bloco de turno consecutivo

var cd{n in N, d in D union {0}} >= 0; # minimo dias consecutivos == 3
var cs{n in N, d in D union {0}, s in S} >= 0; # minimo dias consecutivos == 3

var CS1{d in D, s in SD, k in K}, integer, >=0;      /* Restrição S1 */
var CS2a{n in N, (i,j) in B}, integer, >=0;          /* Restrição S2a */
var CS2b{n in N, (i,j) in B}, integer, >=0;          /* Restrição S2b */
var CS2cS3a{n in N, (i,j) in B, s in S}, integer, >=0; /* Restrição S2cS3a */
var CS2cS3ahist{n in N, s in S}, integer, >=0;
var CS5{n in N, (i,j) in W}, binary;                    /* Var auxiliar fim de semana completo */
var CS6a{n in N}, integer, >=0;                      /* Restrição S6a */
var CS6b{n in N}, integer, >=0;                      /* Restrição S6b */
var CS7{n in N}, integer, >=0;                       /* Restrição S7 */


var gambi, binary;
var coberturaOpt30, integer, >=0;

var MinconsecDias30, integer, >=0;
var MinconsecTurno15, integer, >=0;  
var MinconsecFolga30, integer, >=0; 
var MaxconsecDias30, integer, >=0;
var MaxconsecTurno15, integer, >=0;  
var MaxconsecFolga30, integer, >=0; 

var pref10, integer, >=0; 
var completeWeekend30, integer, >=0; 
var totalDiasTrab20, integer, >=0; 
var maxFimSemanaTrab30, integer, >=0; 


minimize obj: wS1 * sum{d in D, s in SD, k in K} CS1[d,s,k] + 
               
			  wS2ab * sum{n in N, (i,j) in B: j-i<6} ( CS2a[n,i,j]) + # min bloco dias consec trab

			  wS2cd * sum{n in N, (i,j) in B, s in SD: j-i<6} ( CS2cS3a[n,i,j,s] ) + # min blocos turnos consec trab s/ folga
			  wS2cd * sum{n in N, s in SD}	CS2cS3ahist[n,s]+ # min blocos turnos s/ folga (historico)

			  wS3 * sum{n in N, (i,j) in B, s in S: s=card(S) and j-i<6} ( CS2cS3a[n,i,j,s]) + # min blocos folgas consec trab 
			  wS3 * sum{n in N, s in S: s=card(S)}	CS2cS3ahist[n,s] + # min blocos turnos folga (historico)
			  
			  # consecutividade de dias
			  wS2ab * sum{n in N, d in D} (gd[n,d] + ad[n,d] + cd[n,d]) + # max/ min==2/3 consec dias
              wS2ab * sum{n in N, i in 1..min(hd[n], Lgd[n])} (gdhist[n,i]) + # max consec hist
              wS2ab*gambi* sum{n in N} (max(hd[n]-Lgd[n],0)) + #hist parte fixa
              wS2ab * sum{n in N} (ad[n,0] + cd[n,0]) + #hist

			  # consecutividade de turnos s/ folga
			  wS2cd * sum{n in N, d in D, s in SD} (gs[n,d,s] + as[n,d,s] + cs[n,d,s]) + # max/ min==2/3 consec turnos s/ folga
              wS2cd * sum{n in N, s in SD, i in 1..min(hs[n,s], Lgs[n,s])} (gshist[n,s,i]) + # max consec s/ folga hist
              wS2cd *gambi* sum{n in N, s in SD} (max(hs[n,s]-Lgs[n,s],0)) + #hist parte fixa
              wS2cd * sum{n in N, s in SD}  (as[n,0,s] + cs[n,0,s]) +  #hist

			  # consecutividade de folgas
			  wS3 * sum{n in N, d in D, s in S: s=card(S)} (gs[n,d,s] + as[n,d,s] + cs[n,d,s]) + # max min consec folgas
              wS3 * sum{n in N, s in S, i in 1..min(hs[n,s], Lgs[n,s]): s=card(S)} (gshist[n,s,i]) + #hist max folgas
              wS3 *gambi* sum{n in N, s in S: s=card(S)} (max(hs[n,s]-Lgs[n,s],0)) + #hist parte fixa max folgas
              wS3 * sum{n in N, s in S: s=card(S)}  (as[n,0,s] + cs[n,0,s]) +  #hist		  

			  wS4 * + sum{n in N, d in D, s in SD, k in K} (x[n,d,s,k] * ps[n,d,s] + x[n,d,s,k] * pd[n,d] ) +
			  wS5 * sum{n in N, (i,j) in W} CS5[n,i,j] *  V[n] +
			  wS6ab * sum{n in N} (CS6a[n] + CS6b[n]) +
			  wS7 * sum{n in N} CS7[n] ; 
				
# GAMBIARRA PARA INCLUIR VALORES CONSTANTES DO HISTÓRICO NO CPLEX
s.t. gaamm:
    gambi = 1;

# RESTRICOES APENAS PARA DEBUG - NAO DEIXAR LIGADO AO RODAR OS TESTES		
/*
s.t. vers1:
	sum{d in D, s in SD, k in K} CS1[d,s,k] = coberturaOpt30;

s.t.minConsecdias:
	MinconsecDias30 = sum{n in N, (i,j) in B: j-i<6} ( CS2a[n,i,j]) + sum{n in N, d in D} (ad[n,d] + cd[n,d]) + sum{n in N} (ad[n,0] + cd[n,0]) + sum{n in N, (i,j) in B: j-i<6} ( CS2a[n,i,j]) ;

s.t.maxConsecdias:
	MaxconsecDias30 = sum{n in N, d in D} (gd[n,d]) + sum{n in N, i in 1..min(hd[n], Lgd[n])} (gdhist[n,i]) + sum{n in N} (max(hd[n]-Lgd[n],0));

s.t. minConsecTurnos:
	MinconsecTurno15 = sum{n in N, d in D, s in SD} (as[n,d,s] + cs[n,d,s]) + sum{n in N, s in SD}  (as[n,0,s] + cs[n,0,s]) + sum{n in N, (i,j) in B, s in SD: j-i<6} ( CS2cS3a[n,i,j,s]) + sum{n in N, s in SD} CS2cS3ahist[n,s];

s.t. maxConsecTurnos:
	MaxconsecTurno15 = sum{n in N, d in D, s in SD} (gs[n,d,s]) + sum{n in N, s in SD, i in 1..min(hs[n,s], Lgs[n,s])} (gshist[n,s,i]) + sum{n in N, s in SD} (max(hs[n,s]-Lgs[n,s],0)) ;


s.t. minconsecFolga:
	MinconsecFolga30 = sum{n in N, d in D, s in S: s=card(S)} (as[n,d,s] + cs[n,d,s]) + sum{n in N, s in S: s=card(S)}  (as[n,0,s] + cs[n,0,s]) + sum{n in N, (i,j) in B, s in S: s=card(S) and j-i<6} ( CS2cS3a[n,i,j,s]) + sum{n in N, s in S: s=card(S)} CS2cS3ahist[n,s] ;

s.t. maxconsecFolga:
	MaxconsecFolga30 = sum{n in N, d in D, s in S: s=card(S)} (gs[n,d,s]) + sum{n in N, s in S, i in 1..min(hs[n,s], Lgs[n,s]): s=card(S)} (gshist[n,s,i]) + sum{n in N, s in S: s=card(S)} (max(hs[n,s]-Lgs[n,s],0));
 
s.t. vers4:
	pref10 = sum{n in N, d in D, s in SD, k in K} (x[n,d,s,k] * ps[n,d,s] + x[n,d,s,k] * pd[n,d] );

s.t. vers5:
	completeWeekend30 = sum{n in N, (i,j) in W} CS5[n,i,j] *  V[n] ;

s.t. vers6ab:
	totalDiasTrab20 = sum{n in N} (CS6a[n] + CS6b[n]) ;

s.t. vers7:
	maxFimSemanaTrab30 = sum{n in N} CS7[n];
*/


# ====================================================================================================================

/* H1 Enfermeiro deve trabalhar apenas um turno por dia ou folgar */
s.t. H1turnoDia{n in N, d in D}: 
	sum{s in S, k in K} x[n,d,s,k] = 1;


/* H2 Deve haver um número mínimo de enfermeiros trabalhando para cada dia/turno/habilidade */
s.t. H2coberturaMinima{d in D, s in SD, k in K}:
	sum{n in N} x[n,d,s,k] >= Vl[d,s,k];


/* H3a sucessão de turnos deve ser válida, sem incluir do primeiro dia */
s.t. H3AsucessaoTurnos{n in N, d in D diff {1}, (s1,s2) in F}:
	sum{k in K} (x[n,d-1,s1,k] + x[n,d,s2,k]) <= 1;

/* H3b incluindo o primeiro dia */
s.t. H3BsucessaoTurnos{n in N, (s1,s2) in F: s1 = hu[n]}:
	sum{k in K} x[n,1,s2,k] = 0;


/* WARNING diferente do tcc, modelei as quaificações do enfermeiro como parâmetro */
# H4 cada enfermeiro deve ter a qualificação para que foi atribuído 
s.t. H4qualificacao{n in N, k in K, d in D}:
	sum{s in SD} x[n,d,s,k] <= q[n,k]; 

# S1 cobertura com número ótimo de enfermeiros 
s.t. S1coberturaOtima{d in D, s in SD, k in K}:
	CS1[d,s,k] >= Vo[d,s,k] - sum{n in N} x[n,d,s,k];

# ====================================================================================================================
/*
#Ligando as varivável auxiliares dos blocos de dias corridos 

s.t. S2aux1{n in N, (u,v) in B, i in u..v: Lld[n] > 3 and v-u<6}:
	bd[n,u,v] <= sum{s in SD, k in K} x[n,i,s,k];

s.t. S2aux2{n in N, (u,v) in B: u>1 and Lld[n] > 3 and v-u<6}:
	bd[n,u,v] + sum{s in SD, k in K} x[n,u-1,s,k] <=1;

s.t. S2aux3{n in N, (u,v) in B: v < card(D) and Lld[n] > 3 and v-u<6}:
	bd[n,u,v] + sum{s in SD, k in K} x[n,v+1,s,k] <=1;

s.t. S2aux7{n in N: Lld[n] > 3}:
	sum{d in D, s in SD, k in K} x[n,d,s,k] = sum{(u,v) in B: v-u<6} (v-u+1)*bd[n,u,v];


s.t. S2a{n in N, (u,v) in B: Lld[n] > 3 and v-u<6 and v!=card(D)}: # S2a: mínimo de dias consecutivos trabalhados
	CS2a[n,u,v] >= Lld[n] * bd[n,u,v] - ((v-u+1 + hd[n]*floor(1/u)) *bd[n,u,v]);

# ====================================================================================================================

# Ligando as varivável auxiliares dos blocos dos turnos incluindo a folga 

s.t. S2S3aux4{n in N, s in S, (u,v) in B, i in u..v: Lls[n,s] > 3 and v-u<6}:
	bs[n,u,v,s] <= sum{k in K} x[n,i,s,k];

s.t. S2S3aux5{n in N, s in S, (u,v) in B: u>1 and Lls[n,s] > 3 and v-u<6}:
	bs[n,u,v,s] + sum{k in K} x[n,u-1,s,k] <=1;

s.t. S2S3aux6{n in N, s in S, (u,v) in B: v < card(D) and Lls[n,s] > 3 and v-u<6}:
	bs[n,u,v,s] + sum{k in K} x[n,v+1,s,k] <=1; 

s.t. S2S3aux8{n in N, s in S: Lls[n,s] > 3}:
	sum{d in D, k in K} x[n,d,s,k] = sum{(u,v) in B: v-u<6} (v-u+1)*bs[n,u,v,s];

# S2cS3a: mínimo de dias consecutivos trabalhados no mesmo turno e folga 
# histórico mesmo turno
s.t. S2c{n in N, (u,v) in B, s in S: Lls[n,s] > 3 and v-u<6 and v!=card(D)}:
	CS2cS3a[n,u,v,s] >= Lls[n,s] * bs[n,u,v,s] - ((v-u+1 + hs[n,s]*floor(1/u)) * bs[n,u,v,s]);

# histórico diferente turno
s.t. S2chist{n in N, s in S: Lls[n,s] > 3 and hs[n,s]>0}:
	CS2cS3ahist[n,s] >= sum{s1 in S, k in K: s1!=s} x[n,1,s1,k] * Lls[n,s] - hs[n,s];


# ====================================================================================================================

# TESTANDO máximo turnos consecutivos
s.t. S2d{n in N, d in D, s in S: d <= card(D)-Lgs[n,s]}:
	sum{k in K, i in d..d+Lgs[n,s]} x[n,i,s,k] <= Lgs[n,s] + gs[n,d,s];

# TESTANDO máximo dias consecutivos para histórico
s.t. S2dHist{n in N, s in S, i in 1..min(hs[n,s], Lgs[n,s])}: #Funciona para qualquer limite maximo
	i + sum{k in K, d in 1..(Lgs[n,s]+1-i)} x[n,d,s,k] <= Lgs[n,s] + gshist[n,s,i];

# TESTANDO máximo dias consecutivos
s.t. S2b{n in N, d in D: d <= card(D)-Lgd[n]}:
	sum{s in SD, k in K, i in d..d+Lgd[n]} x[n,i,s,k] <= Lgd[n] + gd[n,d];

# TESTANDO máximo dias consecutivos para histórico
s.t. S2bHist{n in N, i in 1..min(hd[n], Lgd[n])}: #Funciona para qualquer limite maximo
	i + sum{s in SD, k in K, d in 1..(Lgd[n]+1-i)} x[n,d,s,k] <= Lgd[n] + gdhist[n,i];

# ====================================================================================================================

# ligando variáveis que identificam inicio do bloco de dias

# liga "td" inicio de um bloco do dia 1
s.t. aux1S2a{n in N: Lld[n] <= 3}:
	sum{s in SD, k in K} x[n,1,s,k] - (if hd[n] > 0 then 1 else 0) <= td[n,1];

# liga "td" inicio de um bloco do restante dos dias
s.t. auxS2a{n in N, d in D: d>1 and Lld[n] <= 3}:
	sum{s in SD, k in K} (x[n,d,s,k] - x[n,d-1,s,k]) <= td[n,d];

# ====================================================================================================================

# Mínimo dias com consecutividade == 2

# ad é o custo == 2 sem histórico
s.t. s2aMin2{n in N, d in D: d<card(D) and Lld[n] == 2}:
	td[n,d] - sum{s in SD, k in K} x[n,d+1,s,k] <= ad[n,d];

s.t. s2aHistMin2{n in N: Lld[n] == 2}: 
	(if hd[n] == 1 then 1 else 0) - sum{s in SD, k in K} x[n,1,s,k] <= ad[n,0];

# ====================================================================================================================
# Mínimo dias com consecutividade == 3

s.t. s2aMin3{n in N, d in D: d<card(D)-1 and Lld[n] == 3}:
	2*td[n,d] - sum{s in SD, k in K} x[n,d+1,s,k] - sum{s in SD, k in K} x[n,d+2,s,k] + td[n,d+2] <= cd[n,d];

s.t. s2aMin3Fim{n in N, d in D: d=card(D)-1 and Lld[n] == 3}:
	2*td[n,d] - 2*sum{s in SD, k in K} x[n,d+1,s,k] <= cd[n,d];


s.t. s2aHistMin3h1{n in N: Lld[n] == 3 and hd[n] == 1}: 
	2 - sum{s in SD, k in K} x[n,1,s,k] - sum{s in SD, k in K} x[n,2,s,k] + td[n,2] <= cd[n,0];

s.t. s2aHistMin3h2{n in N: Lld[n] == 3 and hd[n] == 2}: 
	2 - 1 - sum{s in SD, k in K} x[n,1,s,k] + td[n,1] <= cd[n,0];

# ====================================================================================================================

# ligando variáveis que identificam inicio do bloco de turnos

s.t. auxS2b{n in N, s in S, d in D: d>1 and Lls[n,s] <= 3}:
	sum{k in K} (x[n,d,s,k] - x[n,d-1,s,k]) <= ts[n,d,s];

s.t. aux1S2b{n in N, s in S: Lls[n,s] <= 3}:
	sum{k in K} x[n,1,s,k] - (if hs[n,s] > 0 then 1 else 0) <= ts[n,1,s];


# ====================================================================================================================

# Mínimo turnos com consecutividade == 2

s.t. s2b{n in N, s in S, d in D: d<card(D) and Lls[n,s] == 2}:
	ts[n,d,s] - sum{k in K} x[n,d+1,s,k] <= as[n,d,s];

s.t. s2bHist{n in N, s in S: Lls[n,s] == 2}:
	(if hs[n,s] == 1 then 1 else 0) - sum{k in K} x[n,1,s,k] <= as[n,0,s];


# ====================================================================================================================
# Mínimo turnos com consecutividade == 3

s.t. s2aMin3turnos{n in N, s in S, d in D: d<card(D)-1 and Lls[n,s] == 3}:
	2*ts[n,d,s] - sum{k in K} x[n,d+1,s,k] - sum{k in K} x[n,d+2,s,k] + ts[n,d+2,s] <= cs[n,d,s];

s.t. s2aMin3turnosFim{n in N, s in S, d in D: d=card(D)-1 and Lls[n,s] == 3}:
	2*ts[n,d,s] - 2 * sum{k in K} x[n,d+1,s,k] <= cs[n,d,s];


s.t. s2aHistMin3h1turnos{s in S, n in N: Lls[n,s] == 3 and hs[n,s] == 1}: 
	2 - sum{k in K} x[n,1,s,k] - sum{k in K} x[n,2,s,k] + ts[n,2,s] <= cs[n,0,s];

s.t. s2aHistMin3h2turnos{n in N, s in S: Lls[n,s] == 3 and hs[n,s] == 2}: 
	2 - 1 - sum{k in K} x[n,1,s,k] + ts[n,1,s] <= cs[n,0,s];

# ====================================================================================================================
*/

# Auxiliar para restrição de fins de semana completo 
s.t. auxS5{n in N, (i,j) in W}:
	CS5[n,i,j] = sum{s in SD, k in K} (x[n,i,s,k] + x[n,j,s,k]) - 2*z[n,i,j];

# S6a: número máximo de dias trabalhados no total 
s.t. S6a{n in N}:
	CS6a[n] >= sum{d in D, s in SD, k in K} x[n,d,s,k] - Qg[n];

# S6b: número mínimo de dias trabalhados no total 
s.t. S6b{n in N}:
	CS6b[n] >= Ql[n] - sum{d in D, s in SD, k in K} x[n,d,s,k];

# WARNING: rever essa restrição, parece que quando 0+0 o r poderia assumir qualquer valor 
# Auxiliar da S7 
s.t. auxS7{n in N, (i,j) in W}:
	2*r[n,i,j] >= sum{s in SD, k in K} (x[n,i,s,k] + x[n,j,s,k]);

# S7: número máximo de fins de semana trabalhados 
s.t. S7{n in N}:
	CS7[n] >= sum{(i,j) in W} r[n,i,j] - Rg[n];

# ====================================================================================================================


#Versão 1
# Restrição para modelar dias com turnos iguais: Seg=ter, qui=sex, sab=dom, qua=qui OU qua=ter

s.t. finsDeSemanaIguais{n in N, (i,j) in W, s in S}:
	sum{k in K} x[n,i,s,k] = sum{k in K} x[n,j,s,k];

s.t. segTercaIguais{n in N, (i,j) in ST, s in S}:
	sum{k in K} x[n,i,s,k] = sum{k in K} x[n,j,s,k];

s.t. quiSexIguais{n in N, (i,j) in QS, s in S}:
	sum{k in K} x[n,i,s,k] = sum{k in K} x[n,j,s,k];

#s.t. tercaQuaIguais{n in N, (i,j) in ST, s in S}:
#	sum{k in K} x[n,j,s,k] = sum{k in K} x[n,j+1,s,k];

s.t. quartasIguais{n in N, (i,j) in ST, s in S}:
	sum{k in K} x[n,j+1,s,k] <= sum{k in K} x[n,j,s,k] + sum{k in K} x[n,j+2,s,k];


/*
#Versão 2
# Restrição para modelar dias com turnos e skills iguais: Seg=ter, qui=sex, sab=dom, qua=qui OU qua=ter

s.t. finsDeSemanaIguais{n in N, (i,j) in W, s in S, k in K}:
	x[n,i,s,k] = x[n,j,s,k];

s.t. segTercaIguais{n in N, (i,j) in ST, s in S, k in K}:
	x[n,i,s,k] = x[n,j,s,k];

s.t. quiSexIguais{n in N, (i,j) in QS, s in S, k in K}:
	x[n,i,s,k] = x[n,j,s,k];

#s.t. tercaQuaIguais{n in N, (i,j) in ST, s in S, k in K}:
#	x[n,j,s,k] = x[n,j+1,s,k];

s.t. quartasIguais{n in N, (i,j) in ST, s in S, k in K}:
	x[n,j+1,s,k] <= x[n,j,s,k] + x[n,j+2,s,k];
*/

/*
# Versão 3 
# Restrição para modelar turnos iguais. ter=seg OU ter=qua, qui=qua OU qui=sex,...

s.t. paresIguais{n in N, s in S, d in D: d>1 and d<card(D)}:
	sum{k in K} x[n,d,s,k] <= sum{k in K} x[n,d-1,s,k] + sum{k in K} x[n,d+1,s,k];
*/

























end;
