#ifndef CRONA_3AC_HPP
#define CRONA_3AC_HPP

#include <assert.h>
#include <list>
#include <vector>
#include <map>
#include <set>
#include <string.h>
#include "symbol_table.hpp"
#include "types.hpp"

namespace crona
{
using std::endl;

class TypeAnalysis;
class Procedure;
class IRProgram;

class Label {
    public:
	Label(std::string nameIn)
	{
		this->name = nameIn;
	}
	std::string toString()
	{
		return this->name;
	}
	std::string getName()
	{
		return name;
	}

    private:
	std::string name;
};

#define R64 RegUtils::reg64
#define R8 RegUtils::reg8

enum Register { A, B, C, D, DI, SI, R08, R09, R10, R11, R12, R13, R14, R15 };

class RegUtils {
    public:
	static std::string rootStr(Register reg)
	{
		switch (reg) {
			using crona::Register;
		case A:
			return "a";
		case B:
			return "b";
		case C:
			return "c";
		case D:
			return "d";
		case DI:
			return "di";
		case SI:
			return "si";
		case R08:
			return "r8w";
		case R09:
			return "r9w";
		case R10:
			return "r10w";
		case R11:
			return "r11w";
		case R12:
			return "r12w";
		case R13:
			return "r13w";
		case R14:
			return "r14w";
		case R15:
			return "r15w";
		default:
			throw new InternalError("unexpected register");
		}
	}

	static std::string reg64(Register reg)
	{
		switch (reg) {
			using crona::Register;
		case A:
			return "%rax";
		case B:
			return "%rbx";
		case C:
			return "%rcx";
		case D:
			return "%rdx";
		case DI:
			return "%rdi";
		case SI:
			return "%rsi";
		case R08:
			return "%r8";
		case R09:
			return "%r9";
		case R10:
			return "%r10";
		case R11:
			return "%r11";
		case R12:
			return "%r12";
		case R13:
			return "%r13";
		case R14:
			return "%r14";
		case R15:
			return "%r15";
		default:
			throw new InternalError("unexpected register");
		}
	}

	static std::string reg8(Register reg)
	{
		switch (reg) {
			using crona::Register;
		case A:
			return "%al";
		case B:
			return "%bl";
		case C:
			return "%cl";
		case D:
			return "%dl";
		case DI:
			return "%dil";
		case SI:
			return "%sil";
		case R08:
			return "%r8b";
		case R09:
			return "%r9b";
		case R10:
			return "%r10b";
		case R11:
			return "%r11b";
		case R12:
			return "%r12b";
		case R13:
			return "%r13b";
		case R14:
			return "%r14b";
		case R15:
			return "%r15b";
		default:
			throw new InternalError("unexpected register");
		}
	}

	static void genClear(std::ostream &out, Register reg)
	{
		out << "\txorq " << reg64(reg) << ", " << reg64(reg) << endl;
	}
};

class Opd {
    public:
	Opd(size_t widthIn) : myWidth(widthIn)
	{
	}
	virtual std::string valString() = 0;
	virtual std::string locString() = 0;
	virtual size_t getWidth()
	{
		return myWidth;
	}
	virtual void genLoadAddr(std::ostream &out, Register reg) = 0;
	virtual void genStoreAddr(std::ostream &out, Register reg) = 0;
	virtual void genLoadVal(std::ostream &out, Register reg) = 0;
	virtual void genStoreVal(std::ostream &out, Register reg) = 0;
	static size_t width(const DataType *type)
	{
		if (const BasicType *basic = type->asBasic()) {
			if (basic->isByte()) {
				return 1;
			}
			if (basic->isBool()) {
				return 1;
			}
			return 8;
		} else if (const ArrayType *arr = type->asArray()) {
			return arr->getSize();
		}
		assert(false);
	}
	std::string getMovOp()
	{
		switch (myWidth) {
		case 1:
			return "movb";
		case 8:
			return "movq";
		}
		// This is done when passing an array as an argument. Recall
		// that C-Rona is always pass by value. So we would have to allocate
		// the entire array on the stack. Here are some of the executive steps:
		//  1. Stack would grow down by the array_length * array_base_width
		//  2. St
		std::cout << "array width " << std::to_string(myWidth) << "\n";
		TODO("handle array move");
		throw new InternalError("Bad mov width");
	}
	std::string getGetArg(Register r)
	{
		return getMovOp() + " " + getActualParamLoc() + ", " +
		       getReg(r);
	}
	virtual std::string getReg(Register reg)
	{
		switch (myWidth) {
		case 1:
			return RegUtils::reg8(reg);
		case 8:
			return RegUtils::reg64(reg);
		}
		throw new InternalError("Bad getReg width");
	}
	virtual std::string getMemoryLoc() = 0;
	void setActualParamLoc(std::string loc)
	{
		myActualParamLoc = loc;
	}
	std::string getActualParamLoc()
	{
		return myActualParamLoc;
	}

