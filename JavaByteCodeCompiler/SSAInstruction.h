
#ifndef SSAINSTRUCTION_H
#define SSAINSTRUCTION_H

#include "Method.h"
#include <cassert>
#include <string>
#include <vector>
#include <list>
#include <array>


//SSA opcodes, useful for x86 generation
//slides so we don't support mul, div, shifts, but we include anyway
enum SSAopcode
{
	MOV,
	ADD,
	SUB,
	MUL,
	DIV,
	SHL,
	SHR,
	INC,
	DEC,
	CALL,
	CMP,
	JE,
	JNE,
	JL,
	JLE,
	JG,
	JGE,
	JMP,
	RET,
	PHI
};

//jcc array
const std::array<SSAopcode, 6> JCC =
{
	JE,
	JNE,
	JL,
	JLE,
	JG,
	JGE
};


//lot of memory leaks here

namespace SSA
{

	class Operand
	{
	public:
		enum OpType { stack, local, constant, temp, null_op };
	private:
		OpType opType;
		int val;
	public:
		Operand() : opType(null_op), val(0) {}
		Operand(OpType opType, int val) : opType(opType), val(val) {}
		Operand(const Operand& other);
		virtual bool operator==(const Operand& other) const;
		virtual std::string getStr() const;
		virtual OpType getType() const;
		virtual int getVal() const;
		virtual bool isConst() const;
		virtual bool isVar() const;
		virtual bool isUseVar() const;
		virtual int getUse() const;
		virtual bool sameUse(Operand* other) const;
	};

	//Operands with use subscript
	class OperandUse : public Operand
	{
	private:
		int use;
	public:
		OperandUse() : Operand(Operand::null_op, 0), use(-1) {}
		OperandUse(OpType opType, int val, int use) : Operand(opType, val), use(use) {}
		OperandUse(const OperandUse& other);
		OperandUse(const Operand& other);
		OperandUse(const Operand* other) : Operand(*other), use(other->getUse()) {}
		bool operator==(const OperandUse& other) const;
		bool operator==(const Operand* other) const;
		//need to this std::map, not sure why
		bool operator<(const OperandUse& other) const;
		virtual std::string getStr() const;
		virtual int getUse() const;
		//virtual bool sameUse(Operand* other) const;
	};

	class Branch
	{
	private:
		//could make this be a Basic Block instead of index of basic block
		int targetBasicBlock;
	public:
		Branch(int targetBasicBlock) : targetBasicBlock(targetBasicBlock) {}
		std::string getStr() const;
		int getTargetBB() const;
	};

	class Instruction
	{
	public:
		Instruction();
		~Instruction();
		virtual std::string getStr() const = 0;
		virtual SSAopcode getSSAopcode() const = 0;
		virtual Operand* getDest() const;
		virtual Operand* getSrc1() const;
		virtual Operand* getSrc2() const;
		//return pointers to srcs, allows for easy renames by directly modifying the pointers
		virtual std::vector<Operand*> getSrcs() const;
		//setters should always have operands with their uses
		//could overload setters with just Operand instead of OperandUse, but too lazy
		virtual void setDest(OperandUse op);
		virtual void setSrc1(OperandUse op);
		virtual void setSrc2(OperandUse op);
		virtual void setSrcs(std::vector<OperandUse> srcs);
		virtual void updatePhiSrc(int bb, OperandUse op);
		virtual void renamePhiSrc(int bb, OperandUse op);
		virtual bool isPhi() const;
		virtual std::list<std::pair<int, Operand*>> getPhiSrcs();
		//for call instruction
		virtual std::string getMethodName();
		virtual Branch getBranch();
		//call/ret instructions
		virtual bool getIsVoid() const;
	};

	class MovInstruction : public Instruction
	{
	private:
		Operand* dest;
		Operand* src;
	public:
		MovInstruction(Operand* op1, Operand* op2) : dest(op1), src(op2) {}
		std::string getStr() const;
		virtual SSAopcode getSSAopcode() const;
		Operand* getDest() const;
		virtual Operand* getSrc1() const;
		virtual std::vector<Operand*> getSrcs() const;
		void setDest(OperandUse op);
		void setSrc1(OperandUse op);
		virtual void setSrcs(std::vector<OperandUse> srcs);
	};

	class UnaryInstruction : public Instruction
	{
	public:
		enum Opcode { inc, dec };
	private:
		Opcode opcode;
		Operand* dest;
		//src should always be constant
		Operand* src;
	public:
		UnaryInstruction(Opcode opcode, Operand* dest, Operand* op2) : opcode(opcode), dest(dest), src(op2) {}
		std::string getStr() const;
		virtual SSAopcode getSSAopcode() const;
		Operand* getDest() const;
		virtual Operand* getSrc1() const;
		virtual std::vector<Operand*> getSrcs() const;
		void setDest(OperandUse op);
		void setSrc1(OperandUse op);
		virtual void setSrcs(std::vector<OperandUse> srcs);
	};

