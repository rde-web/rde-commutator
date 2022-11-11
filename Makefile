APP=rde-commutator
VERSION=$(shell git branch --show-current)

ifeq ($(VERSION),master)
VERSION=latest
endif

.PHONY: build
build:
	@mkdir -p build
	@cd build && cmake .. && make

run: build
	@build/commutator

clib:
	@clib install

build.docker:
	@docker build . -t $(APP):$(VERSION)