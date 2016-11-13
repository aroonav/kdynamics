all: fuzzy.cpp
	g++ fuzzy.cpp -o fuzzy
	@echo "Run ./fuzzy to run fuzzy inference system."
clean:
	rm -f fuzzy
