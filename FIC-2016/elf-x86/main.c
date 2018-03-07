// Crackme N°1 - FIC 2016

#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <sys/ptrace.h>
#include <signal.h>
#include <stdlib.h>

void decrypt();
void sighandler(int signum);
unsigned int sum;
unsigned int checksum;

void get_sum() {
	unsigned char *start = "0x08048a3d";
	int size = 900;
	int j;
	for (j=0; j<size; ++j) {
		sum += start[j]*start[j]*2048;
	}
}

void check() {
	unsigned char *start = "0x08048a3d";
	int size = 900;
	int j;
	for (j=0; j<size; ++j) {
		checksum += start[j]*start[j]*2048;
	}
}

int my_puts(char *s) {
	int i;
	for (i=0; s[i]; ++i);
	if (write(1, s, i) == 2) {
		perror("write");
		return -1;
	}
	return 0;
}

void verif(char *pass) {
	char buffer[70] = 
	"\x19\x0a\x09\x6f\x59\x5a\x53\x4f\x58\x0a\x5e\x08\x48\x09\x51\x64\x51\x5f"
	"\x0a\x55\x50\x48\x54\x57\x64\x57\x52\x6f\x5e\x50\x53\x5f\x51\x08\x55\x5a"
	"\x64\x4e\x58\x7d\x4b\x5e\x57\x52\x53\x0f\x5f\x5c\x5c\x4e\x5e\x1a\x42\x42";
	int i;
	for (i=0; pass[i]; ++i) {
		if ((int)pass[i] != '\n') {	
			if ( (((int)pass[i]) ^ 0x3B) != (int)buffer[(i+1)*3] ) {
				my_puts("Bad password !\n");
				exit(1);
			}
		}
	}
	my_puts("Connected !\n");
	exit(1);
}

void decrypt_2() {
	void (*start_addr)() = verif;
	void (*end_addr)() = decrypt_2;
	unsigned char *start = (unsigned char *)start_addr;
	unsigned char *end = (unsigned char *)end_addr;
	int size = (int)(end - start);
	int i;
	for (i=0; i<size; ++i) {
		start[i] = start[i] ^ 0xDE;
	}
}

void sighandler(int signum) {
	char pass[128] = "";
	my_puts("Password : ");
	fgets(pass, sizeof(pass), stdin);
	check();
	if (sum == checksum) {
		decrypt_2();
		verif(pass);
	}
	exit(1);
}

int main() {
	decrypt();
	get_sum();
	signal(SIGTRAP, sighandler);
	asm("int3");
	return 0;
}

void decrypt() {
	void (*start_addr)() = get_sum;
	int (*end_addr)() = main;
	unsigned char *start = (unsigned char *)start_addr;
	unsigned char *end = (unsigned char *)end_addr;
	int size = (int)(end - start);
	int i;
	for (i=0; i<size; ++i) {
		start[i] = start[i] ^ 0xAC;
	}
}
