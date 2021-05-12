#include <fstream>
#include <string.h>
#include <string>
#include <cstdlib>

#include "errors.hpp"
#include "scanner.hpp"
#include "ast.hpp"
#include "name_analysis.hpp"
#include "type_analysis.hpp"
#include "3ac.hpp"

#include "macros.h"

#define OK 0
#define NOT_OK 1

using namespace crona;

static void usageAndDie()
{
	std::cerr << "Usage: cronac <infile> <options>\n"
		  << " [-t <tokensFile>]: Output tokens to <tokensFile>\n"
		  << " [-p]: Parse the input to check syntax\n"
		  << " [-u <unparseFile>]: Unparse to <unparseFile>\n"
		  << " [-n <nameFile]: Output name analysis to <namesFile>\n"
		  << " [-c]: Do type checking\n"
		  << " [-a <3ACFile>]: Output 3AC program to <3ACFile>\n"
		  << " [-o <3ACFile>]: Output x86 program to <X86File>\n"
		  << "\n";
	std::cout << std::flush;
	std::cerr << std::flush;
	exit(1);
}

static void doTokenization(std::ifstream *input, const char *outPath)
{
	crona::Scanner scanner(input);
	if (strcmp(outPath, "--") == 0) {
		scanner.outputTokens(std::cout);
	} else {
		std::ofstream outStream(outPath);
		if (!outStream.good()) {
			std::string msg = "Bad output file ";
			msg += outPath;
			throw new crona::InternalError(msg.c_str());
		}
		scanner.outputTokens(outStream);
	}
}

static crona::ProgramNode *syntacticAnalysis(std::ifstream *input)
{
	if (input == nullptr) {
		return nullptr;
	}

	crona::ProgramNode *root = nullptr;

	crona::Scanner scanner(input);
#if 1
	crona::Parser parser(scanner, &root);
#else
	crona::Parser parser(scanner);
#endif

	int errCode = parser.parse();
	if (errCode != 0) {
		return nullptr;
	}

	return root;
}

static void outputAST(ASTNode *ast, std::string outPath)
{
	if (outPath == "--") {
		ast->unparse(std::cout, 0);
	} else {
		std::ofstream outStream(outPath);
		if (!outStream.good()) {
			std::string msg = "Bad output file ";
			msg += outPath;
			throw new crona::InternalError(msg.c_str());
		}
		ast->unparse(outStream, 0);
	}
}

static bool doUnparsing(std::ifstream *input, std::string outPath)
{
	crona::ProgramNode *ast = syntacticAnalysis(input);
	if (ast == nullptr) {
		std::cerr << "No AST built\n";
		return false;
	}
	if (input == nullptr) {
		return false;
	}

	outputAST(ast, outPath);
	return true;
}

static crona::NameAnalysis *doNameAnalysis(std::ifstream *input)
{
	crona::ProgramNode *ast = syntacticAnalysis(input);
	if (ast == nullptr) {
		return nullptr;
	}

	return crona::NameAnalysis::build(ast);
}

static crona::TypeAnalysis *doTypeAnalysis(std::ifstream *input)
{
	crona::NameAnalysis *nameAnalysis = doNameAnalysis(input);
	if (nameAnalysis == nullptr) {
		return nullptr;
	}

	return crona::TypeAnalysis::build(nameAnalysis);
}

static crona::IRProgram *do3AC(std::ifstream *input)
{
	crona::TypeAnalysis *typeAnalysis = doTypeAnalysis(input);
	if (typeAnalysis == nullptr) {
		return nullptr;
	}

	return typeAnalysis->ast->to3AC(typeAnalysis);
}

static void write3AC(crona::IRProgram *prog, std::string outPath)
{
	if (outPath.empty()) {
		throw new InternalError("Null 3AC file given");
	}
	std::string flatProg = prog->toString();
	if (outPath == "--") {
		std::cout << flatProg << std::endl;
	} else {
		std::ofstream outStream(outPath);
		outStream << flatProg << std::endl;
		outStream.close();
	}
}

static void writeX64(crona::IRProgram *prog, std::string outPath)
{
	if (outPath.empty()) {
		throw new InternalError("Null ASM file given");
	}
	if (outPath == "--") {
		prog->toX64(std::cout);
	} else {
		std::ofstream outStream(outPath);
		prog->toX64(outStream);
		outStream.close();
	}
}