    protected:
	size_t myWidth;
	std::string myActualParamLoc;
};

class SymOpd : public Opd {
    public:
	virtual std::string valString() override
	{
		return "[" + mySym->ID() + "]";
	}
	virtual std::string locString() override
	{
		return mySym->ID();
	}
	std::string getName()
	{
		return mySym->ID();
	}
	const SemSymbol *getSym()
	{
		return mySym;
	}
	virtual void genLoadVal(std::ostream &out, Register reg) override;
	virtual void genStoreVal(std::ostream &out, Register reg) override;
	virtual void genLoadAddr(std::ostream &out, Register reg) override;
	virtual void genStoreAddr(std::ostream &out, Register reg) override
	{
		throw new InternalError("Cannot change the addr of a symOpd");
	}
	virtual void setMemoryLoc(std::string loc)
	{
		myLoc = loc;
	}
	virtual std::string getMemoryLoc() override
	{
		return myLoc;
	}
	virtual std::string getReg(Register reg) override
	{
		size_t width;
		if (mySym->type()->isArray()) {
			width = Opd::width(ArrayType::baseType(mySym->type()));
		} else {
			width = myWidth;
		}
		switch (width) {
		case 1:
			return RegUtils::reg8(reg);
		case 8:
			return RegUtils::reg64(reg);
		}
		throw new InternalError("Bad getReg width");
	}

    private:
	//Private Constructor
	SymOpd(SemSymbol *sym, size_t width) : Opd(width), mySym(sym)
	{
	}
	SemSymbol *mySym;
	bool isGlobal = false;
	friend class Procedure;
	friend class IRProgram;
	std::string myLoc;
};

class LitOpd : public Opd {
    public:
	LitOpd(std::string valIn, size_t width) : Opd(width), val(valIn)
	{
	}
	static LitOpd *buildInt(int val)
	{
		if (val < 256) {
			return new LitOpd(std::to_string(val), 1);
		} else {
			return new LitOpd(std::to_string(val), 8);
		}
	}
	static LitOpd *buildBool(bool val)
	{
		if (val) {
			return new LitOpd("1", 1);
		} else {
			return new LitOpd("0", 1);
		}
	}

	virtual std::string valString() override
	{
		return val;
	}
	virtual std::string locString() override
	{
		throw InternalError("Tried to get location of a constant");
	}
	virtual void genLoadVal(std::ostream &out, Register reg) override;
	virtual void genStoreVal(std::ostream &out, Register reg) override
	{
		throw new InternalError("Cannot change value of a literal");
	}
	virtual void genLoadAddr(std::ostream &out, Register reg) override
	{
		throw new InternalError("Cannot get addr of a literal");
	}
	virtual void genStoreAddr(std::ostream &out, Register reg) override
	{
		throw new InternalError("Cannot set the addr of a literal");
	}

	virtual std::string getMemoryLoc() override
	{
		throw InternalError("Tried to get location of a constant");
	}

    private:
	std::string val;
	std::string name;
};

class AuxOpd : public Opd {
    public:
	AuxOpd(std::string nameIn, size_t width, size_t basewidth)
		: Opd(width), name(nameIn), basewidth(basewidth)
	{
	}
	std::string valString() override
	{
		return "[" + getName() + "]";
	}
	std::string locString() override
	{
		return getName();
	}
	std::string getName()
	{
		return name;
	}
	size_t getBaseWidth()
	{
		return basewidth;
	}
	virtual void genLoadVal(std::ostream &out, Register reg) override;
	virtual void genStoreVal(std::ostream &out, Register reg) override;
	virtual void genLoadAddr(std::ostream &out, Register reg) override;
	virtual void genStoreAddr(std::ostream &out, Register reg) override
	{
		throw new InternalError("Cannot change the addr of a auxOpd");
	}

	virtual std::string getReg(Register reg) override
	{
		switch (basewidth) {
		case 1:
			return RegUtils::reg8(reg);
		case 8:
			return RegUtils::reg64(reg);
		}
		throw new InternalError("Bad getReg width");
	}

	virtual void setMemoryLoc(std::string loc)
	{
		myLoc = loc;
	}
	virtual std::string getMemoryLoc() override
	{
		return myLoc;
	}

