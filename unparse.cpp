#include "ast.hpp"
#include "symbol_table.hpp"

namespace crona
{
/*
doIndent is declared static, which means that it can 
only be called in this file (its symbol is not exported).
*/
static void doIndent(std::ostream &out, int indent)
{
	for (int k = 0; k < indent; k++) {
		out << "\t";
	}
}

/*
In this code, the intention is that functions are grouped 
into files by purpose, rather than by class.
If you're used to having all of the functions of a class 
defined in the same file, this style may be a bit disorienting,
though it is legal. Thus, we can have
ProgramNode::unparse, which is the unparse method of ProgramNodes
defined in the same file as DeclNode::unparse, the unparse method
of DeclNodes.
*/

void ProgramNode::unparse(std::ostream &out, int indent)
{
	for (auto global : *_globals) {
		global->unparse(out, indent);
	}
}

void AssignExpNode::unparse(std::ostream &out, int indent)
{
	doIndent(out, indent);
	_lval_node->unparse(out, 0);
	out << " = ";
	_exp_node->unparse(out, 0);
}

void BinaryExpNode::unparse(std::ostream &out, int indent)
{
	doIndent(out, indent);
	out << "(";
	_lhs->unparse(out, 0);
	out << " " << opStr() << " ";
	_rhs->unparse(out, 0);
	out << ")";
}

void UnaryExpNode::unparse(std::ostream &out, int indent)
{
	doIndent(out, indent);
	out << "(";
	out << this->opStr();
	this->_exp_node->unparse(out, 0);
	out << ")";
}

void IDNode::unparse(std::ostream &out, int indent)
{
	doIndent(out, indent);
	if (_sem_symbol)
		out << _name << "(" << _sem_symbol->typeString() << ")";
	else
		out << _name;
}

void IndexNode::unparse(std::ostream &out, int indent)
{
	doIndent(out, indent);
	_id->unparse(out, 0);
	out << "[";
	_offset->unparse(out, 0);
	out << "]";
}

void CallExpNode::unparse(std::ostream &out, int indent)
{
	doIndent(out, indent);
	_id_func->unparse(out, 0);
	out << "(";
	for (auto *param : *_id_params) {
		param->unparse(out, 0);
		if (param != _id_params->back())
			out << ", ";
	}
	out << ")";
}

void TrueNode::unparse(std::ostream &out, int indent)
{
	doIndent(out, indent);
	out << "true";
}

void FalseNode::unparse(std::ostream &out, int indent)
{
	doIndent(out, indent);
	out << "false";
}

void IntLitNode::unparse(std::ostream &out, int indent)
{
	doIndent(out, indent);
	out << _value;
}

void StrLitNode::unparse(std::ostream &out, int indent)
{
	doIndent(out, indent);
	out << _value;
}

void HavocNode::unparse(std::ostream &out, int indent)
{
	doIndent(out, indent);
	out << "havoc";
}

void AssignStmtNode::unparse(std::ostream &out, int indent)
{
	doIndent(out, indent);
	_assign_node->unparse(out, 0);
	out << ";\n";
}

void CallStmtNode::unparse(std::ostream &out, int indent)
{
	doIndent(out, indent);
	_call_exp_node->unparse(out, 0);
	out << ";\n";
}

void IfStmtNode::unparse(std::ostream &out, int indent)
{
	doIndent(out, indent);
	out << "if (";
	_exp_node->unparse(out, 0);
	out << ") {\n";
	for (auto *stmt : *_stmt_nodes) {
		stmt->unparse(out, indent + 1);
	}
	doIndent(out, indent);
	out << "}\n";
}

void IfElseStmtNode::unparse(std::ostream &out, int indent)
{
	doIndent(out, indent);
	out << "if (";
	_exp_node->unparse(out, 0);
	out << ") {\n";
	for (auto *stmt : *_stmt_nodes_if_true) {
		stmt->unparse(out, indent + 1);
	}
	doIndent(out, indent);
	out << "} ";
	out << "else {\n";
	for (auto *stmt : *_stmt_nodes_if_false) {
		stmt->unparse(out, indent + 1);
	}
	doIndent(out, indent);
	out << "}\n";
}

void PostDecStmtNode::unparse(std::ostream &out, int indent)
{
	doIndent(out, indent);
	_lval_node->unparse(out, 0);
	out << "--;\n";
}

void PostIncStmtNode::unparse(std::ostream &out, int indent)
{
	doIndent(out, indent);
	_lval_node->unparse(out, 0);
	out << "++;\n";
}

void ReadStmtNode::unparse(std::ostream &out, int indent)
{
	doIndent(out, indent);
	out << "read ";
	_lval_node->unparse(out, 0);
	out << ";\n";
}

void WriteStmtNode::unparse(std::ostream &out, int indent)
{
	doIndent(out, indent);
	out << "write ";
	_exp_node->unparse(out, 0);
	out << ";\n";
}

void WhileStmtNode::unparse(std::ostream &out, int indent)
{
	doIndent(out, indent);
	out << "while (";
	_exp_node->unparse(out, 0);
	out << ") {\n";
	for (auto *stmt : *_stmt_nodes)
		stmt->unparse(out, indent + 1);
	doIndent(out, indent);
	out << "}\n";
}

void ReturnStmtNode::unparse(std::ostream &out, int indent)
{
	doIndent(out, indent);
	out << "return";
	if (_exp_node) {
		out << " ";
		_exp_node->unparse(out, 0);
	}
	out << ";\n";
}

void FnDeclNode::unparse(std::ostream &out, int indent)
{
	doIndent(out, indent);
	_id->unparse(out, 0);
	out << " : ";
	_type->unparse(out, 0);
	out << "(";
	for (auto *param : *_decl_nodes) {
		param->unparse(out, 0);
		if (param != _decl_nodes->back())
			out << ", ";
	}
	out << ") {\n";
	for (auto *stmt : *_stmt_nodes)
		stmt->unparse(out, indent + 1);
	doIndent(out, indent);
	out << "}\n";
}

void VarDeclNode::unparse(std::ostream &out, int indent)
{
	doIndent(out, indent);
	_id->unparse(out, 0);
	out << " : ";
	_type->unparse(out, 0);
	out << ";\n";
}

void FormalDeclNode::unparse(std::ostream &out, int indent)
{
	doIndent(out, indent);
	_id->unparse(out, 0);
	out << " : ";
	_type->unparse(out, 0);
}

void IntTypeNode::unparse(std::ostream &out, int indent)
{
	doIndent(out, indent);
	out << "int";
}

void ArrayTypeNode::unparse(std::ostream &out, int indent)
{
	doIndent(out, indent);
	_type_node->unparse(out, 0);
	out << " array";
	out << "[";
	_size->unparse(out, 0);
	out << "]";
}

void BoolTypeNode::unparse(std::ostream &out, int indent)
{
	doIndent(out, indent);
	out << "bool";
}

void StrTypeNode::unparse(std::ostream &out, int indent)
{
	doIndent(out, indent);
	out << "string";
}

void ByteTypeNode::unparse(std::ostream &out, int indent)
{
	doIndent(out, indent);
	out << "byte";
}

void VoidTypeNode::unparse(std::ostream &out, int indent)
{
	doIndent(out, indent);
	out << "void";
}

} // End namespace crona
