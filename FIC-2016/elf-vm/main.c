#include <stdio.h>

// Registers
int ip = 0;	// EIP
int sp = -1;	// ESP
typedef enum {
	A,	// EAX
	B,	// EBX
	C,	// ECX
	D,	// EDX
	Z,	// Zero Flag
	X,	// eXit Flag
	NUM_OF_REGS
} Registers;
// Store values of registers
int registers[NUM_OF_REGS];

// Stack
int stack[256];

// Variables
char pass[64];
int running = 1;
char buffer[70] = 
	"\x19\x0a\x41\x6f\x6e\x5a\x30\x4f\x74\x0a\x68\x08\x65\x09\x72\x64\x5f\x5f"
	"\x46\x55\x4c\x48\x34\x57\x47\x57\x52\x6f\x5e\x50\x53\x5f\x51\x08\x55\x5a"
	"\x64\x4e\x58\x7d\x4b\x5e\x57\x52\x53\x0f\x5f\x5c\x5c\x4e\x5e\x1a\x42\x42";

typedef enum {
	PSH,	// PUSH on the stack
	ADD,	// ADD the to value on the top of the stack
	POPI,	// POP integer in variable
	POPS,	// POP string in variable
	READ,	// READ from input
	SET,	// SET the value of a register
	CMP, 	// Compare two char
	JUMP,	// Unconditional Jump
	JMPZ,	// Jump if Z Flag set
	JMPX,	// Jump if X Flag set
	CSTR,	// Compare String
	DEBUG,	// Print text for debug
	HLT	// Ending the program
} InstructionSet;

const int program[] = {
	// Read pass from user and set registers
	PSH, (int)"Password : ",
	POPS,
	SET, A, (int)pass,
	SET, B, sizeof(pass),
	SET, C, (int)buffer,
	READ,

	// While i < 12 ...
	PSH, 12,
	PSH, 0,
	CMP,
	JMPZ, 6,
	
	// ...compare 2 strings one char at once
	CSTR,	
	PSH, 1,
	ADD,
	JUMP, -8,

	// If X flag set : jump to the bad boy message
	JMPX, 4,
	PSH, (int)"Well done!\n",
	POPS,
	HLT,
	
	PSH, (int)"Wrong pass!\n",
	POPS,
	HLT
};

int fetch() {
	return program[ip];
}

int eval(int instruction) {
	switch(instruction) {
		case PSH:{
			sp++;
			stack[sp] = program[++ip];
			break;
		}
		case CMP:{
			int b = stack[sp--];
			int a = stack[sp--];
			if(a == b){
				registers[Z] = 1;
			}
			sp = sp + 2;
			break;
		}
		case JMPZ:{
			if(registers[Z] == 1){
				ip = ip + program[ip+1];
			}
			ip += 1;
			break;
		}
		case JMPX:{
			if(registers[X] == 1){
				ip = ip + program[ip+1];
			}
			ip += 1;
			break;
		}
		case ADD:{
			int b = stack[sp--];
			int a = stack[sp--];
			stack[++sp] = a + b;
			break;
		}
		case POPI:{
			int val_popped = stack[sp--];
			break;
		}
		case POPS:{
			char *val_popped;
			val_popped = (char *)stack[sp--];
			fputs(val_popped, stdout);
			break;
		}
		case SET:{
			registers[program[ip+1]] = program[ip+2];
			ip = ip+2;
			break;
		}
		case READ:{
			fgets((char *)registers[A], registers[B], stdin);
			break;
		}
		case CSTR:{
			char string1 = ((char *)registers[A])[stack[sp]];
			char string2 = ((char *)registers[C])[ (stack[sp]+1)*2 ];
			if(string1 != string2){
				registers[X] = 1;
			}
			break;
		}
		case HLT:{
			running = 0;
			break;
		}
		case DEBUG:{
			puts("Good until now!");
			break;
		}
		case JUMP:{
			int var = ip + program[ip+1];
			ip = var;
			break;
		}
	}
}

int main() {
	while(running) {
		eval(fetch());
		ip++;
	}
}
