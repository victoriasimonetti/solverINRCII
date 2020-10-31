
#***********************************************************
# Makefile Genérico para programas em C/C++       
# Autor:  Árton Pereira Dorneles                           
# E-mail: artondorneles@gmail.com                          
# Alterado em: 11/03/2012                                  
#                                                          
# Uso - Este makefile deve residir junto com o código fonte    
# ou um nível acima. Configure as variáveis que precisar   
# a seguir PROG_NAME,PROG_VERSION,CC,ARCH,SRC_EXTENSION,SRCS,MODE,      
#          DEBUG_FLAGS,RELEASE_FLAGS,LDFLAGS,CFLAGS,LDPATH,
#          
#                                                          
# Comandos úteis:                                          
#                                                          
#  make -> Compila no modo padrão                          
#  make debug -> Compila usando as DEBUG_FLAGS             
#  make release -> Compila usando as RELEASE_FLAGS         
#  make dist -> Limpa e Compila usando as RELEASE_FLAGS    
#  make clean -> Limpa os arquivos binários e backups      
#  make run -> Compila no modo padrão e executa o programa
#              com os parâmetros definidos por PARAMETERS
#  make show -> Exibe a lista de fontes visíveis ao make 
#  make valgrind -> idêntico a 'make run' mas sob o valgrind
#  make kdbg -> idêntico a 'make run' mas sob o kdbg
#              
#                                                          
# A extensão do executável gerado será .debug ou .release  
#                                                          
#***********************************************************
 
# Variáveis que voce pode modificar ------------------------

TARGET_OS = linux

# Arquitetura pretendida
#ARCH = native
ARCH = k8
#ARCH = i686

# Nome do programa
PROG_NAME  = solver

# Versão do programa. No código é possível utilizar a constante PROG_VERSION para exibi-la.
PROG_VERSION = 0.1 #Obtém a versão do repositório

#Parametros do make run
PARAMETERS=-m ../test/h-1.lp -a bb

# Compilador, e libs
ifeq ($(TARGET_OS),linux)
   CC = g++
	ifeq ($(ARCH),k8)
		OBJECT_LIBS = ./_opt/libopt64.a
	endif
	ifeq ($(ARCH),i686)
		OBJECT_LIBS = ./_opt/libopt.a
	endif
endif
ifeq ($(TARGET_OS),win32)
   CC = i586-mingw32msvc-g++
   OBJECT_LIBS = ./_opt/libopt.dll
endif


# Extensao dos fontes 
SRC_EXTENSION = cpp

# Nome dos fontes necessários para construir o executável
# O Código abaixo detecta os fontes com a extensão escolhida recursivamente
SRCS    = $(shell find -L . ! -name .\* ! -path ./_\* -name \*.$(SRC_EXTENSION))
# Caso queira selecionar manualmente os fontes comente a linha acima e faça algo como a linha abaixo:
# SRCS = main.cpp meuoutroarquivo.cpp subdiretorio1/outroarquivo.cpp 

#Modo default de compilação
#Aceita debug,release
MODE=debug

# Opções do compilador para os modos debug e release
# -g    :ativa debug no binário mas não garante maiores informações
# -ggdb :ativa debug o mais informativo possível
# -std=c99 : permite somente o padrao c99
DEBUG_FLAGS = -ggdb
RELEASE_FLAGS = -O3
WARNING_FLAGS = -Wall -Wconversion #-Wshadow

INCLUDES = -I_opt \
			  -I_cplex/include/ilcplex \
           -I_coin/include/coin/ThirdParty \
           -I_coin/include/coin \
			-I include

ifeq ($(ARCH),k8)
	LDFLAGS = -L_coin/lib \
				 -L_cplex/lib/x86-64_linux/static_pic/ 
endif
ifeq ($(ARCH),i686)
	LDFLAGS = -L_coin/lib \
				 -L_cplex/lib/x86_sles10_4.1/static_pic/ 
endif


# -lm     :math por exemplo  
LDLIBS  = -lOsiCpx -lOsiCbc -lOsiClp -lOsiGlpk -lClp -lCbc -lCgl -lOsi -lCoinUtils -lCbcSolver -lcoinlapack -lcoinblas -lcoinglpk  \
          -lcplex -lpthread  \
          -lz -ldl -lrt -lbz2 -lm 
          
