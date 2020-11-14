set N;  /* conjunto de enfermeiros */
set D;  /* conjunto de dias */
set S;  /* conjunto de turnos incluindo a folga */
set SD; /* conjunto de turnos excluindo a folga */
set K;  /* conjunto de qualificações */
set T; /* conjunto de mínimos consecutivos */

set W dimen 2;          /* +- conjunto de dias dos fins de semanas (w1,w2) */ 
set F dimen 2;          /* +- conjunto de sucessões proibidas (s1, s2) */
set ST dimen 2;   		/* conjunto com dias de segundas e terças */
set QS dimen 2; 		/* conjunto com dias de quintas e sextas */
set B dimen 2;          /* +- conjunto de tuplas onde cada par representa um começo possível de bloco de alocações consecutivas (u,v) */

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

var gs{n in N, d in D, s in S}, binary; #maximo
var gshist{n in N, s in S, i in 1..min(hs[n,s], Lgs[n,s])}, binary;  #maximo
var gd{n in N, d in D}, binary;  #maximo
var gdhist{n in N, 1..min(hd[n], Lgd[n])}, binary;  #maximo

var td{n in N, d in D}, binary; # inicio do bloco de dia consecutivo 
var ts{n in N, d in D, s in S}, binary;             # inicio do bloco de turno consecutivo 
var md{n in N, d in D union {0}, t in T}, binary; 			# minimo dias consecutivos [enf,dia,min]
var ms{n in N, d in D union {0}, s in S, t in T}, binary;   # minimo turnos consecutivos [end,dia,turno,min]


var CS1{d in D, s in SD, k in K}, integer, >=0;      /* Restrição S1 */
var CS5{n in N, (i,j) in W}, binary;                    /* Var auxiliar fim de semana completo */
var CS6a{n in N}, integer, >=0;                      /* Restrição S6a */
var CS6b{n in N}, integer, >=0;                      /* Restrição S6b */
var CS7{n in N}, integer, >=0;                       /* Restrição S7 */



minimize obj: wS1 * sum{d in D, s in SD, k in K} CS1[d,s,k] + 
			
			# consecutividade de dias
			wS2ab * sum{n in N, d in D} (gd[n,d]) + # max consec dia
			
			wS2ab * sum{n in N, d in D: Lld[n]>=2} (md[n,d,1]*(Lld[n]-1)) + 
			wS2ab * sum{n in N, d in D: Lld[n]>=3} (md[n,d,2]*(Lld[n]-2)) + 
			wS2ab * sum{n in N, d in D: Lld[n]>=4} (md[n,d,3]*(Lld[n]-3)) + 
			wS2ab * sum{n in N, d in D: Lld[n]>=5} (md[n,d,4]*(Lld[n]-4)) + 				
									
			wS2ab * sum{n in N: Lld[n]>=2} (md[n,0,1]*(Lld[n]-1)) + 
			wS2ab * sum{n in N: Lld[n]>=3} (md[n,0,2]*(Lld[n]-2)) + 
			wS2ab * sum{n in N: Lld[n]>=4} (md[n,0,3]*(Lld[n]-3)) + 
			wS2ab * sum{n in N: Lld[n]>=5} (md[n,0,4]*(Lld[n]-4)) + 	
										
			wS2ab * sum{n in N, i in 1..min(hd[n], Lgd[n])} (gdhist[n,i]) + # max consec hist

			# consecutividade de turnos s/ folga
			wS2cd * sum{n in N, d in D, s in SD} (gs[n,d,s]) + # max consec

			wS2cd * sum{n in N, d in D, s in SD: Lls[n,s]>=2} (ms[n,d,s,1]*(Lls[n,s]-1)) + 
			wS2cd * sum{n in N, d in D, s in SD: Lls[n,s]>=3} (ms[n,d,s,2]*(Lls[n,s]-2)) + 
			wS2cd * sum{n in N, d in D, s in SD: Lls[n,s]>=4} (ms[n,d,s,3]*(Lls[n,s]-3)) + 
			wS2cd * sum{n in N, d in D, s in SD: Lls[n,s]>=5} (ms[n,d,s,4]*(Lls[n,s]-4)) + 				
									
			wS2cd * sum{n in N, s in SD: Lls[n,s]>=2} (ms[n,0,s,1]*(Lls[n,s]-1)) + 
			wS2cd * sum{n in N, s in SD: Lls[n,s]>=3} (ms[n,0,s,2]*(Lls[n,s]-2)) + 
			wS2cd * sum{n in N, s in SD: Lls[n,s]>=4} (ms[n,0,s,3]*(Lls[n,s]-3)) + 
			wS2cd * sum{n in N, s in SD: Lls[n,s]>=5} (ms[n,0,s,4]*(Lls[n,s]-4)) + 	
	
			wS2cd * sum{n in N, s in SD, i in 1..min(hs[n,s], Lgs[n,s])} (gshist[n,s,i]) + # max consec s/ folga hist

			# consecutividade de folgas
			wS3 * sum{n in N, d in D, s in S: s=card(S)} (gs[n,d,s]) + #max consec

			wS3 * sum{n in N, d in D, s in S: s=card(S) and Lls[n,s]>=2} (ms[n,d,s,1]*(Lls[n,s]-1)) + 
			wS3 * sum{n in N, d in D, s in S: s=card(S) and Lls[n,s]>=3} (ms[n,d,s,2]*(Lls[n,s]-2)) + 
			wS3 * sum{n in N, d in D, s in S: s=card(S) and  Lls[n,s]>=4} (ms[n,d,s,3]*(Lls[n,s]-3)) + 
			wS3 * sum{n in N, d in D, s in S: s=card(S) and  Lls[n,s]>=5} (ms[n,d,s,4]*(Lls[n,s]-4)) + 				
									
			wS3 * sum{n in N, s in S: s=card(S) and  Lls[n,s]>=2} (ms[n,0,s,1]*(Lls[n,s]-1)) + 
			wS3 * sum{n in N, s in S: s=card(S) and  Lls[n,s]>=3} (ms[n,0,s,2]*(Lls[n,s]-2)) + 
			wS3 * sum{n in N, s in S: s=card(S) and  Lls[n,s]>=4} (ms[n,0,s,3]*(Lls[n,s]-3)) + 
			wS3 * sum{n in N, s in S: s=card(S) and  Lls[n,s]>=5} (ms[n,0,s,4]*(Lls[n,s]-4)) + 	
			
			wS3 * sum{n in N, s in S, i in 1..min(hs[n,s], Lgs[n,s]): s=card(S)} (gshist[n,s,i]) + #hist max folgas
			
			# outras restrições
			wS4 * + sum{n in N, d in D, s in SD, k in K} (x[n,d,s,k] * ps[n,d,s] + x[n,d,s,k] * pd[n,d] ) +
			wS5 * sum{n in N, (i,j) in W} CS5[n,i,j] *  V[n] +
			wS6ab * sum{n in N} (CS6a[n] + CS6b[n]) +
			wS7 * sum{n in N} CS7[n] ; 

