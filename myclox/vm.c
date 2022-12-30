#include <stdio.h>

#include "common.h"
#include "compiler.h"
#include "debug.h"
#include "vm.h"
#include "chunk.h"
#include "value.h"


VM vm;

static void resetStack() { vm.stackTop = vm.stack; }

void initVM() {
    resetStack();  
}

void freeVM() {}

void push(Value value) {
    *vm.stackTop = value;
    vm.stackTop++;
}

Value pop() {
    vm.stackTop--;
    return *vm.stackTop;
}

Value peek() { return *(vm.stackTop - 1); }

void replaceTop(Value value) {
    *(vm.stackTop - 1) = value;
}

static InterpretResult run() {
#define READ_BYTE() (*vm.ip++)
#define READ_CONSTANT() (vm.chunk->constants.values[READ_BYTE()])
#define READ_LONG_CONSTANT() (vm.chunk->constants.values[READ_BYTE() | \
			      (READ_BYTE() << 8) | (READ_BYTE() << 16)])
#define BINARY_OP(op) \
    do { \
	double b = pop(); \
        replaceTop(peek() op b); \
    } while (false)
    
    for (;;) {
#ifdef DEBUG_TRACE_EXECUTION
	printf("          ");
	for (Value* slot = vm.stack; slot < vm.stackTop; slot++) {
	    printf("[ ");
	    printValue(*slot);
	    printf(" ]");
	}
	printf("\n");
	disassembleInstruction(vm.chunk, (int)(vm.ip - vm.chunk->code));
#endif
	uint8_t instruction;
	switch (instruction = READ_BYTE()) {
	case OP_CONSTANT: {
	    Value constant = READ_CONSTANT();
	    push(constant);
	    break;
	}
	case OP_CONSTANT_LONG: {
	    Value constant = READ_LONG_CONSTANT();
	    push(constant);
	    break;
	}
	case OP_ADD:      BINARY_OP(+); break;
	case OP_SUBTRACT: BINARY_OP(-); break;
	case OP_MULTIPLY: BINARY_OP(*); break;
	case OP_DIVIDE:   BINARY_OP(/); break;
	case OP_NEGATE:   replaceTop(-peek()); break;
	case OP_RETURN: {
	    printValue(pop());
	    printf("\n");
	    return INTERPRET_OK;
	}
	}
    }

#undef READ_BYTE
#undef READ_CONSTANT
#undef READ_LONG_CONSTANT
#undef BINARY_OP
}

InterpretResult interpret(const char* source) {
    compile(source);
    return INTERPRET_OK;
}
