.PHONY: build test ci format format-check sanitize

ifeq ($(shell uname -s),Darwin)
# Apple Clang's AddressSanitizer does not support LeakSanitizer.
SANITIZER_OPTIONS := halt_on_error=1
else
SANITIZER_OPTIONS := detect_leaks=1:halt_on_error=1
endif

build:
	$(MAKE) -C core build

test: build
	./core/nani core/tests/test.nani

clean:
	$(MAKE) -C core clean

ci: test format-check

sanitize: clean
	ASAN_OPTIONS=$(SANITIZER_OPTIONS) $(MAKE) SANITIZE=1 test

format:
	clang-format -i core/*.c core/*.h

format-check:
	clang-format --dry-run --Werror core/*.c core/*.h
