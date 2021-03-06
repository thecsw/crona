#ifndef __CRONA_SCANNER_HPP__
#define __CRONA_SCANNER_HPP__ 1

#if !defined(yyFlexLexerOnce)
#include <FlexLexer.h>
#endif

#include "grammar.hh"
#include "errors.hpp"

using TokenKind = crona::Parser::token;

namespace crona
{
class Scanner : public yyFlexLexer {
    public:
	Scanner(std::istream *in) : yyFlexLexer(in)
	{
		lineNum = 1;
		colNum = 1;
	};
	virtual ~Scanner(){};

	//get rid of override virtual function warning
	using FlexLexer::yylex;

	// YY_DECL defined in the flex crona.l
	virtual int yylex(crona::Parser::semantic_type *const lval);

	int makeBareToken(int tagIn)
	{
		this->yylval->lexeme =
			new Token(this->lineNum, this->colNum, tagIn);
		colNum += static_cast<size_t>(yyleng);
		return tagIn;
	}

	void errIllegal(size_t l, size_t c, std::string match)
	{
		crona::Report::fatal(l, c, "Illegal character " + match);
	}

	void errStrEsc(size_t l, size_t c)
	{
		crona::Report::fatal(l, c,
				     "String literal with bad"
				     " escape sequence ignored");
	}

	void errStrUnterm(size_t l, size_t c)
	{
		crona::Report::fatal(l, c,
				     "Unterminated string"
				     " literal ignored");
	}

	void errStrEscAndUnterm(size_t l, size_t c)
	{
		crona::Report::fatal(l, c,
				     "Unterminated string literal"
				     " with bad escape sequence ignored");
	}

	void errIntOverflow(size_t l, size_t c)
	{
		crona::Report::fatal(l, c,
				     "Integer literal too large;"
				     " using max value");
	}

	void warn(int lineNumIn, int colNumIn, std::string msg)
	{
		std::cerr << lineNumIn << ":" << colNumIn << " ***WARNING*** "
			  << msg << std::endl;
	}

	void error(int lineNumIn, int colNumIn, std::string msg)
	{
		std::cerr << lineNumIn << ":" << colNumIn << " ***ERROR*** "
			  << msg << std::endl;
	}

	static std::string tokenKindString(int tokenKind);

	void outputTokens(std::ostream &outstream);

    private:
	crona::Parser::semantic_type *yylval = nullptr;
	size_t lineNum;
	size_t colNum;
};

} // namespace crona

#endif /* END __CRONA_SCANNER_HPP__ */
