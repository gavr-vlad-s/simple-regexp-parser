LINKER        = g++
LINKERFLAGS   =  
COMPILER      = g++
COMPILERFLAGS =  -std=c++14 -Wall -g
BIN           = test-simple-parser
LIBS          = -lboost_filesystem -lboost_system
vpath %.cpp src
vpath %.o build
OBJ           = test-simple-parser.o simple_regex_parser.o get_init_state.o print_char32.o search_char.o sets_for_classes.o print_commands.o expr_scaner.o error_count.o aux_expr_scaner.o char_conv.o file_contents.o char_trie.o fsize.o
LINKOBJ       = build/test-simple-parser.o build/simple_regex_parser.o build/get_init_state.o build/print_char32.o build/search_char.o build/sets_for_classes.o build/print_commands.o build/expr_scaner.o build/error_count.o build/aux_expr_scaner.o build/char_conv.o build/file_contents.o build/char_trie.o build/fsize.o

.PHONY: all all-before all-after clean clean-custom

all: all-before $(BIN) all-after

clean: clean-custom 
	rm -f ./build/*.o
	rm -f ./build/$(BIN)

.cpp.o:
	$(COMPILER) -c $< -o $@ $(COMPILERFLAGS) 
	mv $@ ./build

$(BIN):$(OBJ)
	$(LINKER) -o $(BIN) $(LINKOBJ) $(LIBS) $(LINKERFLAGS)
	mv $(BIN) ./build