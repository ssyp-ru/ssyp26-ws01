.PHONY: build test ci

build:
	$(MAKE) -C core build

test: build
	./core/nani core/tests/test.nani

clean:
	$(MAKE) -C core clean

ci: test
