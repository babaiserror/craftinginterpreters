# https://github.com/deltegui/Clox/blob/master/clox/Makefile

CXX = gcc
OUTPUT = ./build/clox
LIBS = -lm

all: build

build-folder:
	if [ ! -d "./build" ]; then mkdir ./build; fi

build: build-folder
	$(CXX) ./*.c $(LIBS) -o $(OUTPUT)

debug: build-folder
	$(CXX) ./*.c $(libs) -g -o $(OUTPUT)

clean:
	rm -rf ./build
