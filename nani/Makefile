.PHONY: build test ci format format-check

build:
	$(MAKE) -C core build

test: build
	./core/nani core/tests/test.nani

clean:
	$(MAKE) -C core clean

ci: test format-check

format:
	clang-format -i core/*.c core/*.h

format-check:
	clang-format --dry-run --Werror core/*.c core/*.h
