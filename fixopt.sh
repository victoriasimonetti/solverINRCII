#!/bin/bash
# -i instância
# -t tempo limite com inicialização do LAHC (segundos)
# -a algoritmo (3:LAHC 4:fixOpt 5:LAFO)
# -L tamanho da lista do LAHC usado para gerar solução inicial
# -s tempo do subproblema do fixOpt (segundos)
# -l tamanho da lista do LAHC para o LAFO
# -n tipo de vizinhança do fixOpt 
#    0: lexicográfica (incrementa 1)
#    1: probabilidades
# -h tempo limite para o LAHC

# Objetivo é testar a relação entre o tamanho da lista do LAFO e o tempo limite do LAHC. 
# O tamanho da lista do LAHC é baseada no meu tcc, onde o irace encontrou como melhor valor 5000.

T=86400
mkdir Testes/1
./solver.release -i 10 -t $T -a 5 -L 5000 -s 60 -l 1 -h 60 -n 0 -w "Testes/1"  > Testes/1/10.log & 

mkdir Testes/2
./solver.release -i 10 -t $T -a 5 -L 5000 -s 60 -l 1 -h 600 -n 0 -w "Testes/2"  > Testes/2/10.log & 

mkdir Testes/3
./solver.release -i 10 -t $T -a 5 -L 5000 -s 60 -l 10 -h 60 -n 0 -w "Testes/3"  > Testes/3/10.log & 

mkdir Testes/4
./solver.release -i 10 -t $T -a 5 -L 5000 -s 60 -l 10 -h 600 -n 0 -w "Testes/4"  > Testes/4/10.log & 

mkdir Testes/5
./solver.release -i 10 -t $T -a 5 -L 5000 -s 60 -l 100 -h 60 -n 0 -w "Testes/5"  > Testes/5/10.log & 

mkdir Testes/6
./solver.release -i 10 -t $T -a 5 -L 5000 -s 60 -l 100 -h 600 -n 0 -w "Testes/6"  > Testes/6/10.log & 

mkdir Testes/7
./solver.release -i 10 -t $T -a 5 -L 5000 -s 60 -l 1000 -h 60 -n 0 -w "Testes/7"  > Testes/7/10.log & 

mkdir Testes/8
./solver.release -i 10 -t $T -a 5 -L 5000 -s 60 -l 1000 -h 600 -n 0 -w "Testes/8"  > Testes/8/10.log & 



mkdir Testes/9
./solver.release -i 10 -t $T -a 5 -L 5000 -s 30 -l 1 -h 60 -n 0 -w "Testes/9"  > Testes/9/10.log & 

mkdir Testes/10
./solver.release -i 10 -t $T -a 5 -L 5000 -s 30 -l 1 -h 600 -n 0 -w "Testes/10"  > Testes/10/10.log & 

mkdir Testes/11
./solver.release -i 10 -t $T -a 5 -L 5000 -s 30 -l 10 -h 60 -n 0 -w "Testes/11"  > Testes/11/10.log & 

mkdir Testes/12
./solver.release -i 10 -t $T -a 5 -L 5000 -s 30 -l 10 -h 600 -n 0 -w "Testes/12"  > Testes/12/10.log & 

mkdir Testes/13
./solver.release -i 10 -t $T -a 5 -L 5000 -s 30 -l 100 -h 60 -n 0 -w "Testes/13"  > Testes/13/10.log & 

mkdir Testes/14
./solver.release -i 10 -t $T -a 5 -L 5000 -s 30 -l 100 -h 600 -n 0 -w "Testes/14"  > Testes/14/10.log & 

mkdir Testes/15
./solver.release -i 10 -t $T -a 5 -L 5000 -s 30 -l 1000 -h 60 -n 0 -w "Testes/15"  > Testes/15/10.log & 

mkdir Testes/16
./solver.release -i 10 -t $T -a 5 -L 5000 -s 30 -l 1000 -h 600 -n 0 -w "Testes/16"  > Testes/16/10.log & 






 


