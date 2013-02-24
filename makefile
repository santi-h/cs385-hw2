COMPILER_OPTIONS = -Wall -g -Wno-format-security
COMPILE = @g++ -c $(COMPILER_OPTIONS) -o$@ $<
COMPILE_N_LINK = @g++ $(COMPILER_OPTIONS) -o$@ $<

OBJECTS =	Command.o \
			CFuncs.o \
			Misc.o		

master: master.cpp $(OBJECTS)
	@echo building [$@]
	$(COMPILE_N_LINK) $(OBJECTS)

%.o: %.cpp %.h
	@echo compiling [$@]
	$(COMPILE)

clean:
	@rm -f ./master
	@rm -f ./*.o



