COMPILER_OPTIONS = -g -Wall
COMPILE = @g++ -c $(COMPILER_OPTIONS) -o$@ $<
COMPILE_N_LINK = @g++ $(COMPILER_OPTIONS) -o$@ $<

OBJECTS =	Command.o \
			CFuncs.o \
			Misc.o		

all: master worker

master: master.cpp $(OBJECTS)
	@echo building [$@]
	$(COMPILE_N_LINK) $(OBJECTS)

worker: worker.cpp $(OBJECTS)
	@echo building [$@]
	$(COMPILE_N_LINK) $(OBJECTS)

%.o: %.cpp %.h
	@echo compiling [$@]
	$(COMPILE)

clean:
	@rm -f ./master
	@rm -f ./worker
	@rm -f ./*.o



