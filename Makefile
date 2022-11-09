.PHONY: build
build:
	@cd build && cmake .. && make
run: build
	@build/commutator

clib:
	@clib install