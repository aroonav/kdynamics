all: fuzzy.cpp
	g++ fuzzy.cpp -o fuzzy
	@echo "Run ./fuzzy to run fuzzy inference system."
new: fuzzy_originalModel.cpp
	g++ fuzzy_originalModel.cpp -o fuzzy_old
	@echo "Run ./fuzzy_old to run fuzzy inference system."
debug: fuzzy.cpp
	g++ fuzzy.cpp -g -o fuzzy
	@echo "Run gdb fuzzy to debug."
clean:
	rm -f fuzzy fuzzy_new