# ====================================================================================================================

# H1 Enfermeiro deve trabalhar apenas um turno por dia ou folgar 
s.t. H1turnoDia{n in N, d in D}: 
	sum{s in S, k in K} x[n,d,s,k] = 1;


# H2 Deve haver um número mínimo de enfermeiros trabalhando para cada dia/turno/habilidade 
s.t. H2coberturaMinima{d in D, s in SD, k in K}:
	sum{n in N} x[n,d,s,k] >= Vl[d,s,k];


# H3a sucessão de turnos deve ser válida, sem incluir do primeiro dia 
s.t. H3AsucessaoTurnos{n in N, d in D diff {1}, (s1,s2) in F}:
	sum{k in K} (x[n,d-1,s1,k] + x[n,d,s2,k]) <= 1;

# H3b incluindo o primeiro dia */
s.t. H3BsucessaoTurnos{n in N, (s1,s2) in F: s1 = hu[n]}:
	sum{k in K} x[n,1,s2,k] = 0;


# H4 cada enfermeiro deve ter a qualificação para que foi atribuído 
s.t. H4qualificacao{n in N, k in K, d in D}:
	sum{s in SD} x[n,d,s,k] <= q[n,k]; 

# S1 cobertura com número ótimo de enfermeiros 
s.t. S1coberturaOtima{d in D, s in SD, k in K}:
	CS1[d,s,k] >= Vo[d,s,k] - sum{n in N} x[n,d,s,k];


# ====================================================================================================================

# TESTANDO máximo turnos consecutivos
s.t. S2d{n in N, d in D, s in S: d <= card(D)-Lgs[n,s]}:
	sum{k in K, i in d..d+Lgs[n,s]} x[n,i,s,k] <= Lgs[n,s] + gs[n,d,s];

# TESTANDO máximo turnos consecutivos para histórico
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
s.t. aux1S2a{n in N}:
	sum{s in SD, k in K} x[n,1,s,k] - (if hd[n] > 0 then 1 else 0) <= td[n,1];

# liga "td" inicio de um bloco do restante dos dias
s.t. auxS2a{n in N, d in D: d>1}:
	sum{s in SD, k in K} (x[n,d,s,k] - x[n,d-1,s,k]) <= td[n,d];

