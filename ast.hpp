#ifndef CRONAC_AST_HPP
#define CRONAC_AST_HPP

#include <sstream>
#include <ostream>
#include <list>
#include "tokens.hpp"
#include "types.hpp"
#include "3ac.hpp"

// **********************************************************************
// ASTnode class (base class for all other kinds of nodes)
// **********************************************************************

namespace crona
{
/* You may find it useful to forward declare AST subclasses
   here so that you can use a class before it's full definition
*/
class NameAnalysis;
class TypeAnalysis;

class SymbolTable;
class SemSymbol;

class ProgramNode; //

/* ExpNode Subclasses */
class ExpNode; //

class AssignExpNode;

// BinaryExpNode
class BinaryExpNode;
class AndNode;
class OrNode;
class PlusNode;
class MinusNode;
class TimesNode;
class DivideNode;
class LessNode;
class LessEqNode;
class EqualsNode;
class NotEqualsNode;
class GreaterEqNode;
class GreaterNode;

// UnaryExpNode
class UnaryExpNode;
class NegNode;
class NotNode;

// LValNode
class LValNode;
class IDNode;
class IndexNode;

class CallExpNode;

class TrueNode;

class FalseNode;

class IntLitNode;

class StrLitNode;

class HavocNode;

/* StmtNode Subclasses */
class StmtNode;

class AssignStmtNode;

class CallStmtNode;

// DeclNode
class DeclNode; //
class FnDeclNode;
// VarDeclNode
class VarDeclNode; //
class FormalDeclNode;

class IfStmtNode;

class IfElseStmtNode;

class PostDecStmtNode;

class PostIncStmtNode;

class ReadStmtNode;

class WriteStmtNode;

class WhileStmtNode;

class ReturnStmtNode;

/* TypeNode subclasses */
class TypeNode; //
class ArrayTypeNode;
class BoolTypeNode;
class ByteTypeNode;
class IntTypeNode; //
class VoidTypeNode;

// See LValNode
//class IDNode; //
class Opd;

class DeclListNode; //

class ASTNode {
    public:
	ASTNode(size_t lineIn, size_t colIn) : l(lineIn), c(colIn)
	{
	}
	virtual void unparse(std::ostream &out, int indent) = 0;
	size_t line() const
	{
		return l;
	}
	size_t col() const
	{
		return c;
	}
	virtual bool nameAnalysis(SymbolTable *);

	/**
	* Return a string specifying the position this node begins.
	* For some node a position doesn't really make sense (i.e.
	* ProgramNode) but for the rest it's the position in the 
	* input file that represents that node
	**/
	std::string pos()
	{
		return "[" + std::to_string(line()) + "," +
		       std::to_string(col()) + "]";
	}

    private:
	size_t l; /// The line at which the node starts in the input file
	size_t c; /// The column at which the node starts in the input file
};

/** 
* \class ProgramNode
* Class that contains the entire abstract syntax tree for a program.
* Note the list of declarations encompasses all global declarations
* which includes (obviously) all global variables and struct declarations
* and (perhaps less obviously), all function declarations
**/
class ProgramNode : public ASTNode {
    public:
	ProgramNode(std::list<DeclNode *> *globalsIn)
		: ASTNode(1, 1), _globals(globalsIn)
	{
	}
	void unparse(std::ostream &out, int indent) override;
	virtual bool nameAnalysis(SymbolTable *) override;
	virtual void typeAnalysis(TypeAnalysis *);
	IRProgram *to3AC(TypeAnalysis *ta);

    private:
	std::list<DeclNode *> *_globals;
};

class StmtNode : public ASTNode {
    public:
	StmtNode(size_t line, size_t col) : ASTNode(line, col)
	{
	}
	virtual void unparse(std::ostream &out, int indent) override = 0;
	virtual void typeAnalysis(TypeAnalysis *);
	virtual void to3AC(Procedure *proc) = 0;
};

/**  \class ExpNode
* Superclass for expression nodes (i.e. nodes that can be used as
* part of an expression).  Nodes that are part of an expression
* should inherit from this abstract superclass.
**/
class ExpNode : public ASTNode {
    public:
	virtual bool nameAnalysis(SymbolTable *symTab) override = 0;
	virtual void typeAnalysis(TypeAnalysis *);
	virtual Opd *flatten(Procedure *proc) = 0;

