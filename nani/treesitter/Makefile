.PHONY: all generate build clean

all: build

generate:
	tree-sitter generate --no-bindings

build: generate
	tree-sitter build -o nani.so

clean:
	rm -f nani.so
	rm -rf src/
	rm package.json
