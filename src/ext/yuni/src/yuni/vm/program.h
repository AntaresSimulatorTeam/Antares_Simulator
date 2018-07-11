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
#pragma once
#include "../yuni.h"
#include "std.h"
#include "fwd.h"



namespace Yuni
{
namespace Private
{
namespace VM
{

	typedef Yuni::VM::InstructionType  InstructionType;



	class Program
	{
	public:

	public:
		Program();
		~Program();

		void clear();

		void add(InstructionType instruction);

		template<class T1>
		void add(InstructionType instruction, T1 r1);

		template<class T1, class T2>
		void add(InstructionType instruction, T1 r1, T2 r2);

		template<class T1, class T2, class T3>
		void add(InstructionType instruction, T1 r1, T2 r2, T3 r3);

		template<class T1, class T2, class T3, class T4>
		void add(InstructionType instruction, T1 r1, T2 r2, T3 r3, T4 r4);

		template<class T1, class T2, class T3, class T4, class T5>
		void add(InstructionType instruction, T1 r1, T2 r2, T3 r3, T4 r4, T5 r5);

		void reserveInstructions(uint count);
		void reserveOperands(uint count);

		/*!
		** \brief Validate the assembly code
		*/
		bool validate() const;

		/*!
		** \brief Execute the program
		**
		** The program should be validated first
		*/
		int execute();

	protected:
		void increaseInstructionCapacity();
		void increaseInstructionCapacity(uint chunkSize);
		void increaseOperandCapacity();
		void increaseOperandCapacity(uint chunkSize);

	public:
		//! Continuous list of instructions
		InstructionType* instructions;
		//! The number of instructions
		uint instructionCount;
		//! The capacity
		uint instructionCapacity;
		//! Continuous list of operands for each instructions
		char* operands;
		//! The number of operands
		uint operandCount;
		//!
		uint operandCapacity;

	}; // class Program





} // namespace VM
} // namespace Private
} // namespace Yuni

#include "program.hxx"