    protected:
	ExpNode(size_t line, size_t col) : ASTNode(line, col)
	{
	}
};

/* BEGIN ExpNode Subclasses */

class AssignExpNode : public ExpNode {
    public:
	AssignExpNode(size_t line, size_t col, LValNode *lval, ExpNode *exp)
		: ExpNode(line, col), _lval_node(lval), _exp_node(exp)
	{
	}
	virtual bool nameAnalysis(SymbolTable *) override;
	void unparse(std::ostream &out, int indent) override;
	virtual void typeAnalysis(TypeAnalysis *) override;
	virtual Opd *flatten(Procedure *proc) override;

    private:
	LValNode *_lval_node;
	ExpNode *_exp_node;
};

// BinaryExpNode
class BinaryExpNode : public ExpNode {
    public:
	void unparse(std::ostream &out, int indent) override;
	virtual Opd *flatten(Procedure *prog) override = 0;

    protected:
	BinaryExpNode(size_t line, size_t col, ExpNode *lhs, ExpNode *rhs)
		: ExpNode(line, col), _lhs(lhs), _rhs(rhs)
	{
	}
	virtual bool nameAnalysis(SymbolTable *) override;
	virtual void typeAnalysis(TypeAnalysis *) override;
	virtual std::string opStr() = 0;
	ExpNode *_lhs;
	ExpNode *_rhs;
};

class AndNode : public BinaryExpNode {
    public:
	AndNode(size_t line, size_t col, ExpNode *lhs, ExpNode *rhs)
		: BinaryExpNode(line, col, lhs, rhs)
	{
	}
	virtual void typeAnalysis(TypeAnalysis *) override;
	virtual Opd *flatten(Procedure *prog) override;

    protected:
	std::string opStr() override
	{
		return "&&";
	}
};

class OrNode : public BinaryExpNode {
    public:
	OrNode(size_t line, size_t col, ExpNode *lhs, ExpNode *rhs)
		: BinaryExpNode(line, col, lhs, rhs)
	{
	}
	virtual void typeAnalysis(TypeAnalysis *) override;
	virtual Opd *flatten(Procedure *prog) override;

    protected:
	std::string opStr() override
	{
		return "||";
	}
};

class PlusNode : public BinaryExpNode {
    public:
	PlusNode(size_t line, size_t col, ExpNode *lhs, ExpNode *rhs)
		: BinaryExpNode(line, col, lhs, rhs)
	{
	}
	virtual void typeAnalysis(TypeAnalysis *) override;
	virtual Opd *flatten(Procedure *prog) override;

    protected:
	std::string opStr() override
	{
		return "+";
	}
};

class MinusNode : public BinaryExpNode {
    public:
	MinusNode(size_t line, size_t col, ExpNode *lhs, ExpNode *rhs)
		: BinaryExpNode(line, col, lhs, rhs)
	{
	}
	virtual void typeAnalysis(TypeAnalysis *) override;
	virtual Opd *flatten(Procedure *prog) override;

    protected:
	std::string opStr() override
	{
		return "-";
	}
};

class TimesNode : public BinaryExpNode {
    public:
	TimesNode(size_t line, size_t col, ExpNode *lhs, ExpNode *rhs)
		: BinaryExpNode(line, col, lhs, rhs)
	{
	}
	virtual void typeAnalysis(TypeAnalysis *) override;
	virtual Opd *flatten(Procedure *prog) override;

    protected:
	std::string opStr() override
	{
		return "*";
	}
};

class DivideNode : public BinaryExpNode {
    public:
	DivideNode(size_t line, size_t col, ExpNode *lhs, ExpNode *rhs)
		: BinaryExpNode(line, col, lhs, rhs)
	{
	}
	virtual void typeAnalysis(TypeAnalysis *) override;
	virtual Opd *flatten(Procedure *prog) override;

    protected:
	std::string opStr() override
	{
		return "/";
	}
};

class LessNode : public BinaryExpNode {
    public:
	LessNode(size_t line, size_t col, ExpNode *lhs, ExpNode *rhs)
		: BinaryExpNode(line, col, lhs, rhs)
	{
	}
	virtual void typeAnalysis(TypeAnalysis *) override;
	virtual Opd *flatten(Procedure *prog) override;