# TESTE: ALERTA DE CORTE DA VARIAVEL T EM NO MÁXIMO 1 POR SEMANA POR ENFERMEIRO. 
#s.t. corteT{n in N, i in 1..card(D)/7}:
#	sum{d in (i-1)*7+1..i*7: d>1} td[n,d] <= 1;

# ====================================================================================================================
# Mínimo dias com consecutividade == 2

s.t. s2aMin2{n in N, d in D: d<card(D) and Lld[n] >= 2}:
	td[n,d] - sum{s in SD, k in K} x[n,d+1,s,k] <= md[n,d,1];

s.t. s2aHistMin2{n in N: Lld[n] >= 2}: 
	(if hd[n] == 1 then 1 else 0) - sum{s in SD, k in K} x[n,1,s,k] <= md[n,0,1];

# ====================================================================================================================
# Mínimo dias com consecutividade == 3

s.t. s2aMin3{n in N, d in D: d<card(D)-1 and Lld[n] >= 3}:
	td[n,d] + sum{s in SD, k in K} x[n,d+1,s,k] - sum{s in SD, k in K} x[n,d+2,s,k] -1 <= md[n,d,2];

s.t. s2aHistMin3h1{n in N: Lld[n] >= 3 and hd[n] == 1}: 
	sum{s in SD, k in K} x[n,1,s,k] - sum{s in SD, k in K} x[n,2,s,k] <= md[n,0,2];

s.t. s2aHistMin3h2{n in N: Lld[n] >= 3 and hd[n] == 2}: 
	1 - sum{s in SD, k in K} x[n,1,s,k] <= md[n,0,2];

# ====================================================================================================================
# Mínimo dias com consecutividade == 4

s.t. s2aMin4{n in N, d in D: d<card(D)-2 and Lld[n] >= 4}:
	td[n,d] - td[n,d+2] + sum{s in SD, k in K} x[n,d+2,s,k] - sum{s in SD, k in K} x[n,d+3,s,k] -1 <= md[n,d,3];

s.t. s2aHistMin4h1{n in N: Lld[n] >= 4 and hd[n] == 1}: 
	1 - td[n,2] + sum{s in SD, k in K} x[n,2,s,k] - sum{s in SD, k in K} x[n,3,s,k] -1 <= md[n,0,3];

s.t. s2aHistMin4h2{n in N: Lld[n] >= 4 and hd[n] == 2}: 
	1 - td[n,1] + sum{s in SD, k in K} x[n,1,s,k] - sum{s in SD, k in K} x[n,2,s,k] -1 <= md[n,0,3];

s.t. s2aHistMin4h3{n in N: Lld[n] >= 4 and hd[n] == 3}: 
	1 - sum{s in SD, k in K} x[n,1,s,k] <= md[n,0,3];


# ====================================================================================================================
# Mínimo dias com consecutividade == 5

s.t. s2aMin5{n in N, d in D: d<card(D)-3 and Lld[n] >= 5}:
	td[n,d] - td[n,d+2] - td[n,d+3] + td[n,d+4] + sum{s in SD, k in K} x[n,d+3,s,k] - sum{s in SD, k in K} x[n,d+4,s,k] -1 <= md[n,d,4];

s.t. s2aHistMin5h1{n in N: Lld[n] >= 5 and hd[n] == 1}: 
	1 - td[n,2] - td[n,3] + td[n,4] + sum{s in SD, k in K} x[n,3,s,k] - sum{s in SD, k in K} x[n,4,s,k] -1 <= md[n,0,4];

s.t. s2aHistMin5h2{n in N: Lld[n] >= 5 and hd[n] == 2}: 
	1 - td[n,1] - td[n,2] + td[n,3] + sum{s in SD, k in K} x[n,2,s,k] - sum{s in SD, k in K} x[n,3,s,k] -1 <= md[n,0,4];

s.t. s2aHistMin5h3{n in N: Lld[n] >= 5 and hd[n] == 3}: 
	1 - td[n,1] + td[n,2] + sum{s in SD, k in K} x[n,1,s,k] - sum{s in SD, k in K} x[n,2,s,k] -1 <= md[n,0,4];

s.t. s2aHistMin5h4{n in N: Lld[n] >= 5 and hd[n] == 4}: 
	1 + td[n,1] - sum{s in SD, k in K} x[n,1,s,k] <= md[n,0,4];

# ====================================================================================================================

# ligando variáveis que identificam inicio do bloco de turnos

