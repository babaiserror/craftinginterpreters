#include "common.h"
#include "chunk.h"
#include "debug.h"
#include "value.h"

int main(int argc, const char* argv[]) {
    Chunk chunk;
    initChunk(&chunk);

    for (int i = 0; i < 300; i++) {
	writeConstant(&chunk, 1.2, i/2);
    }

    writeChunk(&chunk, OP_RETURN, 500);

    disassembleChunk(&chunk, "test chunk");
    freeChunk(&chunk);
    return 0;
}