    protected:
	std::string opStr() override
	{
		return "<";
	}
};

class LessEqNode : public BinaryExpNode {
    public:
	LessEqNode(size_t line, size_t col, ExpNode *lhs, ExpNode *rhs)
		: BinaryExpNode(line, col, lhs, rhs)
	{
	}
	virtual void typeAnalysis(TypeAnalysis *) override;
	virtual Opd *flatten(Procedure *prog) override;

    protected:
	std::string opStr() override
	{
		return "<=";
	}
};

class EqualsNode : public BinaryExpNode {
    public:
	EqualsNode(size_t line, size_t col, ExpNode *lhs, ExpNode *rhs)
		: BinaryExpNode(line, col, lhs, rhs)
	{
	}
	virtual void typeAnalysis(TypeAnalysis *) override;
	virtual Opd *flatten(Procedure *prog) override;

    protected:
	std::string opStr() override
	{
		return "==";
	}
};

class NotEqualsNode : public BinaryExpNode {
    public:
	NotEqualsNode(size_t line, size_t col, ExpNode *lhs, ExpNode *rhs)
		: BinaryExpNode(line, col, lhs, rhs)
	{
	}
	virtual void typeAnalysis(TypeAnalysis *) override;
	virtual Opd *flatten(Procedure *prog) override;

    protected:
	std::string opStr() override
	{
		return "!=";
	}
};

class GreaterEqNode : public BinaryExpNode {
    public:
	GreaterEqNode(size_t line, size_t col, ExpNode *lhs, ExpNode *rhs)
		: BinaryExpNode(line, col, lhs, rhs)
	{
	}
	virtual void typeAnalysis(TypeAnalysis *) override;
	virtual Opd *flatten(Procedure *prog) override;

    protected:
	std::string opStr() override
	{
		return ">=";
	}
};

class GreaterNode : public BinaryExpNode {
    public:
	GreaterNode(size_t line, size_t col, ExpNode *lhs, ExpNode *rhs)
		: BinaryExpNode(line, col, lhs, rhs)
	{
	}
	virtual void typeAnalysis(TypeAnalysis *) override;
	virtual Opd *flatten(Procedure *prog) override;

    protected:
	std::string opStr() override
	{
		return ">";
	}
};

// UnaryExpNode
class UnaryExpNode : public ExpNode {
    public:
	void unparse(std::ostream &out, int indent) override;
	virtual bool nameAnalysis(SymbolTable *) override;
	virtual void typeAnalysis(TypeAnalysis *) = 0;
	virtual Opd *flatten(Procedure *prog) override = 0;

    protected:
	UnaryExpNode(size_t line, size_t col, ExpNode *innerExp)
		: ExpNode(line, col), _exp_node(innerExp)
	{
	}
	virtual std::string opStr() = 0;
	ExpNode *_exp_node;
};

class NegNode : public UnaryExpNode {
    public:
	NegNode(size_t line, size_t col, ExpNode *innerExp)
		: UnaryExpNode(line, col, innerExp)
	{
	}
	virtual void typeAnalysis(TypeAnalysis *) override;
	virtual Opd *flatten(Procedure *prog) override;

    protected:
	std::string opStr() override
	{
		return "-";
	}
};

class NotNode : public UnaryExpNode {
    public:
	NotNode(size_t line, size_t col, ExpNode *innerExp)
		: UnaryExpNode(line, col, innerExp)
	{
	}
	virtual void typeAnalysis(TypeAnalysis *) override;
	virtual Opd *flatten(Procedure *prog) override;

    protected:
	std::string opStr() override
	{
		return "!";
	}
};

// LValNode
class LValNode : public ExpNode {
    public:
	virtual void unparse(std::ostream &out, int indent) override = 0;
	bool nameAnalysis(SymbolTable *symTab) override
	{
		return false;
	}
	virtual void typeAnalysis(TypeAnalysis *) override
	{
		;
	}
	virtual Opd *flatten(Procedure *proc) = 0;
	virtual SemSymbol *getSymbol() = 0;

