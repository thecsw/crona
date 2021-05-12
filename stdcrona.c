#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include <inttypes.h>

typedef uint8_t byte;

void printBool(byte c)
{
	if (c == 0) {
		fprintf(stdout, "false");
	} else {
		fprintf(stdout, "true");
	}
	fflush(stdout);
}

void printChar(char c)
{
	fprintf(stdout, "%c", c);
	fflush(stdout);
}

void printStr(char *s)
{
	fprintf(stdout, "%s", s);
	fflush(stdout);
}

void printInt(long int num)
{
	fprintf(stdout, "%ld", num);
	fflush(stdout);
}

void printByte(byte num)
{
	fprintf(stdout, "%d", num);
	fflush(stdout);
}

void printString(const char *str)
{
	printf("%s", str);
	fflush(stdout);
}

byte getBool()
{
	char c;
	scanf("%c", &c);
	getchar(); // Consume trailing newline
	if (c == '0') {
		return 0;
	} else {
		return 1;
	}
}

long int getInt()
{
	char buffer[32];
	fgets(buffer, 32, stdin);
	long int res = atol(buffer);
	return res;
}

char getChar()
{
	char c;
	c = getchar();
	if (c != '\n' && c != 0x10) {
		char next = getchar(); // Consume trailing newline
		if (next != '\n' && next != 0x10) {
			ungetc(next, stdin);
		}
	} else {
		//user didn't enter anything before newline
	}
	return c;
}

byte getByte()
{
	char buffer[8];
	fgets(buffer, 8, stdin);
	int res = 0;
	if (buffer[0] == '0' && (buffer[1] == 'x' || buffer[1] == 'X')) { // hex
		sscanf(buffer, "%x", &res);
	} else if (buffer[0] == '0') { // octal
		sscanf(buffer, "%o", &res);
	} else { // int
		res = atoi(buffer);
	}
	return (byte)res;
}

void getStr(char *buf, long int len)
{
	fgets(buf, len, stdin);
	if (buf[strlen(buf) - 1] != '\n') {
		fflush(stdin);
	}
	buf[strlen(buf) - 1] = '\0';
}