    private:
	std::string name;
	std::string myLoc = "UNINIT";
	size_t basewidth = 0;
};

class AddrOpd : public Opd {
    public:
	AddrOpd(std::string nameIn, size_t width) : Opd(width), name(nameIn)
	{
	}
	virtual std::string valString() override
	{
		return "[" + getName() + "]";
	}
	virtual std::string locString() override
	{
		return getName();
	}
	virtual void genLoadAddr(std::ostream &out, Register reg) override;
	virtual void genStoreAddr(std::ostream &out, Register reg) override;
	virtual void genLoadVal(std::ostream &out, Register reg) override;
	virtual void genStoreVal(std::ostream &out, Register reg) override;

	virtual void setMemoryLoc(std::string loc)
	{
		myLoc = loc;
	}
	virtual std::string getMemoryLoc() override
	{
		return myLoc;
	}
	virtual std::string getName()
	{
		return name;
	}

    private:
	std::string val;
	std::string name;
	std::string myLoc;
	int copyCount = 0;
};

enum BinOp {
	ADD64,
	SUB64,
	DIV64,
	MULT64,
	EQ64,
	NEQ64,
	LT64,
	GT64,
	LTE64,
	GTE64,
	ADD8,
	SUB8,
	DIV8,
	MULT8,
	EQ8,
	NEQ8,
	LT8,
	GT8,
	LTE8,
	GTE8,
	OR8,
	AND8
};
enum UnaryOp { NEG64, NOT8 };

class Quad {
    public:
	Quad();
	void addLabel(Label *label);
	Label *getLabel()
	{
		return labels.front();
	}
	void clearLabels()
	{
		labels.clear();
	}
	virtual std::string repr() = 0;
	std::string commentStr();
	virtual std::string toString(bool verbose = false);
	void setComment(std::string commentIn);
	virtual void codegenX64(std::ostream &out) = 0;
	void codegenLabels(std::ostream &out);

    private:
	std::string myComment;
	std::list<Label *> labels;
};

class BinOpQuad : public Quad {
    public:
	BinOpQuad(Opd *dstIn, BinOp oprIn, Opd *src1In, Opd *src2In);
	std::string repr() override;
	static std::string oprString(BinOp opr);
	void codegenX64(std::ostream &out) override;
	Opd *getDst()
	{
		return dst;
	}
	Opd *getSrc1()
	{
		return src1;
	}
	Opd *getSrc2()
	{
		return src2;
	}
	BinOp getOp()
	{
		return opr;
	}

    private:
	Opd *dst;
	BinOp opr;
	Opd *src1;
	Opd *src2;
};

class UnaryOpQuad : public Quad {
    public:
	UnaryOpQuad(Opd *dstIn, UnaryOp opIn, Opd *srcIn);
	std::string repr() override;
	static std::string oprStringX64(UnaryOp opr)
	{
		switch (opr) {
		case NOT8:
			return "notb";
		case NEG64:
			return "negq";
		default:
			throw InternalError("unrecognize unary op");
		}
	}
	void codegenX64(std::ostream &out) override;
	Opd *getDst()
	{
		return dst;
	}
	Opd *getSrc()
	{
		return src;
	}
	UnaryOp getOp()
	{
		return opr;
	}

    private:
	Opd *dst;
	UnaryOp opr;
	Opd *src;
};

class AssignQuad : public Quad {
    public:
	AssignQuad(Opd *dstIn, Opd *srcIn);
	std::string repr() override;
	void codegenX64(std::ostream &out) override;

    private:
	Opd *dst;
	Opd *src;
};

class IndexQuad : public Quad {
    public:
	IndexQuad(AddrOpd *dstIn, SymOpd *srcIn, Opd *offIn)
		: dst(dstIn), src(srcIn), off(offIn)
	{
	}
	std::string repr() override;
	void codegenX64(std::ostream &out) override;

    private:
	AddrOpd *dst;
	SymOpd *src;
	Opd *off;
};

class JmpQuad : public Quad {
    public:
	JmpQuad(Label *tgtIn);
	std::string repr() override;
	void codegenX64(std::ostream &out) override;
	Label *getLabel()
	{
		return tgt;
	}

    private:
	Label *tgt;
};

class JmpIfQuad : public Quad {
    public:
	JmpIfQuad(Opd *cndIn, Label *tgtIn);
	std::string repr() override;
	void codegenX64(std::ostream &out) override;
	Label *getLabel()
	{
		return tgt;
	}
	Opd *getCnd()
	{
		return cnd;
	}

    private:
	Opd *cnd;
	Label *tgt;
};

class NopQuad : public Quad {
    public:
	NopQuad();
	std::string repr() override;
	void codegenX64(std::ostream &out) override;
};

class IntrinsicOutputQuad : public Quad {
    public:
	IntrinsicOutputQuad(Opd *arg, const DataType *type);
	std::string repr() override;
	void codegenX64(std::ostream &out) override;
	Opd *getSrc()
	{
		return myArg;
	}
	const DataType *getType()
	{
		return myType;
	}