LDPATH = -Wl,-rpath _coin/lib 

   
# ---------------------------------------------------------------------
# NÃO MODIFIQUE DAQUI PARA BAIXO AO MENOS QUE SAIBA O QUE ESTA FAZENDO!
#
MODE_FLAGS = $(DEBUG_FLAGS)
PROG=$(PROG_NAME).$(MODE)
CFLAGS  = -std=c++11 $(INCLUDES) $(MODE_FLAGS) $(WARNING_FLAGS) -march=$(ARCH) -mtune=$(ARCH) -w -D_GLIBCXX_USE_CXX11_ABI=0

#Isso aqui cria uma lista de objetos a partir da lista de fontes
OBJS = $(SRCS:.$(SRC_EXTENSION)=.o)
#Isso aqui cria uma lista de headers dos fontes
HEADERS = $(SRCS:.$(SRC_EXTENSION)=.h)


all: depend $(PROG)
	@echo "Make concluído em [MODO "`echo $(MODE) | tr a-z A-Z`"]"

run: all
	@./$(PROG)	$(PARAMETERS)

kdbg: all 
	@kdbg ./$(PROG) 

valgrind: all 
	@valgrind --leak-check=full --show-reachable=yes ./$(PROG) $(PARAMETERS)

$(PROG): $(OBJS)
	@echo "Linkando" $(PROG)"..."
	@$(CC) $(LDPATH) $(OBJS) $(OBJECT_LIBS) $(CFLAGS) $(LDFLAGS) $(LDLIBS)  -o $(PROG) 
	@echo "Target OS: "$(TARGET_OS)
	@echo Total de Linhas do programa: `cat $(SRCS) $(HEADERS) -s 2>/dev/null | wc -l`     	

#Isso aki verifica cada fonte do tipo SRC_EXTENSION e gera o seu .o
#Representa todas as dependencias de objetos
.$(SRC_EXTENSION).o:
	@echo "Compilando "$*"..."
	@$(CC) $(CFLAGS) -DPROG_VERSION="\"$(PROG_VERSION)\"" -c $< -o $@     


clean:
#apaga todos os arquivos objetos gerados, programa e backups
	@echo "Removendo binarios..."
	@rm -f $(OBJS) *.o clone*.log cplex.log nohup.out jobs.txt tags out.txt  *~ $(PROG_NAME).release $(PROG_NAME).debug 
	@echo "Pronto."

debug:
	@make MODE=debug MODE_FLAGS=$(DEBUG_FLAGS)

release: 
	@make MODE=release MODE_FLAGS=$(RELEASE_FLAGS)

dist: clean release

show:
	@echo $(SRCS)

depend:
	@makedepend -Y -- $(CFLAGS) -- $(SRCS) 2>/dev/null
	@rm -f makefile.bak Makefile.bak
	@echo "Dependências atualizadas."
	
# DO NOT DELETE

