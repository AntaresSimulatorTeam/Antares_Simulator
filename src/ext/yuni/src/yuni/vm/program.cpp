/*
** This file is part of libyuni, a cross-platform C++ framework (http://libyuni.org).
**
** This Source Code Form is subject to the terms of the Mozilla Public License
** v.2.0. If a copy of the MPL was not distributed with this file, You can
** obtain one at http://mozilla.org/MPL/2.0/.
**
** github: https://github.com/libyuni/libyuni/
** gitlab: https://gitlab.com/libyuni/libyuni/ (mirror)
*/
#include "program.h"
#include <stdlib.h>
#include "instructions.h"


namespace Yuni
{
namespace Private
{
namespace VM
{

	enum
	{
		chunkSize = 1024,
	};



	Program::Program()
		:instructions(nullptr),
		instructionCount(0),
		instructionCapacity(0),
		operands(nullptr),
		operandCount(0),
		operandCapacity(0)
	{}


	Program::~Program()
	{
		(void)::free(instructions);
		(void)::free(operands);
	}


	void Program::clear()
	{
		(void)::free(instructions);
		(void)::free(operands);
		instructionCount = 0;
		operandCount = 0;
	}


	void Program::increaseInstructionCapacity()
	{
		instructionCapacity += chunkSize;
		instructions = reinterpret_cast<InstructionType*>(::realloc(instructions, static_cast<size_t>(instructionCapacity)));
	}


	void Program::increaseInstructionCapacity(uint chunkSize)
	{
		instructionCapacity += chunkSize;
		instructions = reinterpret_cast<InstructionType*>(::realloc(instructions, static_cast<size_t>(instructionCapacity)));
	}


	void Program::increaseOperandCapacity()
	{
		operandCapacity += chunkSize;
		operands = reinterpret_cast<char*>(::realloc(operands, static_cast<size_t>(operandCapacity)));
	}


	void Program::increaseOperandCapacity(uint chunkSize)
	{
		operandCapacity += chunkSize;
		operands = reinterpret_cast<char*>(::realloc(operands, static_cast<size_t>(operandCapacity)));
	}


	void Program::reserveInstructions(uint count)
	{
		if (count > instructionCapacity)
		{
			do
			{
				instructionCapacity += chunkSize;
			}
			while (count > instructionCapacity);
			instructions = reinterpret_cast<InstructionType*>(::realloc(instructions, static_cast<size_t>(instructionCapacity)));
		}
	}


	void Program::reserveOperands(uint count)
	{
		if (count > operandCapacity)
		{
			do
			{
				operandCapacity += chunkSize;
			}
			while (count > operandCapacity);
			operands = reinterpret_cast<char*>(::realloc(operands, static_cast<size_t>(operandCapacity)));
		}
	}



	bool Program::validate() const
	{
		enum OperandType
		{
			nop = 0,
			gpr, // general purpose register
			i8,  // immediate value
			i16, // immediate value (16bits)
			i32, // immediate value (32bits)
			i64, // immediate value (64bits)
			spr, // single-precision register
			dpr, // double precision register
			vr,  // variable
		};
		static const uint operandSize[] =
		{
			0,  // nop
			1,  // gpr
			1,  // i8
			2,  // i16
			4,  // i32
			8,  // i64
			1,  // spr
			1,  // dpr
			1,  // vr
		};
		static const OperandType operandCard[Instruction::max][6] =
		{
			/* exit */        { nop },
			/* intrinsic */   { vr, nop },
			/* add */         { gpr, gpr, gpr, nop },
			/* addu */        { gpr, gpr, gpr, nop },
			/* addi */        { gpr, gpr, i64, nop },
			/* addui */       { gpr, gpr, i64, nop },
			/* nop */         { nop },
			/* exitCode */    { gpr, nop },
			/* exitCodei */   { i8,  nop },
		};

		uint operandsIndex = 0;
		const uint count = instructionCount;
		// foreach instruction...
		for (uint vp = 0; vp != count; ++vp)
		{
			// The current instruction
			const InstructionType instr = instructions[vp];
			// invalid instruction
			if (instr >= Instruction::max)
				return false;

			const OperandType* operands = operandCard[instr];
			for (uint oindx = 0; operands[oindx] != nop; ++oindx)
			{
				switch (operands[oindx])
				{
					case gpr:
						{
							if (operandsIndex + operandSize[gpr] >= operandCount)
								return false;
							if (operands[operandsIndex] > 15)
								return false;
							operandsIndex += operandSize[gpr];
							break;
						}
					case nop:
						break;
					case spr:
						{
							if (operandsIndex + operandSize[spr] >= operandCount)
								return false;
							if (operands[operandsIndex] > 15)
								return false;
							operandsIndex += operandSize[spr];
							break;
						}
					case dpr:
						{
							if (operandsIndex + operandSize[dpr] >= operandCount)
								return false;
							if (operands[operandsIndex] > 15)
								return false;
							operandsIndex += operandSize[dpr];
							break;
						}
					case i64:
						{
							if (operandsIndex + operandSize[i64] >= operandCount)
								return false;
							operandsIndex += operandSize[i64];
							break;
						}
					case i32:
						{
							if (operandsIndex + operandSize[i32] >= operandCount)
								return false;
							operandsIndex += operandSize[i32];
							break;
						}
					case i8:
						{
							if (operandsIndex + operandSize[i8] >= operandCount)
								return false;
							operandsIndex += operandSize[i8];
							break;
						}
					case i16:
						{
							if (operandsIndex + operandSize[i16] >= operandCount)
								return false;
							operandsIndex += operandSize[i16];
							break;
						}
					case vr:
						{
							uint params = operands[operandsIndex++];
							if (params)
							{
								if (params > 4)
									return false;
								if (operandsIndex + params >= operandCount)
									return false;

								for (uint i = 0; i != params; ++i)
								{
									if (operands[operandsIndex + i] > 15) // register
										return false;
								}
								operandsIndex += params;
							}
							break;
						}
				}
			}
		}
		return true;
	}





} // namespace VM
} // namespace Private
} // namespace Yuni

