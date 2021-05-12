#include <ostream>
#include "3ac.hpp"
#include <random>
#include <string>

namespace crona
{
void IRProgram::allocGlobals()
{
	for (auto glob : globals) {
		glob.second->setMemoryLoc("gbl_" + glob.second->getName());
	}
	for (auto strPair : strings) {
		strPair.first->setMemoryLoc(strPair.first->getName());
	}
}

void IRProgram::datagenX64(std::ostream &out)
{
	out << ".data" << endl;
	out << ".globl main" << endl;
	for (auto stringPair : strings) {
		out << stringPair.first->getName() << ":\t.asciz "
		    << stringPair.second << endl;
	}
	out << ".align 8\n";
	for (auto glob : globals) {
		if (auto arrType = glob.first->type()->asArray()) {
			std::string typeStr =
				Opd::width(ArrayType::baseType(arrType)) == 1 ?
					      ".byte" :
					      ".quad";
			out << glob.second->getMemoryLoc() << ":\t" << typeStr
			    << " ";
			for (int i = 0; i < arrType->getLength(); i++) {
				out << "0";
				if (i != arrType->getLength() - 1)
					out << ", ";
			}
			out << endl;
		} else {
			std::string typeStr = glob.second->getWidth() == 1 ?
							    ".byte" :
							    ".quad";
			out << glob.second->getMemoryLoc() << ":\t" << typeStr
			    << " 0" << endl;
		}
	}
}

void IRProgram::toX64(std::ostream &out)
{
	//Allocate space for globals
	allocGlobals();
	datagenX64(out);

	out << ".text" << endl;
	// Iterate over each procedure and codegen it
	for (auto proc : *procs) {
		proc->toX64(out);
	}
}

void Procedure::allocLocals()
{
	size_t offset = 16;
	for (auto local : locals) {
		offset += local.second->getWidth();
		std::string memLoc = "-" + std::to_string(offset) + "(%rbp)";
		local.second->setMemoryLoc(memLoc);
	}
	for (auto tmp : temps) {
		offset += tmp->getWidth();
		std::string memLoc = "-" + std::to_string(offset) + "(%rbp)";
		tmp->setMemoryLoc(memLoc);
	}
	size_t formalsSize = formals.size();
	for (size_t i = 0; i < formalsSize; i++) {
		offset += formals[i]->getWidth();
		std::string memLoc = "-" + std::to_string(offset) + "(%rbp)";
		formals[i]->setMemoryLoc(memLoc);
		if (i >= 6) {
			std::string actualLoc =
				std::to_string(8 * (formalsSize - i - 1)) +
				"(%rbp)";
			formals[i]->setActualParamLoc(actualLoc);
		}
	}
	for (auto loc : addrOpds) {
		offset += 8;
		std::string memLoc = "-" + std::to_string(offset) + "(%rbp)";
		loc->setMemoryLoc(memLoc);
	}
}

void Procedure::toX64(std::ostream &out)
{
	//Allocate all locals
	allocLocals();

	enter->codegenLabels(out);
	enter->codegenX64(out);
	out << "#Fn body " << myName << endl;
	for (auto quad : *bodyQuads) {
		out << "#" << quad->toString() << endl;
		quad->codegenLabels(out);
		quad->codegenX64(out);
	}
	out << "#Fn epilogue " << myName << endl;
	leave->codegenLabels(out);
	leave->codegenX64(out);
}

void Quad::codegenLabels(std::ostream &out)
{
	if (labels.empty()) {
		return;
	}

	for (Label *label : labels) {
		out << label->toString() << ": ";
		if (label != labels.back()) {
			out << endl;
		}
	}
}

void BinOpQuad::codegenX64(std::ostream &out)
{
	src1->genLoadVal(out, A);
	src2->genLoadVal(out, DI);
	switch (opr) {
	case DIV8:
		out << "\tdivb " << src2->getReg(DI) << endl;
		dst->genStoreVal(out, A);
		return;
	case DIV64:
		out << "\tcqo" << endl;
		out << "\tidivq " << src2->getReg(DI) << endl;
		dst->genStoreVal(out, A);
		return;
	case MULT8:
		out << "\tmulb " << src2->getReg(DI) << endl;
		dst->genStoreVal(out, A);
		return;
	default:
		break;
	}

	std::string instr;
	switch (opr) {
	case ADD8:
		instr = "addb";
		break;
	case ADD64:
		instr = "addq";
		break;
	case SUB8:
		instr = "subb";
		break;
	case SUB64:
		instr = "subq";
		break;
	case MULT64:
		instr = "imulq";
		break;
	case OR8:
		instr = "orb";
		break;
	case AND8:
		instr = "andb";
		break;
	default:
		break;
	}
	if (instr != "") {
		out << '\t' << instr << " " << dst->getReg(DI) << ", "
		    << dst->getReg(A) << endl;
		dst->genStoreVal(out, A);
		return;
	}
	std::string setInstr;
	switch (opr) {
	case EQ8:
		instr = "cmpb";
		setInstr = "sete";
		break;
	case EQ64:
		instr = "cmpq";
		setInstr = "sete";
		break;
	case NEQ8:
		instr = "cmpb";
		setInstr = "setne";
		break;
	case NEQ64:
		instr = "cmpq";
		setInstr = "setne";
		break;
	case LT8:
		instr = "cmpb";
		setInstr = "setb";
		break;
	case LT64:
		instr = "cmpq";
		setInstr = "setnge";
		break;
	case LTE8:
		instr = "cmpb";
		setInstr = "setbe";
		break;
	case LTE64:
		instr = "cmpq";
		setInstr = "setle";
		break;
	case GT8:
		instr = "cmpb";
		setInstr = "seta";
		break;
	case GT64:
		instr = "cmpq";
		setInstr = "setnle";
		break;
	case GTE8:
		instr = "cmpb";
		setInstr = "setae";
		break;
	case GTE64:
		instr = "cmpq";
		setInstr = "setge";
		break;
	default:
		throw InternalError("No such opd");
	}
	out << '\t' << instr << " " << src2->getReg(DI) << ", "
	    << src1->getReg(A) << endl;
	out << '\t' << setInstr << " " << dst->getReg(A) << endl;
	dst->genStoreVal(out, A);
}

void UnaryOpQuad::codegenX64(std::ostream &out)
{
	src->genLoadVal(out, A);
	out << '\t' << oprStringX64(opr) << " " << src->getReg(A) << endl;
	dst->genStoreVal(out, A);
}

void AssignQuad::codegenX64(std::ostream &out)
{
	src->genLoadVal(out, A);
	dst->genStoreVal(out, A);
}

void JmpQuad::codegenX64(std::ostream &out)
{
	out << "\tjmp " << tgt->toString() << endl;
}

void JmpIfQuad::codegenX64(std::ostream &out)
{
	cnd->genLoadVal(out, A);
	out << "\tcmpb $0, " << R8(A) << endl;
	out << "\tje " << tgt->getName() << endl;
}

void NopQuad::codegenX64(std::ostream &out)
{
	out << "\tnop" << endl;
}

void HavocQuad::codegenX64(std::ostream &out)
{
	// Sets %al register to a random 0x0 or 0x1 value
	out << "\tmovq $318, " << R64(A)
	    << endl; // set syscall flag to 318: sys_getrandom
	myDst->genLoadAddr(out, DI); // destination into rdi
	RegUtils::genClear(out, SI);
	RegUtils::genClear(out, D);
	out << "\tmovq $1, " << R64(SI)
	    << endl; // how many bytes to fill the buffer
	out << "\tmovq $0, " << R64(D) << endl;
	out << "\tsyscall" << endl; // syscall
	myDst->genLoadVal(out, C);
	out << "\tandb $1, " << R8(C) << endl; // mask all but final bit of %rcx
	out << "\tcmpb $1, " << R8(C) << endl;
	out << "\tsetnz " << R8(A)
	    << endl; // sets %al to 1 if %rcx != 0, else sets %al to 0
	myDst->genStoreVal(out, A);
}

void IntrinsicOutputQuad::codegenX64(std::ostream &out)
{
	if (myType->isArray()) {
		if (ArrayType::baseType(myType)->isByte()) {
			myArg->genLoadAddr(out, DI);
			out << "\tcallq printStr" << endl;
		}
		return;
	}

	myArg->genLoadVal(out, DI);
	if (myType->isBool()) {
		out << "\tcallq printBool" << endl;
	} else if (myType->isByte()) {
		out << "\tcallq printByte" << endl;
	} else if (myType->isInt()) {
		out << "\tcallq printInt" << endl;
	}
}

void IntrinsicInputQuad::codegenX64(std::ostream &out)
{
	if (myType->isBool()) {
		out << "\tcallq getBool" << endl;
	} else if (myType->isByte()) {
		out << "\tcallq getChar" << endl;
	} else if (myType->isInt()) {
		out << "\tcallq getInt" << endl;
	} else if (myType->isArray() and
		   ArrayType::baseType(myType)->isByte()) {
		myArg->genLoadAddr(out, DI);
		int len = myType->asArray()->getLength();
		RegUtils::genClear(out, SI);
		out << "\tmovq $" << len << ", " << R64(SI) << endl;
		out << "\tcallq getStr" << endl;
		return;
	}
	myArg->genStoreVal(out, A);
}

void CallQuad::codegenX64(std::ostream &out)
{
	std::string fnLabel = callee->ID();
	if (fnLabel != "main") {
		fnLabel = "fun_" + fnLabel;
	}
	out << "\tcallq " << fnLabel << endl;
	// clear pushed args off the stack
	auto args = callee->type()->asFn()->getFormalTypes();
	if (args->size() > 6) {
		out << "\taddq $" << std::to_string(8 * (args->size() - 6))
		    << ", %rsp" << endl;
	}
}

void EnterQuad::codegenX64(std::ostream &out)
{
	out << "\tpushq %rbp" << endl;
	out << "\tmovq %rsp, %rbp" << endl;
	out << "\taddq $16, %rbp" << endl;
	out << "\tsubq $" << myProc->arSize() << ", %rsp" << endl;
}

void LeaveQuad::codegenX64(std::ostream &out)
{
	out << "\taddq $" << myProc->arSize() << ", %rsp" << endl;
	out << "\tpopq %rbp" << endl;
	out << "\tretq" << endl;
}

void SetArgQuad::codegenX64(std::ostream &out)
{
	switch (index) {
	case 1:
		opd->genLoadVal(out, DI);
		break;
	case 2:
		opd->genLoadVal(out, SI);
		break;
	case 3:
		opd->genLoadVal(out, D);
		break;
	case 4:
		opd->genLoadVal(out, C);
		break;
	case 5:
		opd->genLoadVal(out, R08);
		break;
	case 6:
		opd->genLoadVal(out, R09);
		break;
	default:
		opd->genLoadVal(out, A);
		out << "\tpushq " << R64(A) << endl;
		break;
	}
}

void GetArgQuad::codegenX64(std::ostream &out)
{
	switch (index) {
	case 1:
		opd->genStoreVal(out, DI);
		return;
	case 2:
		opd->genStoreVal(out, SI);
		return;
	case 3:
		opd->genStoreVal(out, D);
		return;
	case 4:
		opd->genStoreVal(out, C);
		return;
	case 5:
		opd->genStoreVal(out, R08);
		return;
	case 6:
		opd->genStoreVal(out, R09);
		return;
	default:
		out << '\t' << opd->getGetArg(A) << endl;
		opd->genStoreVal(out, A);
		break;
	}
}

void SetRetQuad::codegenX64(std::ostream &out)
{
	opd->genLoadVal(out, A);
}

void GetRetQuad::codegenX64(std::ostream &out)
{
	opd->genStoreVal(out, A);
}

void IndexQuad::codegenX64(std::ostream &out)
{
	src->genLoadAddr(out, R10);
	off->genLoadVal(out, A);
	out << "\taddq " << R64(A) << ", " << R64(R10) << endl;
	dst->genStoreAddr(out, R10);
}

void SymOpd::genLoadVal(std::ostream &out, Register reg)
{
	out << "#\t\tSymOpd::genLoadVal\n";
	if (this->mySym->type()->isArray()) {
		// load address of array
		out << "\tleaq " << myLoc << ", " << R64(reg) << endl;
		return;
	}
	auto loc = isGlobal ? "(" + myLoc + ")" : myLoc;
	out << "\txorq " << R64(reg) << ", " << R64(reg) << endl;
	out << '\t' << getMovOp() << " " << loc << ", " << getReg(reg) << endl;
}

void SymOpd::genStoreVal(std::ostream &out, Register reg)
{
	out << "#\t\tSymOpd::genStoreVal\n";
	if (this->mySym->type()->isArray()) {
		// store each value of array to location
		size_t arrayLength = static_cast<size_t>(
			mySym->type()->asArray()->getLength());
		size_t baseWidth =
			Opd::width(ArrayType::baseType(mySym->type()));

		std::string movCmd;
		switch (baseWidth) {
		case 1:
			movCmd = "movb";
			break;
		case 8:
			movCmd = "movq";
			break;
		}
		out << "\txorq " << R64(R10) << ", " << R64(R10) << endl;
		out << "\txorq " << R64(R11) << ", " << R64(R11) << endl;
		out << "\tleaq " << myLoc << ", " << R64(R11) << endl;
		for (size_t i = 0; i < arrayLength; i++) {
			out << "\t" << movCmd << " " << (i * baseWidth) << "("
			    << R64(reg) << "), " << getReg(R10) << endl;
			out << "\t" << movCmd << " " << getReg(R10) << ", "
			    << (i * baseWidth) << "(" << R64(R11) << ")"
			    << endl;
		}
		return;
	}
	auto loc = isGlobal ? "(" + myLoc + ")" : myLoc;
	out << '\t' << getMovOp() << " " << getReg(reg) << ", " << loc << endl;
}

void SymOpd::genLoadAddr(std::ostream &out, Register reg)
{
	out << "#\t\tSymOpd::genLoadAddr\n";
	if (isGlobal) {
		out << "\tmovq $" << myLoc << ", " << R64(reg) << endl;
		return;
	}
	out << "\txorq " << R64(reg) << ", " << R64(reg) << endl;
	out << "\tleaq " << myLoc << ", " << R64(reg) << endl;
}

void AuxOpd::genLoadVal(std::ostream &out, Register reg)
{
	out << "#\t\tAuxOpd::genLoadVal\n";
	if (basewidth && basewidth != getWidth()) {
		// load address of array
		out << "\tleaq " << myLoc << ", " << R64(reg) << endl;
		return;
	}
	out << "\txorq " << R64(reg) << ", " << R64(reg) << endl;
	out << '\t' << getMovOp() << " " << myLoc << ", " << getReg(reg)
	    << endl;
}

void AuxOpd::genStoreVal(std::ostream &out, Register reg)
{
	out << "#\t\tAuxOpd::genStoreVal\n";
	if (basewidth && basewidth != getWidth()) {
		// store each value of array to location
		size_t arrayLength = getWidth() / basewidth;
		std::string movCmd;
		switch (basewidth) {
		case 1:
			movCmd = "movb";
			break;
		case 8:
			movCmd = "movq";
			break;
		}
		out << "\txorq " << R64(R10) << ", " << R64(R10) << endl;
		out << "\txorq " << R64(R11) << ", " << R64(R11) << endl;
		out << "\tleaq " << myLoc << ", " << R64(R11) << endl;
		for (size_t i = 0; i < arrayLength; i++) {
			out << "\t" << movCmd << " " << (i * basewidth) << "("
			    << R64(reg) << "), " << getReg(R10) << endl;
			out << "\t" << movCmd << " " << getReg(R10) << ", "
			    << (i * basewidth) << "(" << R64(R11) << ")"
			    << endl;
		}
		return;
	}
	out << '\t' << getMovOp() << " " << getReg(reg) << ", " << myLoc
	    << endl;
}

void AuxOpd::genLoadAddr(std::ostream &out, Register reg)
{
	out << "#\t\tAuxOpd::genLoadAddr\n";
	out << "\txorq " << R64(reg) << ", " << R64(reg) << endl;
	out << "\tleaq "
	    << " " << myLoc << ", " << R64(reg) << endl;
}

void AddrOpd::genLoadVal(std::ostream &out, Register reg)
{
	out << "#\t\tAddrOpd::genLoadVal\n";
	if (reg == R11) {
		throw InternalError(
			"R11 used in AddrOpd::genLoadVal, use a different register");
	}
	if (myLoc.find("str") == 0) {
		// load address of array
		out << "\tleaq " << myLoc << ", " << R64(reg) << endl;
		return;
	}
	out << "\txorq " << R64(R11) << ", " << R64(R11) << endl;
	out << "\txorq " << R64(reg) << ", " << R64(reg) << endl;
	out << "\tmovq"
	    << " " << myLoc << ", " << R64(R11) << endl;
	out << '\t' << getMovOp() << " (" << R64(R11) << "), " << getReg(reg)
	    << endl;
}

void AddrOpd::genStoreVal(std::ostream &out, Register reg)
{
	out << "#\t\tAddrOpd::genStoreVal\n";
	if (reg == R11) {
		throw InternalError(
			"R11 used in AddrOpd::genStoreVal, use a different register");
	}
	if (myLoc.find("str") == 0) {
		// this code should **never** run, but oh well I already wrote it
		std::string movCmd;
		int base = 1;
		out << "\txorq " << R64(R10) << ", " << R64(R10) << endl;
		out << "\txorq " << R64(R11) << ", " << R64(R11) << endl;
		out << "\tleaq " << myLoc << ", " << R64(R11) << endl;
		std::string t_lbl =
			"lbl_" + myLoc + "_cpy" + std::to_string(copyCount++);
		std::string t_lbl_after = t_lbl + "_after";
		out << t_lbl << ": nop" << endl;
		out << "\tcmpb $0, (" << R64(reg) << ")" << endl;
		out << "\tje " << t_lbl << "_after" << endl;
		out << "\tmovb (" << R64(reg) << "), " << R64(R10) << endl;
		out << "\tmovb " << R64(R10) << ", (" << R64(R11) << ")"
		    << endl;
		out << "\tincq " << R64(reg) << endl;
		out << "\tincq" << R64(R11) << endl;
		out << "\tjmp " << t_lbl << endl;
		out << t_lbl_after << ": nop" << endl;
		return;
	}
	out << "\txorq " << R64(R11) << ", " << R64(R11) << endl;
	out << "\tmovq"
	    << " " << myLoc << ", " << R64(R11) << endl;
	out << '\t' << getMovOp() << " " << getReg(reg) << ", (" << R64(R11)
	    << ")" << endl;
}

void AddrOpd::genStoreAddr(std::ostream &out, Register reg)
{
	out << "#\t\tAddrOpd::genStoreAddr\n";
	out << "\tmovq"
	    << " " << R64(reg) << ", " << myLoc << endl;
}

void AddrOpd::genLoadAddr(std::ostream &out, Register reg)
{
	out << "#\t\tAddrOpd::genLoadAddr\n";
	out << "\txorq " << R64(reg) << ", " << R64(reg) << endl;
	if (myLoc.substr(0, 3) == "str") {
		out << '\t' << "movq $" << myLoc << ", " << R64(reg) << endl;
		return;
	}
	out << '\t' << "leaq " << myLoc << ", " << R64(reg) << endl;
}

void LitOpd::genLoadVal(std::ostream &out, Register reg)
{
	out << "\txorq " << R64(reg) << ", " << R64(reg) << endl;
	out << '\t' << getMovOp() << " $" << val << ", " << getReg(reg) << endl;
}

} // namespace crona