./utils/Convert.o: ./utils/Convert.h ./utils/Util.h
./utils/DummiePriorityQueue.o: ./utils/DummiePriorityQueue.h
./utils/DummiePriorityQueue.o: ./utils/PriorityQueue.h
./utils/Util.o: ./utils/Util.h ./utils/Convert.h
./utils/DebugPriorityQueue.o: ./utils/DebugPriorityQueue.h
./utils/DebugPriorityQueue.o: ./utils/HeapPriorityQueue.h
./utils/DebugPriorityQueue.o: ./utils/PriorityQueue.h
./utils/DebugPriorityQueue.o: ./utils/DummiePriorityQueue.h
./utils/Crono.o: ./utils/Crono.h
./utils/PriorityQueue.o: ./utils/PriorityQueue.h
./utils/HeapPriorityQueue.o: ./utils/HeapPriorityQueue.h
./utils/HeapPriorityQueue.o: ./utils/PriorityQueue.h
./zen/ZenSolver.o: ./zen/ZenSolver.h ./utils/Crono.h ./utils/Convert.h
./zen/ZenSolver.o: ./utils/Util.h
./zen/ZenSolver.o: _coin/include/coin/OsiCbcSolverInterface.hpp
./zen/ZenSolver.o: _coin/include/coin/CbcModel.hpp
./zen/ZenSolver.o: _coin/include/coin/CoinMessageHandler.hpp
./zen/ZenSolver.o: _coin/include/coin/CoinUtilsConfig.h
./zen/ZenSolver.o: _coin/include/coin/CoinPragma.hpp
./zen/ZenSolver.o: _coin/include/coin/OsiSolverInterface.hpp
./zen/ZenSolver.o: _coin/include/coin/CoinTypes.hpp
./zen/ZenSolver.o: _coin/include/coin/CoinPackedVectorBase.hpp
./zen/ZenSolver.o: _coin/include/coin/CoinError.hpp
./zen/ZenSolver.o: _coin/include/coin/CoinPackedMatrix.hpp
./zen/ZenSolver.o: _coin/include/coin/CoinShallowPackedVector.hpp
./zen/ZenSolver.o: _coin/include/coin/CoinWarmStart.hpp
./zen/ZenSolver.o: _coin/include/coin/CoinFinite.hpp
./zen/ZenSolver.o: _coin/include/coin/OsiCollections.hpp
./zen/ZenSolver.o: _coin/include/coin/OsiSolverParameters.hpp
./zen/ZenSolver.o: _coin/include/coin/OsiBranchingObject.hpp
./zen/ZenSolver.o: _coin/include/coin/OsiCuts.hpp
./zen/ZenSolver.o: _coin/include/coin/OsiRowCut.hpp
./zen/ZenSolver.o: _coin/include/coin/CoinPackedVector.hpp
./zen/ZenSolver.o: _coin/include/coin/CoinSort.hpp
./zen/ZenSolver.o: _coin/include/coin/CoinDistance.hpp
./zen/ZenSolver.o: _coin/include/coin/OsiCut.hpp
./zen/ZenSolver.o: _coin/include/coin/OsiColCut.hpp
./zen/ZenSolver.o: _coin/include/coin/CoinFloatEqual.hpp
./zen/ZenSolver.o: _coin/include/coin/CoinWarmStartBasis.hpp
./zen/ZenSolver.o: _coin/include/coin/CoinHelperFunctions.hpp
./zen/ZenSolver.o: _coin/include/coin/CbcCompareBase.hpp
./zen/ZenSolver.o: _coin/include/coin/CbcNode.hpp
./zen/ZenSolver.o: _coin/include/coin/CoinSearchTree.hpp
./zen/ZenSolver.o: _coin/include/coin/CbcBranchBase.hpp
./zen/ZenSolver.o: _coin/include/coin/CbcObject.hpp
./zen/ZenSolver.o: _coin/include/coin/CbcBranchingObject.hpp
./zen/ZenSolver.o: _coin/include/coin/CbcBranchDecision.hpp
./zen/ZenSolver.o: _coin/include/coin/CbcConsequence.hpp
./zen/ZenSolver.o: _coin/include/coin/CbcObjectUpdateData.hpp
./zen/ZenSolver.o: _coin/include/coin/CbcNodeInfo.hpp
./zen/ZenSolver.o: _coin/include/coin/CbcFullNodeInfo.hpp
./zen/ZenSolver.o: _coin/include/coin/CbcPartialNodeInfo.hpp
./zen/ZenSolver.o: _coin/include/coin/CbcConfig.h
./zen/ZenSolver.o: _coin/include/coin/CbcCountRowCut.hpp
./zen/ZenSolver.o: _coin/include/coin/CbcMessage.hpp
./zen/ZenSolver.o: _coin/include/coin/CbcEventHandler.hpp
./zen/ZenSolver.o: _coin/include/coin/ClpDualRowPivot.hpp
./zen/ZenSolver.o: _coin/include/coin/CbcStrategy.hpp ./zen/ZenCallback.h
./zen/ZenSolver.o: ./utils/Colors.h ./zen/ZenVar.h ./utils/Util.h
./zen/ZenSolver.o: _cplex/include/ilcplex/cplex.h
./zen/ZenSolver.o: _cplex/include/ilcplex/cpxconst.h
./zen/ZenSolver.o: _coin/include/coin/ThirdParty/glpk.h
./zen/ZenSolver.o: ./zen/CustomEventHandler.h
./zen/ZenSolver.o: _coin/include/coin/OsiClpSolverInterface.hpp
./zen/ZenSolver.o: _coin/include/coin/ClpSimplex.hpp
./zen/ZenSolver.o: _coin/include/coin/ClpModel.hpp
./zen/ZenSolver.o: _coin/include/coin/ClpConfig.h
./zen/ZenSolver.o: _coin/include/coin/ClpPackedMatrix.hpp
./zen/ZenSolver.o: _coin/include/coin/ClpMatrixBase.hpp
./zen/ZenSolver.o: _coin/include/coin/ClpParameters.hpp
./zen/ZenSolver.o: _coin/include/coin/ClpObjective.hpp
./zen/ZenSolver.o: _coin/include/coin/ClpSolve.hpp
./zen/ZenSolver.o: _coin/include/coin/ClpLinearObjective.hpp
./zen/ZenSolver.o: _coin/include/coin/ClpEventHandler.hpp
./zen/ZenSolver.o: _coin/include/coin/ClpNode.hpp
./zen/ZenSolver.o: _coin/include/coin/CoinIndexedVector.hpp
./zen/ZenSolver.o: ./zen/CustomOsiCbcSolverInterface.h
./zen/ZenSolver.o: ./zen/CustomOsiGlpkSolverInterface.h
./zen/ZenSolver.o: _coin/include/coin/OsiCpxSolverInterface.hpp
./zen/ZenSolver.o: _coin/include/coin/CbcCutGenerator.hpp
./zen/ZenSolver.o: _coin/include/coin/CglCutGenerator.hpp
./zen/ZenSolver.o: _coin/include/coin/CglTreeInfo.hpp
./zen/ZenSolver.o: _coin/include/coin/CbcCutModifier.hpp
./zen/ZenSolver.o: _coin/include/coin/CbcHeuristicGreedy.hpp
./zen/ZenSolver.o: _coin/include/coin/CbcHeuristic.hpp
./zen/ZenSolver.o: _coin/include/coin/CbcHeuristicDive.hpp
./zen/ZenSolver.o: _coin/include/coin/CglGomory.hpp
./zen/ZenSolver.o: _coin/include/coin/CglProbing.hpp
./zen/ZenSolver.o: _coin/include/coin/CglKnapsackCover.hpp
./zen/ZenSolver.o: _coin/include/coin/CglOddHole.hpp
./zen/ZenSolver.o: _coin/include/coin/CglClique.hpp
./zen/ZenSolver.o: _coin/include/coin/CglFlowCover.hpp
./zen/ZenSolver.o: _coin/include/coin/CglMixedIntegerRounding.hpp
./zen/ZenSolver.o: _coin/include/coin/CglMixedIntegerRounding2.hpp
./zen/CustomOsiGlpkSolverInterface.o: _coin/include/coin/ThirdParty/glpk.h
./zen/CustomOsiGlpkSolverInterface.o: _coin/include/coin/CoinError.hpp
./zen/CustomOsiGlpkSolverInterface.o: _coin/include/coin/CoinUtilsConfig.h
./zen/CustomOsiGlpkSolverInterface.o: _coin/include/coin/CoinPragma.hpp
./zen/CustomOsiGlpkSolverInterface.o: _coin/include/coin/OsiConfig.h
./zen/CustomOsiGlpkSolverInterface.o: ./zen/CustomOsiGlpkSolverInterface.h
./zen/CustomOsiGlpkSolverInterface.o: _coin/include/coin/OsiSolverInterface.hpp
./zen/CustomOsiGlpkSolverInterface.o: _coin/include/coin/CoinTypes.hpp
./zen/CustomOsiGlpkSolverInterface.o: _coin/include/coin/CoinMessageHandler.hpp
./zen/CustomOsiGlpkSolverInterface.o: _coin/include/coin/CoinPackedVectorBase.hpp
./zen/CustomOsiGlpkSolverInterface.o: _coin/include/coin/CoinPackedMatrix.hpp
./zen/CustomOsiGlpkSolverInterface.o: _coin/include/coin/CoinShallowPackedVector.hpp
./zen/CustomOsiGlpkSolverInterface.o: _coin/include/coin/CoinWarmStart.hpp
./zen/CustomOsiGlpkSolverInterface.o: _coin/include/coin/CoinFinite.hpp
./zen/CustomOsiGlpkSolverInterface.o: _coin/include/coin/OsiCollections.hpp
./zen/CustomOsiGlpkSolverInterface.o: _coin/include/coin/OsiSolverParameters.hpp
./zen/CustomOsiGlpkSolverInterface.o: _coin/include/coin/CoinWarmStartBasis.hpp
./zen/CustomOsiGlpkSolverInterface.o: _coin/include/coin/CoinSort.hpp
./zen/CustomOsiGlpkSolverInterface.o: _coin/include/coin/CoinDistance.hpp
./zen/CustomOsiGlpkSolverInterface.o: _coin/include/coin/CoinHelperFunctions.hpp
./zen/CustomOsiGlpkSolverInterface.o: _coin/include/coin/OsiRowCut.hpp
./zen/CustomOsiGlpkSolverInterface.o: _coin/include/coin/CoinPackedVector.hpp
./zen/CustomOsiGlpkSolverInterface.o: _coin/include/coin/OsiCut.hpp
./zen/CustomOsiGlpkSolverInterface.o: _coin/include/coin/OsiColCut.hpp
./src/localBranch/LocalBranching.o: ./src/localBranch/LocalBranching.h
./src/localBranch/LocalBranching.o: zen/ZenSolver.h ./utils/Crono.h
./src/localBranch/LocalBranching.o: ./utils/Convert.h ./utils/Util.h
./src/localBranch/LocalBranching.o: _coin/include/coin/OsiCbcSolverInterface.hpp
./src/localBranch/LocalBranching.o: _coin/include/coin/CbcModel.hpp
./src/localBranch/LocalBranching.o: _coin/include/coin/CoinMessageHandler.hpp
./src/localBranch/LocalBranching.o: _coin/include/coin/CoinUtilsConfig.h
./src/localBranch/LocalBranching.o: _coin/include/coin/CoinPragma.hpp
./src/localBranch/LocalBranching.o: _coin/include/coin/OsiSolverInterface.hpp
./src/localBranch/LocalBranching.o: _coin/include/coin/CoinTypes.hpp
./src/localBranch/LocalBranching.o: _coin/include/coin/CoinPackedVectorBase.hpp
./src/localBranch/LocalBranching.o: _coin/include/coin/CoinError.hpp
./src/localBranch/LocalBranching.o: _coin/include/coin/CoinPackedMatrix.hpp
./src/localBranch/LocalBranching.o: _coin/include/coin/CoinShallowPackedVector.hpp
./src/localBranch/LocalBranching.o: _coin/include/coin/CoinWarmStart.hpp
./src/localBranch/LocalBranching.o: _coin/include/coin/CoinFinite.hpp
./src/localBranch/LocalBranching.o: _coin/include/coin/OsiCollections.hpp
./src/localBranch/LocalBranching.o: _coin/include/coin/OsiSolverParameters.hpp
./src/localBranch/LocalBranching.o: _coin/include/coin/OsiBranchingObject.hpp
./src/localBranch/LocalBranching.o: _coin/include/coin/OsiCuts.hpp
./src/localBranch/LocalBranching.o: _coin/include/coin/OsiRowCut.hpp
./src/localBranch/LocalBranching.o: _coin/include/coin/CoinPackedVector.hpp
./src/localBranch/LocalBranching.o: _coin/include/coin/CoinSort.hpp
./src/localBranch/LocalBranching.o: _coin/include/coin/CoinDistance.hpp
./src/localBranch/LocalBranching.o: _coin/include/coin/OsiCut.hpp
./src/localBranch/LocalBranching.o: _coin/include/coin/OsiColCut.hpp
./src/localBranch/LocalBranching.o: _coin/include/coin/CoinFloatEqual.hpp
./src/localBranch/LocalBranching.o: _coin/include/coin/CoinWarmStartBasis.hpp
./src/localBranch/LocalBranching.o: _coin/include/coin/CoinHelperFunctions.hpp
./src/localBranch/LocalBranching.o: _coin/include/coin/CbcCompareBase.hpp
./src/localBranch/LocalBranching.o: _coin/include/coin/CbcNode.hpp
./src/localBranch/LocalBranching.o: _coin/include/coin/CoinSearchTree.hpp
./src/localBranch/LocalBranching.o: _coin/include/coin/CbcBranchBase.hpp
./src/localBranch/LocalBranching.o: _coin/include/coin/CbcObject.hpp
./src/localBranch/LocalBranching.o: _coin/include/coin/CbcBranchingObject.hpp
./src/localBranch/LocalBranching.o: _coin/include/coin/CbcBranchDecision.hpp
./src/localBranch/LocalBranching.o: _coin/include/coin/CbcConsequence.hpp
./src/localBranch/LocalBranching.o: _coin/include/coin/CbcObjectUpdateData.hpp
./src/localBranch/LocalBranching.o: _coin/include/coin/CbcNodeInfo.hpp
./src/localBranch/LocalBranching.o: _coin/include/coin/CbcFullNodeInfo.hpp
./src/localBranch/LocalBranching.o: _coin/include/coin/CbcPartialNodeInfo.hpp
./src/localBranch/LocalBranching.o: _coin/include/coin/CbcConfig.h
./src/localBranch/LocalBranching.o: _coin/include/coin/CbcCountRowCut.hpp
./src/localBranch/LocalBranching.o: _coin/include/coin/CbcMessage.hpp
./src/localBranch/LocalBranching.o: _coin/include/coin/CbcEventHandler.hpp
./src/localBranch/LocalBranching.o: _coin/include/coin/ClpDualRowPivot.hpp
./src/localBranch/LocalBranching.o: _coin/include/coin/CbcStrategy.hpp
./src/localBranch/LocalBranching.o: ./zen/ZenCallback.h ./utils/Colors.h
./src/localBranch/LocalBranching.o: ./zen/ZenVar.h ./utils/Util.h
./src/localBranch/LocalBranching.o: _cplex/include/ilcplex/cplex.h
./src/localBranch/LocalBranching.o: _cplex/include/ilcplex/cpxconst.h
./src/localBranch/LocalBranching.o: _coin/include/coin/ThirdParty/glpk.h
./src/localBranch/LocalBranching.o: ./include/Instance.h ./include/Nurse.h
./src/Solution.o: ./include/Solution.h ./include/Instance.h ./include/Nurse.h
./src/Solution.o: ./include/Fulfilled.h zen/ZenSolver.h ./utils/Crono.h
./src/Solution.o: ./utils/Convert.h ./utils/Util.h
./src/Solution.o: _coin/include/coin/OsiCbcSolverInterface.hpp
./src/Solution.o: _coin/include/coin/CbcModel.hpp
./src/Solution.o: _coin/include/coin/CoinMessageHandler.hpp
./src/Solution.o: _coin/include/coin/CoinUtilsConfig.h
./src/Solution.o: _coin/include/coin/CoinPragma.hpp
./src/Solution.o: _coin/include/coin/OsiSolverInterface.hpp
./src/Solution.o: _coin/include/coin/CoinTypes.hpp
./src/Solution.o: _coin/include/coin/CoinPackedVectorBase.hpp
./src/Solution.o: _coin/include/coin/CoinError.hpp
./src/Solution.o: _coin/include/coin/CoinPackedMatrix.hpp
./src/Solution.o: _coin/include/coin/CoinShallowPackedVector.hpp
./src/Solution.o: _coin/include/coin/CoinWarmStart.hpp
./src/Solution.o: _coin/include/coin/CoinFinite.hpp
./src/Solution.o: _coin/include/coin/OsiCollections.hpp
./src/Solution.o: _coin/include/coin/OsiSolverParameters.hpp
./src/Solution.o: _coin/include/coin/OsiBranchingObject.hpp
./src/Solution.o: _coin/include/coin/OsiCuts.hpp
./src/Solution.o: _coin/include/coin/OsiRowCut.hpp
./src/Solution.o: _coin/include/coin/CoinPackedVector.hpp
./src/Solution.o: _coin/include/coin/CoinSort.hpp
./src/Solution.o: _coin/include/coin/CoinDistance.hpp
./src/Solution.o: _coin/include/coin/OsiCut.hpp
./src/Solution.o: _coin/include/coin/OsiColCut.hpp
./src/Solution.o: _coin/include/coin/CoinFloatEqual.hpp
./src/Solution.o: _coin/include/coin/CoinWarmStartBasis.hpp
./src/Solution.o: _coin/include/coin/CoinHelperFunctions.hpp
./src/Solution.o: _coin/include/coin/CbcCompareBase.hpp
./src/Solution.o: _coin/include/coin/CbcNode.hpp
./src/Solution.o: _coin/include/coin/CoinSearchTree.hpp
./src/Solution.o: _coin/include/coin/CbcBranchBase.hpp
./src/Solution.o: _coin/include/coin/CbcObject.hpp
./src/Solution.o: _coin/include/coin/CbcBranchingObject.hpp
./src/Solution.o: _coin/include/coin/CbcBranchDecision.hpp
./src/Solution.o: _coin/include/coin/CbcConsequence.hpp
./src/Solution.o: _coin/include/coin/CbcObjectUpdateData.hpp
./src/Solution.o: _coin/include/coin/CbcNodeInfo.hpp
./src/Solution.o: _coin/include/coin/CbcFullNodeInfo.hpp
./src/Solution.o: _coin/include/coin/CbcPartialNodeInfo.hpp
./src/Solution.o: _coin/include/coin/CbcConfig.h
./src/Solution.o: _coin/include/coin/CbcCountRowCut.hpp
./src/Solution.o: _coin/include/coin/CbcMessage.hpp
./src/Solution.o: _coin/include/coin/CbcEventHandler.hpp
./src/Solution.o: _coin/include/coin/ClpDualRowPivot.hpp
./src/Solution.o: _coin/include/coin/CbcStrategy.hpp ./zen/ZenCallback.h
./src/Solution.o: ./utils/Colors.h ./zen/ZenVar.h ./utils/Util.h
./src/Solution.o: _cplex/include/ilcplex/cplex.h
./src/Solution.o: _cplex/include/ilcplex/cpxconst.h
./src/Solution.o: _coin/include/coin/ThirdParty/glpk.h
./src/Instance.o: ./include/Instance.h ./include/Nurse.h
./src/main.o: _coin/include/coin/ThirdParty/glpk.h _opt/opt.h
./src/main.o: _coin/include/coin/OsiCpxSolverInterface.hpp
./src/main.o: _coin/include/coin/OsiSolverInterface.hpp
./src/main.o: _coin/include/coin/CoinTypes.hpp
./src/main.o: _coin/include/coin/CoinUtilsConfig.h
./src/main.o: _coin/include/coin/CoinMessageHandler.hpp
./src/main.o: _coin/include/coin/CoinPragma.hpp
./src/main.o: _coin/include/coin/CoinPackedVectorBase.hpp
./src/main.o: _coin/include/coin/CoinError.hpp
./src/main.o: _coin/include/coin/CoinPackedMatrix.hpp
./src/main.o: _coin/include/coin/CoinShallowPackedVector.hpp
./src/main.o: _coin/include/coin/CoinWarmStart.hpp
./src/main.o: _coin/include/coin/CoinFinite.hpp
./src/main.o: _coin/include/coin/OsiCollections.hpp
./src/main.o: _coin/include/coin/OsiSolverParameters.hpp
./src/main.o: _coin/include/coin/CoinWarmStartBasis.hpp
./src/main.o: _coin/include/coin/CoinSort.hpp
./src/main.o: _coin/include/coin/CoinDistance.hpp
./src/main.o: _coin/include/coin/CoinHelperFunctions.hpp
./src/main.o: _coin/include/coin/OsiColCut.hpp
./src/main.o: _coin/include/coin/CoinPackedVector.hpp
./src/main.o: _coin/include/coin/OsiCut.hpp _coin/include/coin/OsiRowCut.hpp
./src/main.o: ./utils/Crono.h ./utils/Convert.h ./utils/Util.h ./utils/Util.h
./src/main.o: ./utils/DummiePriorityQueue.h ./utils/PriorityQueue.h
./src/main.o: ./utils/Combination.h zen/ZenSolver.h
./src/main.o: _coin/include/coin/OsiCbcSolverInterface.hpp
./src/main.o: _coin/include/coin/CbcModel.hpp
./src/main.o: _coin/include/coin/OsiBranchingObject.hpp
./src/main.o: _coin/include/coin/OsiCuts.hpp
./src/main.o: _coin/include/coin/CoinFloatEqual.hpp
./src/main.o: _coin/include/coin/CbcCompareBase.hpp
./src/main.o: _coin/include/coin/CbcNode.hpp
./src/main.o: _coin/include/coin/CoinSearchTree.hpp
./src/main.o: _coin/include/coin/CbcBranchBase.hpp
./src/main.o: _coin/include/coin/CbcObject.hpp
./src/main.o: _coin/include/coin/CbcBranchingObject.hpp
./src/main.o: _coin/include/coin/CbcBranchDecision.hpp
./src/main.o: _coin/include/coin/CbcConsequence.hpp
./src/main.o: _coin/include/coin/CbcObjectUpdateData.hpp
./src/main.o: _coin/include/coin/CbcNodeInfo.hpp
./src/main.o: _coin/include/coin/CbcFullNodeInfo.hpp
./src/main.o: _coin/include/coin/CbcPartialNodeInfo.hpp
./src/main.o: _coin/include/coin/CbcConfig.h
./src/main.o: _coin/include/coin/CbcCountRowCut.hpp
./src/main.o: _coin/include/coin/CbcMessage.hpp
./src/main.o: _coin/include/coin/CbcEventHandler.hpp
./src/main.o: _coin/include/coin/ClpDualRowPivot.hpp
./src/main.o: _coin/include/coin/CbcStrategy.hpp ./zen/ZenCallback.h
./src/main.o: ./utils/Colors.h ./zen/ZenVar.h _cplex/include/ilcplex/cplex.h
./src/main.o: _cplex/include/ilcplex/cpxconst.h
./src/main.o: ./src/localBranch/LocalBranching.h ./include/Instance.h
./src/main.o: ./include/Nurse.h ./_opt/opt.h ./include/Instance.h
./src/main.o: ./include/Solution.h ./include/Fulfilled.h
./src/main.o: ./include/Neighborhood.h
./src/Neighborhood.o: ./include/Neighborhood.h ./include/Instance.h
./src/Neighborhood.o: ./include/Nurse.h ./include/Solution.h
./src/Neighborhood.o: ./include/Fulfilled.h zen/ZenSolver.h ./utils/Crono.h
./src/Neighborhood.o: ./utils/Convert.h ./utils/Util.h
./src/Neighborhood.o: _coin/include/coin/OsiCbcSolverInterface.hpp
./src/Neighborhood.o: _coin/include/coin/CbcModel.hpp
./src/Neighborhood.o: _coin/include/coin/CoinMessageHandler.hpp
./src/Neighborhood.o: _coin/include/coin/CoinUtilsConfig.h
./src/Neighborhood.o: _coin/include/coin/CoinPragma.hpp
./src/Neighborhood.o: _coin/include/coin/OsiSolverInterface.hpp
./src/Neighborhood.o: _coin/include/coin/CoinTypes.hpp
./src/Neighborhood.o: _coin/include/coin/CoinPackedVectorBase.hpp
./src/Neighborhood.o: _coin/include/coin/CoinError.hpp
./src/Neighborhood.o: _coin/include/coin/CoinPackedMatrix.hpp
./src/Neighborhood.o: _coin/include/coin/CoinShallowPackedVector.hpp
./src/Neighborhood.o: _coin/include/coin/CoinWarmStart.hpp
./src/Neighborhood.o: _coin/include/coin/CoinFinite.hpp
./src/Neighborhood.o: _coin/include/coin/OsiCollections.hpp
./src/Neighborhood.o: _coin/include/coin/OsiSolverParameters.hpp
./src/Neighborhood.o: _coin/include/coin/OsiBranchingObject.hpp
./src/Neighborhood.o: _coin/include/coin/OsiCuts.hpp
./src/Neighborhood.o: _coin/include/coin/OsiRowCut.hpp
./src/Neighborhood.o: _coin/include/coin/CoinPackedVector.hpp
./src/Neighborhood.o: _coin/include/coin/CoinSort.hpp
./src/Neighborhood.o: _coin/include/coin/CoinDistance.hpp
./src/Neighborhood.o: _coin/include/coin/OsiCut.hpp
./src/Neighborhood.o: _coin/include/coin/OsiColCut.hpp
./src/Neighborhood.o: _coin/include/coin/CoinFloatEqual.hpp
./src/Neighborhood.o: _coin/include/coin/CoinWarmStartBasis.hpp
./src/Neighborhood.o: _coin/include/coin/CoinHelperFunctions.hpp
./src/Neighborhood.o: _coin/include/coin/CbcCompareBase.hpp
./src/Neighborhood.o: _coin/include/coin/CbcNode.hpp
./src/Neighborhood.o: _coin/include/coin/CoinSearchTree.hpp
./src/Neighborhood.o: _coin/include/coin/CbcBranchBase.hpp
./src/Neighborhood.o: _coin/include/coin/CbcObject.hpp
./src/Neighborhood.o: _coin/include/coin/CbcBranchingObject.hpp
./src/Neighborhood.o: _coin/include/coin/CbcBranchDecision.hpp
./src/Neighborhood.o: _coin/include/coin/CbcConsequence.hpp
./src/Neighborhood.o: _coin/include/coin/CbcObjectUpdateData.hpp
./src/Neighborhood.o: _coin/include/coin/CbcNodeInfo.hpp
./src/Neighborhood.o: _coin/include/coin/CbcFullNodeInfo.hpp
./src/Neighborhood.o: _coin/include/coin/CbcPartialNodeInfo.hpp
./src/Neighborhood.o: _coin/include/coin/CbcConfig.h
./src/Neighborhood.o: _coin/include/coin/CbcCountRowCut.hpp
./src/Neighborhood.o: _coin/include/coin/CbcMessage.hpp
./src/Neighborhood.o: _coin/include/coin/CbcEventHandler.hpp
./src/Neighborhood.o: _coin/include/coin/ClpDualRowPivot.hpp
./src/Neighborhood.o: _coin/include/coin/CbcStrategy.hpp ./zen/ZenCallback.h
./src/Neighborhood.o: ./utils/Colors.h ./zen/ZenVar.h ./utils/Util.h
./src/Neighborhood.o: _cplex/include/ilcplex/cplex.h
./src/Neighborhood.o: _cplex/include/ilcplex/cpxconst.h
./src/Neighborhood.o: _coin/include/coin/ThirdParty/glpk.h
