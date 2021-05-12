#include <typeinfo>

#include "ast.hpp"
#include "symbol_table.hpp"
#include "errors.hpp"

namespace crona
{
//TODO here is a subset of the nodes needed to do nameAnalysis,
// you should add the rest to allow for a complete treatment
// of any AST

#define ERR_DUPDECL                                                            \
	Report::fatal(line(), col(), "Multiply declared identifier");
#define ERR_BADDECLTYPE                                                        \
	Report::fatal(line(), col(), "Invalid type in declaration");
#define ERR_UNDECLID Report::fatal(line(), col(), "Undeclared identifier");

bool ASTNode::nameAnalysis(SymbolTable *symTab)
{
	throw new ToDoError("This function should have"
			    "been overriden in the subclass!");
}

bool ProgramNode::nameAnalysis(SymbolTable *symTab)
{
	bool res = true;
	for (auto global : *_globals) {
		res = global->nameAnalysis(symTab) && res;
	}
	return res;
}

bool VarDeclNode::nameAnalysis(SymbolTable *symTab)
{
	bool nameAnalysisOk = true;
	if (typeid(*_type).hash_code() == typeid(VoidTypeNode).hash_code()) {
		ERR_BADDECLTYPE;
		nameAnalysisOk = false;
	}
	auto sym = new SemSymbol(SemSymbol::Kind::VAR, _id->ID(),
				 _type->getType());
	if (!symTab->insert(_id->ID(), sym)) {
		ERR_DUPDECL;
		nameAnalysisOk = false;
	} else {
		_id->setSymbol(sym);
	}
	return nameAnalysisOk;
}

bool FnDeclNode::nameAnalysis(SymbolTable *symTab)
{
	bool nameAnalysisOk = true;

	std::list<const DataType *> *formalTypes =
		new std::list<const DataType *>();
	for (auto varDecl : *_decl_nodes) {
		const DataType *formalType = varDecl->getTypeNode()->getType();
		formalTypes->push_back(formalType);
	}

	const DataType *retType = _type->getType();
	FnType *dataType = new FnType(formalTypes, retType);

	auto sym = new SemSymbol(SemSymbol::Kind::FN, _id->ID(), dataType);
	if (!symTab->insert(_id->ID(), sym)) {
		ERR_DUPDECL;
		nameAnalysisOk = false;
	} else {
		_id->setSymbol(sym);
	}

	symTab->enter();
	for (auto varDecl : *_decl_nodes) {
		nameAnalysisOk =
			varDecl->nameAnalysis(symTab) && nameAnalysisOk;
	}
	for (auto stmt : *_stmt_nodes) {
		nameAnalysisOk = stmt->nameAnalysis(symTab) && nameAnalysisOk;
	}
	symTab->leave();
	return nameAnalysisOk;
}

bool AssignExpNode::nameAnalysis(SymbolTable *symTab)
{
	bool good = _lval_node->nameAnalysis(symTab);
	return _exp_node->nameAnalysis(symTab) && good;
}

bool BinaryExpNode::nameAnalysis(SymbolTable *symTab)
{
	bool good = _lhs->nameAnalysis(symTab);
	return _rhs->nameAnalysis(symTab) && good;
}

bool UnaryExpNode::nameAnalysis(SymbolTable *symTab)
{
	return _exp_node->nameAnalysis(symTab);
}

bool IDNode::nameAnalysis(SymbolTable *symTab)
{
	if (auto sym = symTab->getSymbol(ID())) {
		setSymbol(sym);
		return true;
	}
	ERR_UNDECLID;
	return false;
}

bool IndexNode::nameAnalysis(SymbolTable *symTab)
{
	bool good = _id->nameAnalysis(symTab);
	return _offset->nameAnalysis(symTab) && good;
}

bool CallExpNode::nameAnalysis(SymbolTable *symTab)
{
	bool good = _id_func->nameAnalysis(symTab);
	for (auto *param : *_id_params)
		good = param->nameAnalysis(symTab) && good;
	return good;
}

bool TrueNode::nameAnalysis(SymbolTable *symTab)
{
	return true;
}

bool FalseNode::nameAnalysis(SymbolTable *symTab)
{
	return true;
}

bool HavocNode::nameAnalysis(SymbolTable *symTab)
{
	return true;
}

bool AssignStmtNode::nameAnalysis(SymbolTable *symTab)
{
	return _assign_node->nameAnalysis(symTab);
}

bool CallStmtNode::nameAnalysis(SymbolTable *symTab)
{
	return _call_exp_node->nameAnalysis(symTab);
}

bool IfStmtNode::nameAnalysis(SymbolTable *symTab)
{
	bool good = _exp_node->nameAnalysis(symTab);
	symTab->enter();
	for (auto *stmt : *_stmt_nodes) {
		good = stmt->nameAnalysis(symTab) && good;
	}
	symTab->leave();
	return good;
}

bool IfElseStmtNode::nameAnalysis(SymbolTable *symTab)
{
	bool good = _exp_node->nameAnalysis(symTab);
	symTab->enter();
	for (auto *stmt : *_stmt_nodes_if_true) {
		good = stmt->nameAnalysis(symTab) && good;
	}
	symTab->leave();
	symTab->enter();
	for (auto *stmt : *_stmt_nodes_if_false) {
		good = stmt->nameAnalysis(symTab) && good;
	}
	symTab->leave();
	return good;
}

bool PostDecStmtNode::nameAnalysis(SymbolTable *symTab)
{
	return _lval_node->nameAnalysis(symTab);
}

bool PostIncStmtNode::nameAnalysis(SymbolTable *symTab)
{
	return _lval_node->nameAnalysis(symTab);
}

bool ReadStmtNode::nameAnalysis(SymbolTable *symTab)
{
	return _lval_node->nameAnalysis(symTab);
}

bool WriteStmtNode::nameAnalysis(SymbolTable *symTab)
{
	return _exp_node->nameAnalysis(symTab);
}

bool WhileStmtNode::nameAnalysis(SymbolTable *symTab)
{
	bool good = _exp_node->nameAnalysis(symTab);
	symTab->enter();
	for (auto *stmt : *_stmt_nodes)
		good = stmt->nameAnalysis(symTab) && good;
	symTab->leave();
	return good;
}

bool ReturnStmtNode::nameAnalysis(SymbolTable *symTab)
{
	if (_exp_node) {
		return _exp_node->nameAnalysis(symTab);
	}
	return true;
}
} // namespace crona
