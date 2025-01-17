#include <stdint.h>
#include <stdlib.h>

#include "chunk.h"
#include "memory.h"
#include "vm.h"
#include "value.h"

void initChunk(Chunk* chunk) {
    chunk->count = 0;
    chunk->capacity = 0;
    chunk->code = NULL;
    chunk->lineCount = 0;
    chunk->lineCapacity = 0;
    chunk->lines = NULL;
    initValueArray(&chunk->constants);
}

void freeChunk(Chunk *chunk) {
    FREE_ARRAY(uint8_t, chunk->code, chunk->capacity);
    FREE_ARRAY(LineStart, chunk->lines, chunk->lineCapacity);
    freeValueArray(&chunk->constants);
    initChunk(chunk);
}

void writeChunk(Chunk* chunk, uint8_t byte, int line) {
    if (chunk->capacity < chunk->count + 1) {
	int oldCapacity = chunk->capacity;
	chunk->capacity = GROW_CAPACITY(oldCapacity);
	chunk->code = GROW_ARRAY(uint8_t, chunk->code,
			 oldCapacity, chunk->capacity);
    }
    
    chunk->code[chunk->count] = byte;
    chunk->count++;
    
    if (chunk->lineCount > 0 &&
	chunk->lines[chunk->lineCount - 1].line == line) {
	return;
    }

    if (chunk->lineCapacity < chunk->lineCount + 1) {
	int oldCapacity = chunk->lineCapacity;
	chunk->lineCapacity = GROW_CAPACITY(oldCapacity);
	chunk->lines = GROW_ARRAY(LineStart, chunk->lines,
			 oldCapacity, chunk->lineCapacity);
    }

    LineStart* lineStart = &chunk->lines[chunk->lineCount];
    lineStart->offset = chunk->count - 1;
    lineStart->line = line;
    chunk->lineCount++;
}

int addConstant(Chunk *chunk, Value value) {
    push(value);
    writeValueArray(&chunk->constants, value);
    pop(value);
    return chunk->constants.count - 1;
}

void writeConstant(Chunk *chunk, Value value, int line) {
    int index = addConstant(chunk, value);

    if (index < 256) {
	writeChunk(chunk, OP_CONSTANT, line);
	writeChunk(chunk, (uint8_t)index, line);
    } else {
	writeChunk(chunk, OP_CONSTANT_LONG, line);
	writeChunk(chunk, (uint8_t)(index & 0xff), line);
	writeChunk(chunk, (uint8_t)((index >> 8) & 0xff), line);
	writeChunk(chunk, (uint8_t)((index >> 16) & 0xff), line);
    }
}

int getLine(Chunk *chunk, int instructionOffset) {
    int lo = 0;
    int hi = chunk->lineCount;

    for(;;) {
	int mid = lo + (hi - lo) / 2;

	LineStart lineStart = chunk->lines[mid];
	if (instructionOffset < lineStart.offset) {
	    hi = mid;
	} else if (mid == chunk->lineCount - 1 ||
		   instructionOffset < chunk->lines[mid+1].offset) {
	    return lineStart.line;
	} else {
	    lo = mid + 1;
	}
    }
}