    protected:
	LValNode(size_t line, size_t col) : ExpNode(line, col)
	{
	}
};

class IDNode : public LValNode {
    public:
	IDNode(size_t line, size_t col, std::string idName)
		: LValNode(line, col), _name(idName)
	{
	}
	void unparse(std::ostream &out, int indent) override;
	virtual bool nameAnalysis(SymbolTable *) override;
	std::string ID() const
	{
		return _name;
	}
	void setSymbol(SemSymbol *sym)
	{
		_sem_symbol = sym;
	}
	virtual SemSymbol *getSymbol() override
	{
		return _sem_symbol;
	}
	virtual void typeAnalysis(TypeAnalysis *) override;
	virtual Opd *flatten(Procedure *proc) override;

    private:
	std::string _name;
	SemSymbol *_sem_symbol;
};

class IndexNode : public LValNode {
    public:
	IndexNode(size_t line, size_t col, IDNode *baseId, ExpNode *indexExp)
		: LValNode(line, col), _id(baseId), _offset(indexExp)
	{
	}
	void unparse(std::ostream &out, int indent) override;
	virtual bool nameAnalysis(SymbolTable *) override;
	virtual void typeAnalysis(TypeAnalysis *) override;
	virtual Opd *flatten(Procedure *prog) override;
	virtual SemSymbol *getSymbol() override
	{
		return _id->getSymbol();
	}

    private:
	IDNode *_id;
	ExpNode *_offset;
};

class CallExpNode : public ExpNode {
    public:
	CallExpNode(size_t line, size_t col, IDNode *id_func,
		    std::list<ExpNode *> *fn_params)
		: ExpNode(line, col), _id_func(id_func), _id_params(fn_params)
	{
	}
	virtual bool nameAnalysis(SymbolTable *) override;
	virtual void typeAnalysis(TypeAnalysis *) override;
	void unparse(std::ostream &out, int indent) override;
	virtual Opd *flatten(Procedure *proc) override;

    protected:
	IDNode *_id_func;
	std::list<ExpNode *> *_id_params;
};

class TrueNode : public ExpNode {
    public:
	TrueNode(size_t line, size_t col) : ExpNode(line, col)
	{
	}
	virtual bool nameAnalysis(SymbolTable *) override;
	virtual void typeAnalysis(TypeAnalysis *) override;
	void unparse(std::ostream &out, int indent) override;
	virtual Opd *flatten(Procedure *prog) override;
};

class FalseNode : public ExpNode {
    public:
	FalseNode(size_t line, size_t col) : ExpNode(line, col)
	{
	}
	virtual bool nameAnalysis(SymbolTable *) override;
	virtual void typeAnalysis(TypeAnalysis *) override;
	void unparse(std::ostream &out, int indent) override;
	virtual Opd *flatten(Procedure *prog) override;
};

class IntLitNode : public ExpNode {
    public:
	IntLitNode(size_t line, size_t col, int val)
		: ExpNode(line, col), _value(val)
	{
	}
	void unparse(std::ostream &out, int indent) override;
	bool nameAnalysis(SymbolTable *symTab) override
	{
		return true;
	}
	int value() const
	{
		return _value;
	}
	virtual void typeAnalysis(TypeAnalysis *) override;
	virtual Opd *flatten(Procedure *prog) override;

    private:
	const int _value;
};

class StrLitNode : public ExpNode {
    public:
	StrLitNode(size_t line, size_t col, std::string val)
		: ExpNode(line, col), _value(val)
	{
	}
	void unparse(std::ostream &out, int indent) override;
	bool nameAnalysis(SymbolTable *symTab) override
	{
		return true;
	}
	virtual void typeAnalysis(TypeAnalysis *) override;
	virtual Opd *flatten(Procedure *prog) override;

    private:
	const std::string _value;
};

class HavocNode : public ExpNode {
    public:
	HavocNode(size_t line, size_t col) : ExpNode(line, col)
	{
	}
	virtual bool nameAnalysis(SymbolTable *) override;
	virtual void typeAnalysis(TypeAnalysis *) override;
	void unparse(std::ostream &out, int indent) override;
	virtual Opd *flatten(Procedure *prog) override;
};

/* END ExpNode Subclasses */

/* BEGIN StmtNode Subclasses */

class AssignStmtNode : public StmtNode {
    public:
	AssignStmtNode(size_t line, size_t col, AssignExpNode *assign_node)
		: StmtNode(line, col), _assign_node(assign_node)
	{
	}
	virtual bool nameAnalysis(SymbolTable *) override;
	void unparse(std::ostream &out, int indent) override;
	virtual void typeAnalysis(TypeAnalysis *) override;
	virtual void to3AC(Procedure *prog) override;

