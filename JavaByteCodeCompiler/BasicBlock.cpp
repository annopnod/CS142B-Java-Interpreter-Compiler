#include "BasicBlock.h"


//probably should create operator= and deep copy instructions
BasicBlock::~BasicBlock()
{
	/*for(int i = 0; i < instructions.size(); ++i)
	{
		delete instructions[i];
	}*/
}

int BasicBlock::getIndex()
{
	return index;
}

int BasicBlock::getStartLine()
{
	return startLine;
}

int BasicBlock::getEndLine()
{
	return endLine;
}

int BasicBlock::getSSAstart()
{
	return SSAstartLine;
}

int BasicBlock::getSSAend()
{
	return SSAendLine;
}

int BasicBlock::getInStackDepth()
{
	return inStackDepth;
}

std::list<int> BasicBlock::getPred()
{
	return pred;
}

std::list<int> BasicBlock::getSucc()
{
	return succ;
}

std::vector<SSA::Instruction*>& BasicBlock::getInstructions()
{
	return instructions;
}

void BasicBlock::addInstruction(SSA::Instruction * i)
{
	instructions.push_back(i);
}

void BasicBlock::addInstructionBeforeJmp(SSA::Instruction * i)
{
	for (std::vector<SSA::Instruction*>::const_reverse_iterator iter = instructions.crbegin(); iter != instructions.crend(); ++iter)
	{
		//continue if we see cmp, jmp, or jcc
		SSAopcode opcode = (*iter)->getSSAopcode();
		if (opcode == CMP || opcode == JMP || std::find(JCC.cbegin(), JCC.cend(), opcode) != JCC.cend())
		{
			continue;
		}
		//if not cmp, jmp or jcc, insert instruction and return
		//call base() to get const_iter, since insert doesn't accept const_reverse_iter
		instructions.insert(iter.base(), i);
		return;
	}
}

void BasicBlock::addPhi(SSA::Operand op)
{
	instructions.insert(instructions.cbegin(), new SSA::PhiInstruction(new SSA::Operand(op)));
}

void BasicBlock::addPhi(SSA::PhiInstruction * phi)
{
	instructions.insert(instructions.cbegin(), phi);
}

void BasicBlock::updatePhiSrc(SSA::Operand dest, int bb, SSA::OperandUse src)
{
	for (SSA::Instruction* ins : instructions)
	{
		if (*(ins->getDest()) == dest)
		{
			ins->updatePhiSrc(bb, src);
		}
	}
}

void BasicBlock::renamePhiSrc(SSA::Operand dest, int bb, SSA::OperandUse src)
{
	
	for (SSA::Instruction* ins : instructions)
	{
		if (*(ins->getDest()) == dest)
		{
			ins->renamePhiSrc(bb, src);
		}
	}
}

void BasicBlock::addPred(int i)
{
	if(std::find(std::cbegin(pred), std::cend(pred), i) == std::cend(pred))
		pred.push_back(i);
}

void BasicBlock::addSucc(int i)
{
	if(std::find(std::cbegin(succ), std::cend(succ), i) == std::cend(succ))
		succ.push_back(i);
}

void BasicBlock::setInStackDepth(int liveInStackDepth)
{
	this->inStackDepth = liveInStackDepth;
}

void BasicBlock::setSSAstart(int i)
{
	SSAstartLine = i;
}

void BasicBlock::setSSAend(int i)
{
	SSAendLine = i;
}

bool BasicBlock::operator==(BasicBlock & other)
{
	return index == other.index;
}

std::list<SSA::OperandUse> BasicBlock::getLiveIn()
{
	return liveIn;
}

void BasicBlock::addLiveIn(SSA::OperandUse op)
{
	liveIn.push_back(SSA::OperandUse(op));
}
