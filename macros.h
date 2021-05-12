#ifndef _MACROS_H
#define _MACROS_H
/*
Collection of C/C++ macros by Sagindyk Urazayev @thecsw
*/

/* Min and max */
#define MAX(a, b) ((a > b) ? a : b)
#define MIN(a, b) ((a < b) ? a : b)

/* Standard modes */
#define ANSI_NORMAL "\033[0m"
#define ANSI_BOLD "\033[1m"
#define ANSI_FAINT "\033[2m"
#define ANSI_ITALIC "\033[3m"
#define ANSI_UNDERLINE "\033[4m"
#define ANSI_SLOW_BLINK "\033[5m"
#define ANSI_RAPID_BLINK "\033[6m"

/* Change the color of the term"inal */
#define ANSI_BLACK_FG "\033[30m"
#define ANSI_RED_FG "\033[31m"
#define ANSI_GREEN_FG "\033[32m"
#define ANSI_YELLOW_FG "\033[33m"
#define ANSI_BLUE_FG "\033[34m"
#define ANSI_MAGENTA_FG "\033[35m"
#define ANSI_CYAN_FG "\033[36m"
#define ANSI_WHITE_FG "\033[37m"
/* Background */
#define ANSI_BLACK_BG "\033[40m"
#define ANSI_RED_BG "\033[41m"
#define ANSI_GREEN_BG "\033[42m"
#define ANSI_YELLOW_BG "\033[43m"
#define ANSI_BLUE_BG "\033[44m"
#define ANSI_MAGENTA_BG "\033[45m"
#define ANSI_CYAN_BG "\033[46m"
#define ANSI_WHITE_BG "\033[47m"
/* Bright colors */
#define ANSI_BRIGHT_BLACK_FG "\033[90m"
#define ANSI_BRIGHT_RED_FG "\033[91m"
#define ANSI_BRIGHT_GREEN_FG "\033[92m"
#define ANSI_BRIGHT_YELLOW_FG "\033[93m"
#define ANSI_BRIGHT_BLUE_FG "\033[94m"
#define ANSI_BRIGHT_MAGENTA_FG "\033[95m"
#define ANSI_BRIGHT_CYAN_FG "\033[96m"
#define ANSI_BRIGHT_WHITE_FG "\033[97m"
/* Bright background */
#define ANSI_BRIGHT_BLACK_BG "\033[100m"
#define ANSI_BRIGHT_RED_BG "\033[101m"
#define ANSI_BRIGHT_GREEN_BG "\033[102m"
#define ANSI_BRIGHT_YELLOW_BG "\033[103m"
#define ANSI_BRIGHT_BLUE_BG "\033[104m"
#define ANSI_BRIGHT_MAGENTA_BG "\033[105m"
#define ANSI_BRIGHT_CYAN_BG "\033[106m"
#define ANSI_BRIGHT_WHITE_BG "\033[107m"

/* 
   Command line argument parser inspired by plan9
   Should be used in a form of
   ARGBEGIN {
   case 'c':
     do_something;
     break;
   } ARGEND

   Use ARGF to get the next argument, like a filename
   Use ARGC to get the current letter argument
 */
#define ARGBEGIN                                                               \
	for (argv++, argc--; argv[0] && argv[0][0] == '-' && argv[0][1];       \
	     argc--, argv++) {                                                 \
		char *_args = &argv[0][1];                                     \
		int _argc;                                                     \
		if (_args[0] == '-') {                                         \
			argc--;                                                \
			argv++;                                                \
			break;                                                 \
		}                                                              \
		_argc = _args[0];                                              \
		switch (_argc)
#define ARGEND }
#define ARGF (*(argv + 1) ? *++argv : NULL)
#define ARGC _argc
#endif /* _MACROS_H */