    protected:
	AssignExpNode *_assign_node;
};

class CallStmtNode : public StmtNode {
    public:
	CallStmtNode(size_t line, size_t col, CallExpNode *call_exp_node)
		: StmtNode(line, col), _call_exp_node(call_exp_node)
	{
	}
	virtual bool nameAnalysis(SymbolTable *) override;
	virtual void typeAnalysis(TypeAnalysis *) override;
	void unparse(std::ostream &out, int indent) override;
	virtual void to3AC(Procedure *proc) override;

    protected:
	CallExpNode *_call_exp_node;
};

class IfStmtNode : public StmtNode {
    public:
	IfStmtNode(size_t line, size_t col, ExpNode *exp_node,
		   std::list<StmtNode *> *stmt_nodes)
		: StmtNode(line, col), _exp_node(exp_node),
		  _stmt_nodes(stmt_nodes)
	{
	}
	virtual bool nameAnalysis(SymbolTable *) override;
	virtual void typeAnalysis(TypeAnalysis *) override;
	void unparse(std::ostream &out, int indent) override;
	virtual void to3AC(Procedure *prog) override;

    protected:
	ExpNode *_exp_node;
	std::list<StmtNode *> *_stmt_nodes;
};

class IfElseStmtNode : public StmtNode {
    public:
	IfElseStmtNode(size_t line, size_t col, ExpNode *exp_node,
		       std::list<StmtNode *> *stmt_nodes_if_true,
		       std::list<StmtNode *> *stmt_nodes_if_false)
		: StmtNode(line, col), _exp_node(exp_node),
		  _stmt_nodes_if_true(stmt_nodes_if_true),
		  _stmt_nodes_if_false(stmt_nodes_if_false)
	{
	}
	virtual bool nameAnalysis(SymbolTable *) override;
	virtual void typeAnalysis(TypeAnalysis *) override;
	void unparse(std::ostream &out, int indent) override;
	virtual void to3AC(Procedure *prog) override;

    protected:
	ExpNode *_exp_node;
	std::list<StmtNode *> *_stmt_nodes_if_true;
	std::list<StmtNode *> *_stmt_nodes_if_false;
};

class PostDecStmtNode : public StmtNode {
    public:
	PostDecStmtNode(size_t line, size_t col, LValNode *lval_node)
		: StmtNode(line, col), _lval_node(lval_node)
	{
	}
	virtual bool nameAnalysis(SymbolTable *) override;
	virtual void typeAnalysis(TypeAnalysis *) override;
	void unparse(std::ostream &out, int indent) override;
	virtual void to3AC(Procedure *prog) override;

    protected:
	LValNode *_lval_node;
};

class PostIncStmtNode : public StmtNode {
    public:
	PostIncStmtNode(size_t line, size_t col, LValNode *lval_node)
		: StmtNode(line, col), _lval_node(lval_node)
	{
	}
	virtual bool nameAnalysis(SymbolTable *) override;
	virtual void typeAnalysis(TypeAnalysis *) override;
	void unparse(std::ostream &out, int indent) override;
	virtual void to3AC(Procedure *prog) override;

    protected:
	LValNode *_lval_node;
};

class ReadStmtNode : public StmtNode {
    public:
	ReadStmtNode(size_t line, size_t col, LValNode *lval_node)
		: StmtNode(line, col), _lval_node(lval_node)
	{
	}
	virtual bool nameAnalysis(SymbolTable *) override;
	virtual void typeAnalysis(TypeAnalysis *) override;
	void unparse(std::ostream &out, int indent) override;
	virtual void to3AC(Procedure *prog) override;

    protected:
	LValNode *_lval_node;
};

class WriteStmtNode : public StmtNode {
    public:
	WriteStmtNode(size_t line, size_t col, ExpNode *exp_node)
		: StmtNode(line, col), _exp_node(exp_node)
	{
	}
	virtual bool nameAnalysis(SymbolTable *) override;
	virtual void typeAnalysis(TypeAnalysis *) override;
	void unparse(std::ostream &out, int indent) override;
	virtual void to3AC(Procedure *prog) override;

