all: fuzzy.cpp
	g++ fuzzy.cpp -o fuzzy
	@echo "Run ./fuzzy to run fuzzy inference system."
debug: fuzzy.cpp
	g++ fuzzy.cpp -g -o fuzzy
	@echo "Run gdb fuzzy to debug."
clean:
	rm -f fuzzy