    private:
	Opd *myArg;
	const DataType *myType;
};

class IntrinsicInputQuad : public Quad {
    public:
	IntrinsicInputQuad(Opd *arg, const DataType *type);
	std::string repr() override;
	void codegenX64(std::ostream &out) override;
	Opd *getDst()
	{
		return myArg;
	}

    private:
	Opd *myArg;
	const DataType *myType;
};

class HavocQuad : public Quad {
    public:
	HavocQuad(Opd *dst);
	std::string repr() override;
	void codegenX64(std::ostream &out) override;
	Opd *getDst()
	{
		return myDst;
	}

    private:
	Opd *myDst;
};

class CallQuad : public Quad {
    public:
	CallQuad(SemSymbol *calleeIn);
	std::string repr() override;
	void codegenX64(std::ostream &out) override;

    private:
	SemSymbol *callee;
};

class EnterQuad : public Quad {
    public:
	EnterQuad(Procedure *proc);
	virtual std::string repr() override;
	void codegenX64(std::ostream &out) override;

    private:
	Procedure *myProc;
};

class LeaveQuad : public Quad {
    public:
	LeaveQuad(Procedure *proc);
	virtual std::string repr() override;
	void codegenX64(std::ostream &out) override;

    private:
	Procedure *myProc;
};

class SetArgQuad : public Quad {
    public:
	SetArgQuad(size_t indexIn, Opd *opdIn);
	std::string repr() override;
	void codegenX64(std::ostream &out) override;

    private:
	size_t index;
	Opd *opd;
};

class GetArgQuad : public Quad {
    public:
	GetArgQuad(size_t indexIn, Opd *opdIn);
	std::string repr() override;
	void codegenX64(std::ostream &out) override;

    private:
	size_t index;
	Opd *opd;
};

class SetRetQuad : public Quad {
    public:
	SetRetQuad(Opd *opdIn);
	std::string repr() override;
	void codegenX64(std::ostream &out) override;

    private:
	Opd *opd;
};

class GetRetQuad : public Quad {
    public:
	GetRetQuad(Opd *opdIn);
	std::string repr() override;
	void codegenX64(std::ostream &out) override;

    private:
	Opd *opd;
};

class Procedure {
    public:
	Procedure(IRProgram *prog, std::string name);
	void addQuad(Quad *quad);
	Quad *popQuad();
	IRProgram *getProg();
	std::vector<SymOpd *> getFormals()
	{
		return formals;
	}
	crona::Label *makeLabel();

	void gatherLocal(SemSymbol *sym);
	void gatherFormal(SemSymbol *sym);
	SymOpd *getSymOpd(SemSymbol *sym);
	AuxOpd *makeTmp(size_t width, size_t basewidth = 0);
	AddrOpd *makeAddrOpd(size_t width);

	std::string toString(bool verbose = false);
	std::string getName();

	crona::Label *getLeaveLabel();

	void toX64(std::ostream &out);
	size_t arSize() const;
	size_t numTemps() const;

	std::list<Quad *> *getQuads()
	{
		return bodyQuads;
	}
	EnterQuad *getEnter()
	{
		return enter;
	}
	LeaveQuad *getLeave()
	{
		return leave;
	}

    private:
	void allocLocals();
	EnterQuad *enter;
	LeaveQuad *leave;
	Label *leaveLabel;

	IRProgram *myProg;
	std::map<SemSymbol *, SymOpd *> locals;
	std::list<AuxOpd *> temps;
	std::vector<SymOpd *> formals;
	std::list<AddrOpd *> addrOpds;
	std::list<Quad *> *bodyQuads;
	std::string myName;
	size_t maxTmp;
};

class IRProgram {
    public:
	IRProgram(TypeAnalysis *taIn) : ta(taIn)
	{
		procs = new std::list<Procedure *>();
	}
	Procedure *makeProc(std::string name);
	std::list<Procedure *> *getProcs();
	Label *makeLabel();
	Opd *makeString(std::string val);
	void gatherGlobal(SemSymbol *sym);
	SymOpd *getGlobal(SemSymbol *sym);
	size_t opWidth(ASTNode *node);
	const DataType *nodeType(ASTNode *node);

	std::string toString(bool verbose = false);

	void toX64(std::ostream &out);

    private:
	TypeAnalysis *ta;
	size_t max_label = 0;
	size_t str_idx = 0;
	std::list<Procedure *> *procs;
	HashMap<AddrOpd *, std::string> strings;
	std::map<SemSymbol *, SymOpd *> globals;

	void datagenX64(std::ostream &out);
	void allocGlobals();
};

} // namespace crona

#endif