    protected:
	ExpNode *_exp_node;
};

class WhileStmtNode : public StmtNode {
    public:
	WhileStmtNode(size_t line, size_t col, ExpNode *exp_node,
		      std::list<StmtNode *> *stmt_nodes)
		: StmtNode(line, col), _exp_node(exp_node),
		  _stmt_nodes(stmt_nodes)
	{
	}
	virtual bool nameAnalysis(SymbolTable *) override;
	virtual void typeAnalysis(TypeAnalysis *) override;
	void unparse(std::ostream &out, int indent) override;
	virtual void to3AC(Procedure *prog) override;

    protected:
	ExpNode *_exp_node;
	std::list<StmtNode *> *_stmt_nodes;
};

class ReturnStmtNode : public StmtNode {
    public:
	ReturnStmtNode(size_t line, size_t col, ExpNode *exp_node)
		: StmtNode(line, col), _exp_node(exp_node)
	{
	}
	virtual bool nameAnalysis(SymbolTable *) override;
	virtual void typeAnalysis(TypeAnalysis *) override;
	void unparse(std::ostream &out, int indent) override;
	virtual void to3AC(Procedure *prog) override;

    protected:
	ExpNode *_exp_node;
};

/* END StmtNode Subclasses */

/** \class DeclNode
* Superclass for declarations (i.e. nodes that can be used to 
* declare a struct, function, variable, etc).  This base class will 
**/
class DeclNode : public StmtNode {
    public:
	virtual void unparse(std::ostream &out, int indent) override = 0;
	virtual void typeAnalysis(TypeAnalysis *) override;
	virtual void to3AC(IRProgram *prog) = 0;
	virtual void to3AC(Procedure *proc) override = 0;

    protected:
	DeclNode(size_t line, size_t col) : StmtNode(line, col)
	{
	}
};

class FnDeclNode : public DeclNode {
    public:
	FnDeclNode(size_t line, size_t col, IDNode *id, TypeNode *type,
		   std::list<FormalDeclNode *> *decl_nodes,
		   std::list<StmtNode *> *stmt_nodes)
		: DeclNode(line, col), _id(id), _type(type),
		  _decl_nodes(decl_nodes), _stmt_nodes(stmt_nodes)
	{
	}
	void unparse(std::ostream &out, int indent) override;
	virtual bool nameAnalysis(SymbolTable *) override;
	IDNode *ID() const
	{
		return _id;
	}
	std::list<FormalDeclNode *> *getFormals() const
	{
		return _decl_nodes;
	}
	virtual void typeAnalysis(TypeAnalysis *) override;
	void to3AC(IRProgram *prog) override;
	void to3AC(Procedure *prog) override;

    protected:
	IDNode *_id;
	TypeNode *_type;
	std::list<FormalDeclNode *> *_decl_nodes;
	std::list<StmtNode *> *_stmt_nodes;
};

class VarDeclNode : public DeclNode {
    public:
	VarDeclNode(size_t line, size_t col, IDNode *id, TypeNode *type)
		: DeclNode(line, col), _id(id), _type(type)
	{
	}
	void unparse(std::ostream &out, int indent) override;
	virtual bool nameAnalysis(SymbolTable *) override;
	IDNode *ID()
	{
		return _id;
	}
	TypeNode *getTypeNode()
	{
		return _type;
	}
	virtual void typeAnalysis(TypeAnalysis *) override;
	virtual void to3AC(Procedure *proc) override;
	virtual void to3AC(IRProgram *prog) override;

    protected:
	IDNode *_id;
	TypeNode *_type;
};

class FormalDeclNode : public VarDeclNode {
    public:
	FormalDeclNode(size_t line, size_t col, IDNode *id, TypeNode *type)
		: VarDeclNode(line, col, id, type)
	{
	}
	void unparse(std::ostream &out, int indent) override;
	virtual void to3AC(Procedure *proc) override;
	virtual void to3AC(IRProgram *prog) override;
};

/**  \class TypeNode
* Superclass of nodes that indicate a data type. For example, in 
* the declaration "int a", the int part is the type node (a is an IDNode
* and the whole thing is a DeclNode).
**/
class TypeNode : public ASTNode {
    protected:
	TypeNode(size_t lineIn, size_t colIn) : ASTNode(lineIn, colIn)
	{
	}