	class BinaryInstruction : public Instruction
	{
	public:
		enum Opcode { add, sub, mul, div, shl, shr };
	private:
		Opcode opcode;
		Operand* dest;
		Operand* src1;
		Operand* src2;
	public:
		BinaryInstruction(Opcode opcode, Operand* dest, Operand* op1, Operand* op2) : opcode(opcode), dest(dest), src1(op1), src2(op2) {}
		std::string getStr() const;
		virtual SSAopcode getSSAopcode() const;
		Operand* getDest() const;
		virtual Operand* getSrc1() const;
		virtual Operand* getSrc2() const;
		virtual std::vector<Operand*> getSrcs() const;
		void setDest(OperandUse op);
		void setSrc1(OperandUse op);
		void setSrc2(OperandUse op);
		virtual void setSrcs(std::vector<OperandUse> srcs);
	};

	class CmpInstruction : public Instruction
	{
	private:
		Operand* cond;
		Operand* src1;
		Operand* src2;
	public:
		CmpInstruction(Operand* cond, Operand* op1, Operand* op2) : cond(cond), src1(op1), src2(op2) {}
		std::string getStr() const;
		virtual SSAopcode getSSAopcode() const;
		Operand* getDest() const;
		virtual Operand* getSrc1() const;
		virtual Operand* getSrc2() const;
		virtual std::vector<Operand*> getSrcs() const;
		void setDest(OperandUse op);
		void setSrc1(OperandUse op);
		void setSrc2(OperandUse op);
		virtual void setSrcs(std::vector<OperandUse> srcs);
	};

	class CondBranchInstruction : public Instruction
	{
	public:
		enum Opcode { eq, ne, lt, lte, gt, gte};
	private:
		Opcode opcode;
		Operand* cond;
		Branch branch1;
		Branch branch2;
	public:
		CondBranchInstruction(Opcode opcode, Operand* cond, Branch branch1, Branch branch2);
		std::string getStr() const;
		virtual SSAopcode getSSAopcode() const;
		Operand* getSrc1() const;
		virtual std::vector<Operand*> getSrcs() const;
		void setSrc1(OperandUse op);
		virtual void setSrcs(std::vector<OperandUse> srcs);
		//returns branch2, because we assume thats the branch thats targets the basic block NOT right after branch
		virtual Branch getBranch();
	};

	class UncondBranchInstruction : public Instruction
	{
	private:
		Branch branch;
	public:
		UncondBranchInstruction(Branch branch1) : branch(branch1) {}
		std::string getStr() const;
		virtual SSAopcode getSSAopcode() const;
		virtual Branch getBranch();
	};

	class CallInstruction : public Instruction
	{
	private:
		std::string methodName;
		bool isVoid; //represents whether the called method is void
		Operand* dest;
		std::list<Operand*> args;
	public:
		CallInstruction(std::string m, bool isVoid, Operand* dest, std::list<Operand*> args) : methodName(m), isVoid(isVoid), dest(dest), args(args){}
		std::string getStr() const;
		virtual std::string getMethodName();
		virtual SSAopcode getSSAopcode() const;
		Operand* getDest() const;
		virtual std::vector<Operand*> getSrcs() const;
		virtual void setSrcs(std::vector<OperandUse> srcs);
		virtual bool getIsVoid() const;
	};

	class ReturnInstruction : public Instruction
	{
	private:
		bool isVoid;
		Operand* src;
	public:
		ReturnInstruction() : isVoid(true), src(new Operand(Operand::null_op, 0)) {}
		ReturnInstruction(Operand* op) : isVoid(false), src(op) {}
		std::string getStr() const;
		virtual SSAopcode getSSAopcode() const;
		virtual Operand* getSrc1() const;
		virtual std::vector<Operand*> getSrcs() const;
		void setSrc1(OperandUse op);
		virtual void setSrcs(std::vector<OperandUse> srcs);
		virtual bool getIsVoid() const;
	};

	//TODO this class
	class PhiInstruction : public Instruction
	{
	private:
		Operand* op;
		std::list<std::pair<int, Operand*>> srcs;
	public:
		PhiInstruction(Operand* op) : op(op) {}
		std::string getStr() const;
		virtual SSAopcode getSSAopcode() const;
		Operand* getDest() const;
		void setDest(OperandUse op);
		//if source with index bb doesn't exist, adds the source op
		//if source with index bb does exist, updates source with op
		virtual void updatePhiSrc(int bb, OperandUse op);
		//rename source with basic block index bb to op
		virtual void renamePhiSrc(int bb, OperandUse op);
		virtual bool isPhi() const;
		virtual std::list<std::pair<int, Operand*>> getPhiSrcs();
	};
}


#endif
