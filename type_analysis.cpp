#include "ast.hpp"
#include "symbol_table.hpp"
#include "errors.hpp"
#include "types.hpp"
#include "name_analysis.hpp"
#include "type_analysis.hpp"

namespace crona
{
TypeAnalysis *TypeAnalysis::build(NameAnalysis *nameAnalysis)
{
	//To emphasize that type analysis depends on name analysis
	// being complete, a name analysis must be supplied for
	// type analysis to be performed.
	TypeAnalysis *typeAnalysis = new TypeAnalysis();
	auto ast = nameAnalysis->ast;
	typeAnalysis->ast = ast;

	ast->typeAnalysis(typeAnalysis);
	if (typeAnalysis->hasError) {
		return nullptr;
	}

	return typeAnalysis;
}

void ProgramNode::typeAnalysis(TypeAnalysis *ta)
{
	//pass the TypeAnalysis down throughout
	// the entire tree, getting the types for
	// each element in turn and adding them
	// to the ta object's hashMap
	for (auto global : *_globals) {
		global->typeAnalysis(ta);
	}

	//The type of the program node will never
	// be needed. We can just set it to VOID
	//(Alternatively, we could make our type
	// be error if the DeclListNode is an error)
	ta->nodeType(this, BasicType::produce(VOID));
}

void FnDeclNode::typeAnalysis(TypeAnalysis *ta)
{
	//HINT: you might want to change the signature for
	// typeAnalysis on FnBodyNode to take a second
	// argument which is the type of the current
	// function. This will help you to know at a
	// return statement whether the return type matches
	// the current function

	//Note: this function may need extra code
	_id->typeAnalysis(ta);
	_type->typeAnalysis(ta);
	for (auto varDecl : *_decl_nodes) {
		varDecl->typeAnalysis(ta);
	}

	auto fnType = ta->nodeType(_id);
	ta->nodeType(this, fnType);

	auto oldFnType = ta->getCurrentFnType();
	ta->setCurrentFnType(ta->nodeType(this)->asFn());
	for (auto stmt : *_stmt_nodes) {
		stmt->typeAnalysis(ta);
	}
	ta->setCurrentFnType(oldFnType);
	ta->nodeType(this, ta->nodeType(this));
}

void StmtNode::typeAnalysis(TypeAnalysis *ta)
{
	TODO("Implement me in the subclass");
}

void AssignStmtNode::typeAnalysis(TypeAnalysis *ta)
{
	_assign_node->typeAnalysis(ta);

	//It can be a bit of a pain to write
	// "const DataType *" everywhere, so here
	// the use of auto is used instead to tell the
	// compiler to figure out what the subType variable
	// should be
	auto subType = ta->nodeType(_assign_node);

	// As error returns null if subType is NOT an error type
	// otherwise, it returns the subType itself
	if (subType->asError()) {
		ta->nodeType(this, subType);
	} else {
		ta->nodeType(this, BasicType::produce(VOID));
	}
}

void ExpNode::typeAnalysis(TypeAnalysis *ta)
{
	TODO("Override me in the subclass");
}

void AssignExpNode::typeAnalysis(TypeAnalysis *ta)
{
	//Do typeAnalysis on the subexpressions
	_lval_node->typeAnalysis(ta);
	_exp_node->typeAnalysis(ta);

	const DataType *tgtType = ta->nodeType(_lval_node);
	const DataType *srcType = ta->nodeType(_exp_node);

	// if rhs is an error, then we have already reported
	// whatever went wrong on the right side and bailed out
	if (srcType->asError()) {
		ta->nodeType(this, ErrorType::produce());
		return;
	}

	//While incomplete, this gives you one case for
	// assignment: if the types are exactly the same
	// it is usually ok to do the assignment. One
	// exception is that if both types are function
	// names, it should fail type analysis
	if (srcType->validVarType() and tgtType->validVarType()) {
		if (tgtType == srcType) {
			ta->nodeType(this, tgtType);
			return;
		} else if (srcType->isArray() and tgtType->isArray()) {
			if (ArrayType::baseType(srcType) ==
				    ArrayType::baseType(srcType) and
			    srcType->asArray()->getSize() ==
				    tgtType->asArray()->getSize()) {
				ta->nodeType(this, tgtType);
				return;
			}
		} else if (tgtType->isInt() and srcType->isByte()) {
			_exp_node = new ByteToIntNode(_exp_node);
			ta->nodeType(_exp_node, tgtType);
			ta->nodeType(this, tgtType);
			return;
		} else {
			ta->errAssignOpr(this->line(), this->col());
			ta->nodeType(this, ErrorType::produce());
			return;
		}
	} else {
		if (!tgtType->validVarType()) {
			ta->errAssignOpd(_lval_node->line(), _lval_node->col());
		}
		if (!srcType->validVarType()) {
			ta->errAssignOpd(_exp_node->line(), _exp_node->col());
		}
		ta->nodeType(this, ErrorType::produce());
		return;
	}

	//Some functions are already defined for you to
	// report type errors. Note that these functions
	// also tell the typeAnalysis object that the
	// analysis has failed, meaning that main.cpp
	// will print "Type check failed" at the end
	// ta->errAssignOpr(this->line(), this->col());

	//Note that reporting an error does not set the
	// type of the current node, so setting the node
	// type must be done
	ta->nodeType(this, ErrorType::produce());
}

void DeclNode::typeAnalysis(TypeAnalysis *ta)
{
	TODO("Override me in the subclass");
}

void VarDeclNode::typeAnalysis(TypeAnalysis *ta)
{
	// VarDecls always pass type analysis, since they
	// are never used in an expression. You may choose
	// to type them void (like this), as discussed in class
	ta->nodeType(this, BasicType::produce(VOID));
}

void IDNode::typeAnalysis(TypeAnalysis *ta)
{
	// IDs never fail type analysis and always
	// yield the type of their symbol (which
	// depends on their definition)
	ta->nodeType(this, _sem_symbol->type());
}

void IntLitNode::typeAnalysis(TypeAnalysis *ta)
{
	// IntLits never fail their type analysis and always
	// yield the type INT
	if (0 <= _value and _value < 256) {
		ta->nodeType(this, BasicType::produce(BYTE));
		return;
	}
	ta->nodeType(this, BasicType::produce(INT));
}

void StrLitNode::typeAnalysis(TypeAnalysis *ta)
{
	// StrLits never fail their type analysis and always
	// yield the type BYTE[length + 1]
	ta->nodeType(this, ArrayType::produce(BasicType::produce(BYTE),
					      _value.size() + 1));
}

void BinaryExpNode::typeAnalysis(TypeAnalysis *ta)
{
	TODO("Override me in the subclass");
}

void AndNode::typeAnalysis(TypeAnalysis *ta)
{
	// check that both operands are bool
	_lhs->typeAnalysis(ta);
	_rhs->typeAnalysis(ta);

	const DataType *lhsType = ta->nodeType(_lhs);
	const DataType *rhsType = ta->nodeType(_rhs);
	if (lhsType->isBool() and rhsType->isBool()) {
		ta->nodeType(this, BasicType::produce(BOOL));
		return;
	}
	if (!lhsType->isBool()) {
		ta->errLogicOpd(_lhs->line(), _lhs->col());
	}
	if (!rhsType->isBool()) {
		ta->errLogicOpd(_rhs->line(), _rhs->col());
	}
	ta->nodeType(this, ErrorType::produce());
}

void OrNode::typeAnalysis(TypeAnalysis *ta)
{
	// check that both operands are bool
	_lhs->typeAnalysis(ta);
	_rhs->typeAnalysis(ta);

	const DataType *lhsType = ta->nodeType(_lhs);
	const DataType *rhsType = ta->nodeType(_rhs);
	if (lhsType->isBool() and rhsType->isBool()) {
		ta->nodeType(this, BasicType::produce(BOOL));
		return;
	}
	if (!lhsType->isBool()) {
		ta->errLogicOpd(_lhs->line(), _lhs->col());
	}
	if (!rhsType->isBool()) {
		ta->errLogicOpd(_rhs->line(), _rhs->col());
	}
	ta->nodeType(this, ErrorType::produce());
}

void PlusNode::typeAnalysis(TypeAnalysis *ta)
{
	_lhs->typeAnalysis(ta);
	_rhs->typeAnalysis(ta);

	auto lhsType = ta->nodeType(_lhs);
	auto rhsType = ta->nodeType(_rhs);
	if (lhsType->isNumeric() and rhsType == lhsType) {
		ta->nodeType(this, lhsType);
		return;
	}
	if (lhsType->isByte() && rhsType->isInt()) {
		ByteToIntNode *promote = new ByteToIntNode(_lhs);
		_lhs = promote;
		ta->nodeType(promote, BasicType::INT());
		ta->nodeType(this, BasicType::INT());
		return;
	}
	if (lhsType->isInt() && rhsType->isByte()) {
		ByteToIntNode *promote = new ByteToIntNode(_rhs);
		_rhs = promote;
		ta->nodeType(promote, BasicType::INT());
		ta->nodeType(this, BasicType::INT());
		return;
	}
	if (not lhsType->isNumeric())
		ta->errMathOpd(_lhs->line(), _lhs->col());
	if (not rhsType->isNumeric())
		ta->errMathOpd(_rhs->line(), _rhs->col());
	ta->nodeType(this, ErrorType::produce());
}

void MinusNode::typeAnalysis(TypeAnalysis *ta)
{
	_lhs->typeAnalysis(ta);
	_rhs->typeAnalysis(ta);

	const DataType *lhsType = ta->nodeType(_lhs);
	const DataType *rhsType = ta->nodeType(_rhs);
	if (lhsType->isNumeric() and rhsType == lhsType) {
		ta->nodeType(this, lhsType);
		return;
	}
	if (lhsType->isByte() && rhsType->isInt()) {
		ByteToIntNode *promote = new ByteToIntNode(_lhs);
		_lhs = promote;
		ta->nodeType(promote, BasicType::INT());
		ta->nodeType(this, BasicType::INT());
		return;
	}
	if (lhsType->isInt() && rhsType->isByte()) {
		ByteToIntNode *promote = new ByteToIntNode(_rhs);
		_rhs = promote;
		ta->nodeType(promote, BasicType::INT());
		ta->nodeType(this, BasicType::INT());
		return;
	}
	if (not lhsType->isNumeric())
		ta->errMathOpd(_lhs->line(), _lhs->col());
	if (not rhsType->isNumeric())
		ta->errMathOpd(_rhs->line(), _rhs->col());
	ta->nodeType(this, ErrorType::produce());
}

void TimesNode::typeAnalysis(TypeAnalysis *ta)
{
	_lhs->typeAnalysis(ta);
	_rhs->typeAnalysis(ta);

	const DataType *lhsType = ta->nodeType(_lhs);
	const DataType *rhsType = ta->nodeType(_rhs);
	if (lhsType->isNumeric() and rhsType == lhsType) {
		ta->nodeType(this, lhsType);
		return;
	}
	if (lhsType->isByte() && rhsType->isInt()) {
		ByteToIntNode *promote = new ByteToIntNode(_lhs);
		_lhs = promote;
		ta->nodeType(promote, BasicType::INT());
		ta->nodeType(this, BasicType::INT());
		return;
	}
	if (lhsType->isInt() && rhsType->isByte()) {
		ByteToIntNode *promote = new ByteToIntNode(_rhs);
		_rhs = promote;
		ta->nodeType(promote, BasicType::INT());
		ta->nodeType(this, BasicType::INT());
		return;
	}
	if (not lhsType->isNumeric())
		ta->errMathOpd(_lhs->line(), _lhs->col());
	if (not rhsType->isNumeric())
		ta->errMathOpd(_rhs->line(), _rhs->col());
	ta->nodeType(this, ErrorType::produce());
}

void DivideNode::typeAnalysis(TypeAnalysis *ta)
{
	_lhs->typeAnalysis(ta);
	_rhs->typeAnalysis(ta);

	const DataType *lhsType = ta->nodeType(_lhs);
	const DataType *rhsType = ta->nodeType(_rhs);
	if (lhsType->isNumeric() and rhsType == lhsType) {
		ta->nodeType(this, lhsType);
		return;
	}
	if (lhsType->isByte() && rhsType->isInt()) {
		ByteToIntNode *promote = new ByteToIntNode(_lhs);
		_lhs = promote;
		ta->nodeType(promote, BasicType::INT());
		ta->nodeType(this, BasicType::INT());
		return;
	}
	if (lhsType->isInt() && rhsType->isByte()) {
		ByteToIntNode *promote = new ByteToIntNode(_rhs);
		_rhs = promote;
		ta->nodeType(promote, BasicType::INT());
		ta->nodeType(this, BasicType::INT());
		return;
	}
	if (not lhsType->isNumeric())
		ta->errMathOpd(_lhs->line(), _lhs->col());
	if (not rhsType->isNumeric())
		ta->errMathOpd(_rhs->line(), _rhs->col());
	ta->nodeType(this, ErrorType::produce());
}

void LessNode::typeAnalysis(TypeAnalysis *ta)
{
	_lhs->typeAnalysis(ta);
	_rhs->typeAnalysis(ta);

	const DataType *lhsType = ta->nodeType(_lhs);
	const DataType *rhsType = ta->nodeType(_rhs);
	if (lhsType->isNumeric() and rhsType == lhsType) {
		ta->nodeType(this, BasicType::produce(BOOL));
		return;
	}
	if (lhsType->isByte() && rhsType->isInt()) {
		ByteToIntNode *promote = new ByteToIntNode(_lhs);
		_lhs = promote;
		ta->nodeType(promote, BasicType::INT());
		ta->nodeType(this, BasicType::BOOL());
		return;
	}
	if (lhsType->isInt() && rhsType->isByte()) {
		ByteToIntNode *promote = new ByteToIntNode(_rhs);
		_rhs = promote;
		ta->nodeType(promote, BasicType::INT());
		ta->nodeType(this, BasicType::BOOL());
		return;
	}
	if (not lhsType->isNumeric())
		ta->errRelOpd(_lhs->line(), _lhs->col());
	if (not rhsType->isNumeric())
		ta->errRelOpd(_rhs->line(), _rhs->col());
	ta->nodeType(this, ErrorType::produce());
}

void LessEqNode::typeAnalysis(TypeAnalysis *ta)
{
	_lhs->typeAnalysis(ta);
	_rhs->typeAnalysis(ta);

	const DataType *lhsType = ta->nodeType(_lhs);
	const DataType *rhsType = ta->nodeType(_rhs);
	if (lhsType->isNumeric() and rhsType == lhsType) {
		ta->nodeType(this, BasicType::produce(BOOL));
		return;
	}
	if (lhsType->isByte() && rhsType->isInt()) {
		ByteToIntNode *promote = new ByteToIntNode(_lhs);
		_lhs = promote;
		ta->nodeType(promote, BasicType::INT());
		ta->nodeType(this, BasicType::BOOL());
		return;
	}
	if (lhsType->isInt() && rhsType->isByte()) {
		ByteToIntNode *promote = new ByteToIntNode(_rhs);
		_rhs = promote;
		ta->nodeType(promote, BasicType::INT());
		ta->nodeType(this, BasicType::BOOL());
		return;
	}
	if (not lhsType->isNumeric())
		ta->errRelOpd(_lhs->line(), _lhs->col());
	if (not rhsType->isNumeric())
		ta->errRelOpd(_rhs->line(), _rhs->col());
	ta->nodeType(this, ErrorType::produce());
}

void EqualsNode::typeAnalysis(TypeAnalysis *ta)
{
	_lhs->typeAnalysis(ta);
	_rhs->typeAnalysis(ta);

	const DataType *lhsType = ta->nodeType(_lhs);
	const DataType *rhsType = ta->nodeType(_rhs);
	// if both operands are numeric or booleans, then only
	// the good vibes
	if (lhsType->isBool() and lhsType->isBool()) {
		ta->nodeType(this, BasicType::produce(BOOL));
		return;
	}
	if (lhsType->isNumeric() and rhsType == lhsType) {
		ta->nodeType(this, BasicType::produce(BOOL));
		return;
	}
	if (lhsType->isByte() && rhsType->isInt()) {
		ByteToIntNode *promote = new ByteToIntNode(_lhs);
		_lhs = promote;
		ta->nodeType(promote, BasicType::INT());
		ta->nodeType(this, BasicType::BOOL());
		return;
	}
	if (lhsType->isInt() && rhsType->isByte()) {
		ByteToIntNode *promote = new ByteToIntNode(_rhs);
		_rhs = promote;
		ta->nodeType(promote, BasicType::INT());
		ta->nodeType(this, BasicType::BOOL());
		return;
	}
	// If bad types
	if (lhsType->isVoid() or lhsType->isArray() or lhsType->asFn())
		ta->errEqOpd(_lhs->line(), _lhs->col());
	if (rhsType->isVoid() or rhsType->isArray() or rhsType->asFn())
		ta->errEqOpd(_rhs->line(), _rhs->col());

	// OK types but bad comparison
	if ((lhsType->isNumeric() and rhsType->isBool()) or
	    (rhsType->isNumeric() and lhsType->isBool()))
		ta->errEqOpr(_lhs->line(), _lhs->col());

	// Produce the error
	ta->nodeType(this, ErrorType::produce());
}

void NotEqualsNode::typeAnalysis(TypeAnalysis *ta)
{
	_lhs->typeAnalysis(ta);
	_rhs->typeAnalysis(ta);

	const DataType *lhsType = ta->nodeType(_lhs);
	const DataType *rhsType = ta->nodeType(_rhs);
	if (lhsType->isBool() and lhsType->isBool()) {
		ta->nodeType(this, BasicType::produce(BOOL));
		return;
	}
	if (lhsType->isNumeric() and rhsType == lhsType) {
		ta->nodeType(this, BasicType::produce(BOOL));
		return;
	}
	if (lhsType->isByte() && rhsType->isInt()) {
		ByteToIntNode *promote = new ByteToIntNode(_lhs);
		_lhs = promote;
		ta->nodeType(promote, BasicType::INT());
		ta->nodeType(this, BasicType::BOOL());
		return;
	}
	if (lhsType->isInt() && rhsType->isByte()) {
		ByteToIntNode *promote = new ByteToIntNode(_rhs);
		_rhs = promote;
		ta->nodeType(promote, BasicType::INT());
		ta->nodeType(this, BasicType::BOOL());
		return;
	}

	// If bad types
	if (lhsType->isVoid() or lhsType->isArray() or
	    lhsType->asFn() != nullptr)
		ta->errEqOpd(_lhs->line(), _lhs->col());
	if (rhsType->isVoid() or rhsType->isArray() or
	    rhsType->asFn() != nullptr)
		ta->errEqOpd(_rhs->line(), _rhs->col());

	// OK types but bad comparison
	if ((lhsType->isNumeric() and rhsType->isBool()) or
	    (rhsType->isNumeric() and lhsType->isBool()))
		ta->errEqOpr(_lhs->line(), _lhs->col());

	// Produce the error
	ta->nodeType(this, ErrorType::produce());
}

void GreaterEqNode::typeAnalysis(TypeAnalysis *ta)
{
	_lhs->typeAnalysis(ta);
	_rhs->typeAnalysis(ta);

	const DataType *lhsType = ta->nodeType(_lhs);
	const DataType *rhsType = ta->nodeType(_rhs);
	if (lhsType->isNumeric() and rhsType == lhsType) {
		ta->nodeType(this, BasicType::produce(BOOL));
		return;
	}
	if (lhsType->isByte() && rhsType->isInt()) {
		ByteToIntNode *promote = new ByteToIntNode(_lhs);
		_lhs = promote;
		ta->nodeType(promote, BasicType::INT());
		ta->nodeType(this, BasicType::BOOL());
		return;
	}
	if (lhsType->isInt() && rhsType->isByte()) {
		ByteToIntNode *promote = new ByteToIntNode(_rhs);
		_rhs = promote;
		ta->nodeType(promote, BasicType::INT());
		ta->nodeType(this, BasicType::BOOL());
		return;
	}
	if (not lhsType->isNumeric())
		ta->errRelOpd(_lhs->line(), _lhs->col());
	if (not rhsType->isNumeric())
		ta->errRelOpd(_rhs->line(), _rhs->col());
	ta->nodeType(this, ErrorType::produce());
}

void GreaterNode::typeAnalysis(TypeAnalysis *ta)
{
	_lhs->typeAnalysis(ta);
	_rhs->typeAnalysis(ta);

	const DataType *lhsType = ta->nodeType(_lhs);
	const DataType *rhsType = ta->nodeType(_rhs);
	if (lhsType->isNumeric() and rhsType == lhsType) {
		ta->nodeType(this, BasicType::produce(BOOL));
		return;
	}
	if (lhsType->isByte() && rhsType->isInt()) {
		ByteToIntNode *promote = new ByteToIntNode(_lhs);
		_lhs = promote;
		ta->nodeType(promote, BasicType::INT());
		ta->nodeType(this, BasicType::BOOL());
		return;
	}
	if (lhsType->isInt() && rhsType->isByte()) {
		ByteToIntNode *promote = new ByteToIntNode(_rhs);
		_rhs = promote;
		ta->nodeType(promote, BasicType::INT());
		ta->nodeType(this, BasicType::BOOL());
		return;
	}
	if (not lhsType->isNumeric())
		ta->errRelOpd(_lhs->line(), _lhs->col());
	if (not rhsType->isNumeric())
		ta->errRelOpd(_rhs->line(), _rhs->col());
	ta->nodeType(this, ErrorType::produce());
}

void NegNode::typeAnalysis(TypeAnalysis *ta)
{
	_exp_node->typeAnalysis(ta);
	const DataType *subType = ta->nodeType(_exp_node);

	//Propagate error, don't re-report
	if (subType->asError()) {
		ta->nodeType(this, subType);
		return;
	} else if (subType->isInt()) {
		ta->nodeType(this, BasicType::INT());
	} else if (subType->isByte()) {
		ByteToIntNode *promote = new ByteToIntNode(_exp_node);
		_exp_node = promote;
		ta->nodeType(promote, BasicType::INT());
		ta->nodeType(this, BasicType::INT());
	} else {
		ta->errMathOpd(_exp_node->line(), _exp_node->col());
		ta->nodeType(this, ErrorType::produce());
	}
}

void NotNode::typeAnalysis(TypeAnalysis *ta)
{
	// check that operand is bool
	_exp_node->typeAnalysis(ta);

	const DataType *expType = ta->nodeType(_exp_node);
	if (expType->isBool()) {
		ta->nodeType(this, BasicType::produce(BOOL));
		return;
	}
	ta->errLogicOpd(_exp_node->line(), _exp_node->col());
	ta->nodeType(this, ErrorType::produce());
}

void IndexNode::typeAnalysis(TypeAnalysis *ta)
{
	_id->typeAnalysis(ta);
	_offset->typeAnalysis(ta);

	const DataType *idType = ta->nodeType(_id);
	const DataType *indexType = ta->nodeType(_offset);

	if (idType->isArray() and indexType->isNumeric()) {
		ta->nodeType(this, ArrayType::baseType(idType));
		return;
	}
	if (not idType->isArray())
		ta->errArrayID(_id->line(), _id->col());
	if (not indexType->isNumeric())
		ta->errArrayIndex(_offset->line(), _offset->col());
	ta->nodeType(this, ErrorType::produce());
}

void CallExpNode::typeAnalysis(TypeAnalysis *typing)
{
	std::list<const DataType *> *aList = new std::list<const DataType *>();
	for (auto actual : *_id_params) {
		actual->typeAnalysis(typing);
		aList->push_back(typing->nodeType(actual));
	}

	SemSymbol *calleeSym = _id_func->getSymbol();
	assert(calleeSym != nullptr);
	const DataType *calleeType = calleeSym->type();
	const FnType *fnType = calleeType->asFn();
	if (fnType == nullptr) {
		typing->errCallee(_id_func->line(), _id_func->col());
		typing->nodeType(this, ErrorType::produce());
		return;
	}

	const std::list<const DataType *> *fList = fnType->getFormalTypes();
	if (aList->size() != fList->size()) {
		typing->errArgCount(line(), col());
		//Note: we still consider the call to return the
		// return type
	} else {
		auto actualTypesItr = aList->begin();
		auto formalTypesItr = fList->begin();
		auto actualsItr = _id_params->begin();
		while (actualTypesItr != aList->end()) {
			const DataType *actualType = *actualTypesItr;
			const DataType *formalType = *formalTypesItr;
			ExpNode *actual = *actualsItr;
			auto actualsItrOld = actualsItr;
			actualTypesItr++;
			formalTypesItr++;
			actualsItr++;

			//Matching to error is ignored
			if (actualType->asError()) {
				continue;
			}
			if (formalType->asError()) {
				continue;
			}

			//Ok match
			if (formalType == actualType) {
				continue;
			}

			const ArrayType *formalArr = formalType->asArray();
			const ArrayType *actualArr = actualType->asArray();
			if (formalArr && actualArr) {
				if (ArrayType::baseType(formalArr) ==
				    ArrayType::baseType(actualArr)) {
					continue;
				}
			}

			//Promote
			if (formalType->isInt() && actualType->isByte()) {
				//Promote
				ByteToIntNode *up = new ByteToIntNode(actual);
				typing->nodeType(up, BasicType::INT());

				actualsItrOld =
					_id_params->erase(actualsItrOld);
				actualsItrOld =
					_id_params->insert(actualsItrOld, up);

				continue;
			}

			//Bad match
			typing->errArgMatch(actual->line(), actual->col());
			typing->nodeType(this, ErrorType::produce());
		}
	}

	typing->nodeType(this, fnType->getReturnType());
	return;
}

void ByteToIntNode::typeAnalysis(TypeAnalysis *typing)
{
	//We never really expect this to happen, but we can handle it
	typing->nodeType(this, BasicType::INT());
}

void TrueNode::typeAnalysis(TypeAnalysis *ta)
{
	ta->nodeType(this, BasicType::produce(BOOL));
}

void FalseNode::typeAnalysis(TypeAnalysis *ta)
{
	ta->nodeType(this, BasicType::produce(BOOL));
}

void HavocNode::typeAnalysis(TypeAnalysis *ta)
{
	ta->nodeType(this, BasicType::produce(BOOL));
}

void CallStmtNode::typeAnalysis(TypeAnalysis *ta)
{
	_call_exp_node->typeAnalysis(ta);
	//It can be a bit of a pain to write
	// "const DataType *" everywhere, so here
	// the use of auto is used instead to tell the
	// compiler to figure out what the subType variable
	// should be
	auto subType = ta->nodeType(_call_exp_node);

	// As error returns null if subType is NOT an error type
	// otherwise, it returns the subType itself
	if (subType->asError()) {
		ta->nodeType(this, subType);
	} else {
		ta->nodeType(this, BasicType::produce(VOID));
	}
}

void IfStmtNode::typeAnalysis(TypeAnalysis *ta)
{
	_exp_node->typeAnalysis(ta);
	// If the condition is an error, the error has already
	// cascaded and we don't explicitly report the error
	if (ta->nodeType(_exp_node)->asError()) {
		ta->nodeType(this, ErrorType::produce());
		return;
	}

	if (!ta->nodeType(_exp_node)->isBool()) {
		ta->errIfCond(_exp_node->line(), _exp_node->col());
	}

	for (auto *stmt : *_stmt_nodes) {
		stmt->typeAnalysis(ta);
	}
	ta->nodeType(this, BasicType::produce(VOID));
}

void IfElseStmtNode::typeAnalysis(TypeAnalysis *ta)
{
	_exp_node->typeAnalysis(ta);

	// If the condition is an error, the error has already
	// cascaded and we don't explicitly report the error
	if (ta->nodeType(_exp_node)->asError()) {
		ta->nodeType(this, ErrorType::produce());
		return;
	}

	if (!ta->nodeType(_exp_node)->isBool()) {
		ta->errIfCond(_exp_node->line(), _exp_node->col());
	}

	for (auto *stmt : *_stmt_nodes_if_true) {
		stmt->typeAnalysis(ta);
	}
	for (auto *stmt : *_stmt_nodes_if_false) {
		stmt->typeAnalysis(ta);
	}
	ta->nodeType(this, BasicType::produce(VOID));
}

void PostDecStmtNode::typeAnalysis(TypeAnalysis *ta)
{
	_lval_node->typeAnalysis(ta);
	const DataType *lvalType = ta->nodeType(_lval_node);
	if (lvalType->isNumeric()) {
		auto toReturn = INT;
		if (lvalType->isByte())
			toReturn = BYTE;
		ta->nodeType(this, BasicType::produce(toReturn));
		return;
	}
	ta->errMathOpd(_lval_node->line(), _lval_node->col());
	ta->nodeType(this, ErrorType::produce());
}

void PostIncStmtNode::typeAnalysis(TypeAnalysis *ta)
{
	_lval_node->typeAnalysis(ta);
	const DataType *lvalType = ta->nodeType(_lval_node);
	if (lvalType->isNumeric()) {
		auto toReturn = INT;
		if (lvalType->isByte())
			toReturn = BYTE;
		ta->nodeType(this, BasicType::produce(toReturn));
		return;
	}
	ta->errMathOpd(_lval_node->line(), _lval_node->col());
	ta->nodeType(this, ErrorType::produce());
}

void ReadStmtNode::typeAnalysis(TypeAnalysis *ta)
{
	_lval_node->typeAnalysis(ta);
	auto lvalType = ta->nodeType(_lval_node);
	if (lvalType->isBool() or lvalType->isInt() or
	    (lvalType->isArray() and ArrayType::baseType(lvalType)->isByte())) {
		ta->nodeType(this, BasicType::produce(VOID));
		return;
	}
	if (lvalType->asFn()) {
		ta->errReadFn(_lval_node->line(), _lval_node->col());
	} else {
		// other read errors
		// reading an array other than byte
		// reading an rvalue (read 7)
	}
	ta->nodeType(this, ErrorType::produce());
}

void WriteStmtNode::typeAnalysis(TypeAnalysis *ta)
{
	_exp_node->typeAnalysis(ta);
	const DataType *lvalType = ta->nodeType(_exp_node);
	if (lvalType->isInt() or lvalType->isBool() or
	    (lvalType->isArray() and ArrayType::baseType(lvalType)->isByte())) {
		ta->nodeType(this, BasicType::produce(VOID));
		return;
	}
	if (lvalType->asFn()) // writing a function
		ta->errWriteFn(_exp_node->line(), _exp_node->col());
	if (lvalType->isVoid()) // writing a void
		ta->errWriteVoid(_exp_node->line(), _exp_node->col());
	if (lvalType->isArray()) // writing a non-byte array
		ta->errWriteArray(_exp_node->line(), _exp_node->col());
	// Mark this node as an error
	ta->nodeType(this, ErrorType::produce());
}

void WhileStmtNode::typeAnalysis(TypeAnalysis *ta)
{
	_exp_node->typeAnalysis(ta);

	// If the condition is an error, the error has already
	// cascaded and we don't explicitly report the error
	if (ta->nodeType(_exp_node)->asError()) {
		ta->nodeType(this, ErrorType::produce());
		return;
	}

	if (not ta->nodeType(_exp_node)->isBool()) {
		ta->errWhileCond(_exp_node->line(), _exp_node->col());
		ta->nodeType(this, ErrorType::produce());
		return;
	}
	for (auto *stmt : *_stmt_nodes)
		stmt->typeAnalysis(ta);
	ta->nodeType(this, BasicType::produce(VOID));
}

void ReturnStmtNode::typeAnalysis(TypeAnalysis *ta)
{
	auto fnRetType = ta->getCurrentFnType()->getReturnType();
	if (not fnRetType->isVoid() and not _exp_node) {
		ta->errRetEmpty(line(), col());
		return;
	}
	if (fnRetType->isVoid()) {
		ta->errRetValueFromVoid(_exp_node->line(), _exp_node->col());
		return;
	}
	if (_exp_node) {
		_exp_node->typeAnalysis(ta);
		auto retType = ta->nodeType(_exp_node);
		if (retType == fnRetType or retType->asError()) {
			ta->nodeType(this, fnRetType);
			return;
		}
		if (retType->isByte() and fnRetType->isInt()) {
			_exp_node = new ByteToIntNode(_exp_node);
			ta->nodeType(_exp_node, BasicType::INT());
			return;
		}
		ta->errRetWrong(_exp_node->line(), _exp_node->col());
	}
	ta->nodeType(this, ErrorType::produce());
}

void TypeNode::typeAnalysis(TypeAnalysis *ta)
{
	ta->nodeType(this, getType());
}

} /* namespace crona */
