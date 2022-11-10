.PHONY: build
build:
	@mkdir -p build
	@cd build && cmake .. && make
run: build
	@build/commutator

clib:
	@clib install