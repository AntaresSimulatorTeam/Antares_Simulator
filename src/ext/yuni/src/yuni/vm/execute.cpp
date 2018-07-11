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
#include "instructions.h"
#include <cstdlib>
#include <cstring>


namespace Yuni
{
namespace Private
{
namespace VM
{

	namespace // anonymous
	{

		class ProcessorData
		{
		public:
			ProcessorData()
				:exitCode(0)
			{
				memset(gpr, 0, sizeof(gpr));
				memset(spr, 0, sizeof(spr));
				memset(dpr, 0, sizeof(dpr));
			}

		public:
			//! General purpose registers
			uint64 gpr[16];
			//! Single-precision registers
			float spr[16];
			//! Double-precision registers
			double dpr[16];
			//! Exit code
			int exitCode;

		}; // class ProcessorData


	} // anonymous namespace



	int Program::execute()
	{
		// Implementation : Direct dispatch
		// Next instruction
		# define NEXT  goto **vp++

		// Converting each instructions into a list of goto jump
		void** jumps;
		{
			uint count = instructionCount;
			if (!count)
				return 0;
			jumps = (void**)::malloc(sizeof(void**) * (count + 1));
			void* const aliases[Instruction::max] =
			{
				&&j_exit,  // exit
				&&j_intrinsic,    // intrinsic
				&&j_add,
				&&j_addu,
				&&j_addi,
				&&j_addui,
				&&j_nop,
				&&j_exitcode,
				&&j_exitcodei
			};
			// We assume here that all instructions are valid
			// The first instruction will always be the 'exit' instruction
			jumps[0] = &&j_exit;
			for (uint i = 0; i != count; ++i)
				jumps[i + 1] = aliases[instructions[i]];
		}

		// data for our virtual processor
		ProcessorData data;
		// pointer, on the first instruction
		void** vp = ((void**) jumps) + 1;
		// The current operand
		uint op = 0;

		// execute the first instruction
		NEXT;

		// implementations of all instructions
		j_intrinsic:
			{
				uint params = (uint) operands[op++];
				// FIXME
				op += params;
				NEXT;
			}

		j_add:
			{
				uint ret = (uint) operands[op++];
				uint r1  = (uint) operands[op++];
				uint r2  = (uint) operands[op++];
				*((sint64*)(data.gpr) + ret) = (sint64)(data.gpr[r1]) + (sint64)(data.gpr[r2]);
				NEXT;
			}
		j_addu:
			{
				uint ret = (uint) operands[op++];
				uint r1  = (uint) operands[op++];
				uint r2  = (uint) operands[op++];
				data.gpr[ret] = data.gpr[r1] + data.gpr[r2];
				NEXT;
			}
		j_addi:
			{
				uint ret = (uint) operands[op++];
				uint r1  = (uint) operands[op++];
				const sint64 i = *((sint64*)(operands + op));
				op += 8;
				*((sint64*)(data.gpr) + ret) = (sint64)(data.gpr[r1]) + i;
				NEXT;
			}
		j_addui:
			{
				uint ret = (uint) operands[op++];
				uint r1  = (uint) operands[op++];
				uint64 i = *((uint64*)(operands + op));
				op += 8;
				data.gpr[ret] = data.gpr[r1] + i;
				NEXT;
			}
		j_nop:
			{
				// do nothing on purpose
				NEXT;
			}
		j_exitcode:
			{
				uint r1 = (uint) operands[op++];
				data.exitCode = (int) data.gpr[r1];
				NEXT;
			}
		j_exitcodei:
			{
				data.exitCode = (int) operands[op++];
				NEXT;
			}

		j_exit:
		(void)::free(jumps);
		return data.exitCode;
		# undef NEXT
	}





} // namespace VM
} // namespace Private
} // namespace Yuni

