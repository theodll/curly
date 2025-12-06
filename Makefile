all:
	mkdir -p build
	clang src/curly.c -o build/curly


clean:
	rm -rf build
