.PHONY: all generate build clean

# `tree-sitter build` was added after tree-sitter CLI 0.20.8. Compile the
# generated parser directly so that only `tree-sitter generate` is required.
ifeq ($(shell uname -s),Darwin)
SHARED_LIBRARY_FLAGS := -dynamiclib
else
SHARED_LIBRARY_FLAGS := -shared
endif

all: build

generate:
	tree-sitter generate --no-bindings

build: generate
	cc -fPIC $(SHARED_LIBRARY_FLAGS) -Isrc -o nani.so src/parser.c

clean:
	rm -f nani.so
	rm -rf src/
	rm package.json