s.t. auxS2b{n in N, s in S, d in D: d>1}:
	sum{k in K} (x[n,d,s,k] - x[n,d-1,s,k]) <= ts[n,d,s];

s.t. aux1S2b{n in N, s in S}:
	sum{k in K} x[n,1,s,k] - (if hs[n,s] > 0 then 1 else 0) <= ts[n,1,s];

# ====================================================================================================================
# Mínimo turnos com consecutividade == 2

s.t. s2b{n in N, s in S, d in D: d<card(D) and Lls[n,s] >= 2}:
	ts[n,d,s] - sum{k in K} x[n,d+1,s,k] <= ms[n,d,s,1];

s.t. s2bHist{n in N, s in S: Lls[n,s] >= 2}:
	(if hs[n,s] == 1 then 1 else 0) - sum{k in K} x[n,1,s,k] <= ms[n,0,s,1];

# ====================================================================================================================
# Mínimo turnos com consecutividade == 3

s.t. s2bMin3turnos{n in N, s in S, d in D: d<card(D)-1 and Lls[n,s] >= 3}:
    ts[n,d,s] + sum{k in K} x[n,d+1,s,k] - sum{k in K} x[n,d+2,s,k] -1 <= ms[n,d,s,2];


s.t. s2bHistMin3h1turnos{s in S, n in N: Lls[n,s] >= 3 and hs[n,s] == 1}: 
    sum{k in K} x[n,1,s,k] - sum{k in K} x[n,2,s,k] <= ms[n,0,s,2];


s.t. s2bHistMin3h2turnos{n in N, s in S: Lls[n,s] >= 3 and hs[n,s] == 2}: 
	1 - sum{k in K} x[n,1,s,k] <= ms[n,0,s,2];

# ====================================================================================================================
# Mínimo turnos com consecutividade == 4

s.t. s2bMin4{n in N, s in S, d in D: d<card(D)-2 and Lls[n,s] >= 4}:
	ts[n,d,s] - ts[n,d+2,s] + sum{k in K} x[n,d+2,s,k] - sum{k in K} x[n,d+3,s,k] -1 <= ms[n,d,s,3];

s.t. s2bHistMin4h1{n in N, s in S: Lls[n,s] >= 4 and hs[n,s] == 1}: 
	1 - ts[n,2,s] + sum{k in K} x[n,2,s,k] - sum{k in K} x[n,3,s,k] -1 <= ms[n,0,s,3];

s.t. s2bHistMin4h2{n in N, s in S: Lls[n,s] >= 4 and hs[n,s] == 2}: 
	1 - ts[n,1,s] + sum{k in K} x[n,1,s,k] - sum{k in K} x[n,2,s,k] -1 <= ms[n,0,s,3];

s.t. s2bHistMin4h3{n in N, s in S: Lls[n,s] >= 4 and hs[n,s] == 3}: 
	1 - sum{k in K} x[n,1,s,k] <= ms[n,0,s,3];


# ====================================================================================================================
# Mínimo turnos com consecutividade == 5

s.t. s2bMin5{n in N, s in S, d in D: d<card(D)-3 and Lls[n,s] >= 5}:
	ts[n,d,s] - ts[n,d+2,s] - ts[n,d+3,s] + ts[n,d+4,s] + sum{k in K} x[n,d+3,s,k] - sum{k in K} x[n,d+4,s,k] -1 <= ms[n,d,s,4];

s.t. s2bHistMin5h1{n in N, s in S: Lls[n,s] >= 5 and hs[n,s] == 1}: 
	1 - ts[n,2,s] - ts[n,3,s] + ts[n,4,s] + sum{k in K} x[n,3,s,k] - sum{k in K} x[n,4,s,k] -1  <= ms[n,0,s,4];

s.t. s2bHistMin5h2{n in N, s in S: Lls[n,s] >= 5 and hs[n,s] == 2}: 
	1 - ts[n,1,s] - ts[n,2,s] + ts[n,3,s] + sum{k in K} x[n,2,s,k] - sum{k in K} x[n,3,s,k] -1  <= ms[n,0,s,4];

s.t. s2bHistMin5h3{n in N, s in S: Lls[n,s] >= 5 and hs[n,s] == 3}: 
	1 - ts[n,1,s] + ts[n,2,s] + sum{k in K} x[n,1,s,k] - sum{k in K} x[n,2,s,k] -1  <= ms[n,0,s,4];

s.t. s2bHistMin5h34{n in N, s in S: Lls[n,s] >= 5 and hs[n,s] == 4}: 
	1 + ts[n,1,s] - sum{k in K} x[n,1,s,k]  <= ms[n,0,s,4];


# ====================================================================================================================

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























end;