int main(int argc, char *argv[])
{
	char *program_name = *argv++;
	if (argc <= 1) {
		usageAndDie();
	}

	char *file_name = *argv;
	std::ifstream *input = new std::ifstream(file_name);
	if (input == NULL) {
		usageAndDie();
	}

	if (!input->good()) {
		std::cerr << "Bad path " << file_name << std::endl;
		usageAndDie();
	}

	// Check if we actually just straight-up need to
	// build a compilable executable
	// I know, this is a bad try-catch, but whatever

	// This is gonna be real bad. Make the .s file lol with C++
	// horrible string operations
	std::string program_asm = "CRONA_INTERNAL.s";
	try {
		if (auto prog = do3AC(input)) {
			writeX64(prog, program_asm);
			// Do this very very very dangerous thingy
			std::system("as -o CRONA_INTERNAL.o CRONA_INTERNAL.s");
			std::system("\
ld -dynamic-linker /lib64/ld-linux-x86-64.so.2          \
/usr/lib/x86_64-linux-gnu/crt1.o                        \
/usr/lib/x86_64-linux-gnu/crti.o                        \
-lc                                                     \
CRONA_INTERNAL.o                                        \
./stdcrona.o                                            \
/usr/lib/x86_64-linux-gnu/crtn.o                        \
-o a.out                                                \
");
			std::system("rm CRONA_INTERAL.o CRONA_INTERNAL.s");
			return OK;
		}
		// Now, let's actually bind everything together!
	} catch (crona::InternalError *e) {
		std::cerr << "InternalError: " << e->msg() << "\n";
		return NOT_OK;
	}

	// Check whether the command is a no-op
	bool useful = false;
	// Output file if printing tokens
	const char *tokensFile = nullptr;
	// Flag set if doing syntactic analysis
	bool checkParse = false;
	// Output file if unparsing
	const char *unparseFile = nullptr;
	// Output file if doing name analysis
	const char *nameFile = nullptr;
	// Flag set if doing syntactic analysis
	bool checkTypes = false;
	// Output file if doing 3AC conversion
	const char *threeACFile = nullptr;
	// Output file for X64 representation
	const char *asmFile = nullptr;

	ARGBEGIN
	{
	case 't':
		tokensFile = ARGF;
		useful = true;
		break;
	case 'p':
		checkParse = true;
		useful = true;
		break;
	case 'u':
		unparseFile = ARGF;
		useful = true;
		break;
	case 'n':
		nameFile = ARGF;
		useful = true;
		break;
	case 'c':
		checkTypes = true;
		useful = true;
		break;
	case 'a':
		threeACFile = ARGF;
		useful = true;
		break;
	case 'o':
		asmFile = ARGF;
		useful = true;
		break;
	default:
		std::cerr << "Unknown option"
			  << " " << static_cast<char>(ARGC) << "\n";
		usageAndDie();
	}
	ARGEND;

	if (not useful) {
		std::cerr << "You didn't specify an operation to do!\n";
		usageAndDie();
	}

	try {
		if (tokensFile != nullptr) {
			doTokenization(input, tokensFile);
		}
		if (checkParse) {
			if (!syntacticAnalysis(input)) {
				std::cerr << "Parse failed";
			}
		}
		if (unparseFile != nullptr) {
			doUnparsing(input, unparseFile);
		}
		if (nameFile) {
			crona::NameAnalysis *na;
			na = doNameAnalysis(input);
			if (na != nullptr) {
				outputAST(na->ast, nameFile);
				return OK;
			}
			std::cerr << "Name Analysis Failed\n";
			return NOT_OK;
		}
		if (checkTypes) {
			if (doTypeAnalysis(input) != nullptr) {
				return OK;
			}
			std::cerr << "Type Analysis Failed\n";
			return NOT_OK;
		}
		if (threeACFile) {
			if (auto prog = do3AC(input)) {
				write3AC(prog, threeACFile);
				return OK;
			}
			return NOT_OK;
		}
		if (asmFile) {
			if (auto prog = do3AC(input)) {
				writeX64(prog, asmFile);
				return OK;
			}
			return NOT_OK;
		}
	} catch (crona::ToDoError *e) {
		std::cerr << "ToDoError: " << e->msg() << "\n";
		return NOT_OK;
	} catch (crona::InternalError *e) {
		std::cerr << "InternalError: " << e->msg() << "\n";
		return NOT_OK;
	}

	return OK;
}