    public:
	virtual void unparse(std::ostream &out, int indent) = 0;
	virtual std::string typeString() const = 0;
	virtual bool nameAnalysis(SymbolTable *symTab) override
	{
		return true;
	}
	virtual DataType *getType() = 0;
	virtual void typeAnalysis(TypeAnalysis *ta);
	//TODO: consider adding an isRef to use in unparse to
	// indicate if this is a reference type
};

/* BEGIN TypeNode Subclasses */

class IntTypeNode : public TypeNode {
    public:
	IntTypeNode(size_t lineIn, size_t colIn) : TypeNode(lineIn, colIn)
	{
	}
	void unparse(std::ostream &out, int indent) override;
	virtual std::string typeString() const override
	{
		return "int";
	}
	virtual DataType *getType() override;
};

class ByteToIntNode : public ExpNode {
    public:
	ByteToIntNode(ExpNode *child)
		: ExpNode(child->line(), child->col()), _exp_node(child)
	{
	}
	void unparse(std::ostream &out, int indent) override
	{
		_exp_node->unparse(out, indent);
	}
	bool nameAnalysis(SymbolTable *symTab) override
	{
		return true;
	}
	virtual void typeAnalysis(TypeAnalysis *) override;
	virtual Opd *flatten(Procedure *prog) override;

    private:
	ExpNode *_exp_node;
};

class ArrayTypeNode : public TypeNode {
    public:
	ArrayTypeNode(size_t lineIn, size_t colIn, TypeNode *type_node,
		      IntLitNode *size)
		: TypeNode(lineIn, colIn), _type_node(type_node), _size(size)
	{
	}
	void unparse(std::ostream &out, int indent) override;
	virtual std::string typeString() const override
	{
		std::stringstream type;
		type << _type_node->typeString() << " array[" << _size->value()
		     << "]";
		return type.str();
	}
	virtual TypeNode *getBaseType()
	{
		return _type_node;
	}
	virtual DataType *getType() override
	{
		const BasicType *t = _type_node->getType()->asBasic();
		return ArrayType::produce(t, _size->value());
	}

    protected:
	TypeNode *_type_node;
	IntLitNode *_size;
};

class BoolTypeNode : public TypeNode {
    public:
	BoolTypeNode(size_t lineIn, size_t colIn) : TypeNode(lineIn, colIn)
	{
	}
	void unparse(std::ostream &out, int indent) override;
	virtual std::string typeString() const override
	{
		return "bool";
	}
	virtual DataType *getType() override;
};

class StrTypeNode : public TypeNode {
    public:
	StrTypeNode(size_t lineIn, size_t colIn) : TypeNode(lineIn, colIn)
	{
	}
	void unparse(std::ostream &out, int indent) override;
	virtual std::string typeString() const override
	{
		return "byte array[0]";
	}
};

class ByteTypeNode : public TypeNode {
    public:
	ByteTypeNode(size_t lineIn, size_t colIn) : TypeNode(lineIn, colIn)
	{
	}
	void unparse(std::ostream &out, int indent) override;
	virtual std::string typeString() const override
	{
		return "byte";
	}
	virtual DataType *getType() override;
};

class VoidTypeNode : public TypeNode {
    public:
	VoidTypeNode(size_t lineIn, size_t colIn) : TypeNode(lineIn, colIn)
	{
	}
	void unparse(std::ostream &out, int indent) override;
	virtual std::string typeString() const override
	{
		return "void";
	}
	virtual DataType *getType() override
	{
		return BasicType::VOID();
	}
};

/* END TypeNode Subclasses */

/** A variable declaration. Note that this class is intended to 
 * represent a global or local variable of any type (including a struct
 * type. Note that this is not intended to represent a declaration of
 * a struct. In other words:
 * struct MyStruct { 
 *   int fieldA;
 * };
 * is NOT a VarDeclNode because it introduces a new datatype, not a new
 * variable (in this case, the example is a StructDeclNode).  * However,
 * struct MyStruct instance; *is* a VarDeclNode, since it introduces a 
 * new variable to the program. 
**/

} //End namespace crona

#endif
