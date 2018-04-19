projectPath := ../paillier
INCLUDES := $(projectPath)
INCLUDE_FLAGS:=$(addprefix -I, $(INCLUDES))
#LIBS:=ntl gmp m ssl crypto
#LIBFLAGS:=$(addprefix -l, $(LIBS))
SOURCES := $(wildcard *.cpp)
TARGETS := $(SOURCES:.cpp=)
PAILLIER_SOURCES := $(wildcard $(projectPath)/*.cpp)
INTERPRETER_SOURCES := $(wildcard $(projectPath)/interpreter/*.cpp)

interpreter-test : interpreter-test.cpp $(INTERPRETER_SOURCES)
	g++ $^ -o $@ $(INCLUDE_FLAGS)

.PHONY : test
test : $(TARGETS)
	$(foreach test, $(TARGETS), ./$(test);)

.PHONY : clean
clean :
	rm interpreter-test
