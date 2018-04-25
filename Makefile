projectPath := ../paillier
INCLUDES := $(projectPath)
INCLUDE_FLAGS:=$(addprefix -I, $(INCLUDES))
LIBS:=ntl gmp m ssl crypto
LIBFLAGS:=$(addprefix -l, $(LIBS))
SOURCES := $(wildcard *.cpp)
TARGETS := $(SOURCES:.cpp=)
PAILLIER_SOURCES := $(wildcard $(projectPath)/paillier/*.cpp)
INTERPRETER_SOURCES := $(wildcard $(projectPath)/interpreter/*.cpp)

interpreter-test : interpreter-test.cpp $(INTERPRETER_SOURCES)
	g++ $^ -o $@ $(INCLUDE_FLAGS)

paillier-test : paillier-test.cpp $(PAILLIER_SOURCES)
	g++ -g $^ -o $@ $(INCLUDE_FLAGS) $(LIBFLAGS)

.PHONY : test
test : $(TARGETS)
	$(foreach test, $(TARGETS), ./$(test);)

.PHONY : clean
clean :
	rm interpreter-test
