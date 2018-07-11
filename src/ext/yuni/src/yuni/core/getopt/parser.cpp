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
#include "parser.h"
#include <iostream>
#include <cassert>

// The standard error output is not displayed on Windows
# ifndef YUNI_OS_WINDOWS
#	define STD_CERR  std::cerr
# else
#	define STD_CERR  std::cout
# endif

//! The maximum length for a long name of an option
# define YUNI_GETOPT_LONGNAME_MAX_LENGTH  256



namespace Yuni
{
namespace Private
{
namespace GetOptImpl
{


	class Context final
	{
	public:
		//! typedef for IOption
		typedef Private::GetOptImpl::IOption IOption;
		//! Typedef for Parser
		typedef GetOpt::Parser Parser;
		//! Option list (order given by the user)
		typedef Parser::OptionList OptionList;
		//! All options ordered by their short name
		typedef Parser::OptionsOrderedByShortName OptionsOrderedByShortName;
		//! All options ordered by their long name
		typedef Parser::OptionsOrderedByLongName OptionsOrderedByLongName;

		/*!
		** \brief Types of tokens
		*/
		enum TokenType
		{
			//! A mere parameter
			ttParameter = 0,
			//! A short name of an option (-t or /t on Windows)
			ttShortOption,
			//! A long name of an option (--text or /text on Windows)
			ttLongOption,
		};

	public:

		/*!
		** \brief Get the type a CString (short/long option, parameter...)
		*/
		static TokenType GetTokenType(const char* argv);

	public:
		//! \name Constructor
		//@{
		/*!
		** \brief Constructor
		**
		** \param parser The public parser
		*/
		explicit Context(Parser& parser);
		//@}


		/*!
		** \brief parse the command line
		*/
		bool operator () (int argc, char* argv[]);


	private:
		//! An option has not been found
		void optionIsUnknown(const char* name);
		//! A additional parameter is missing for an option
		void parameterIsMissing(const char* name);
		//! Find the additional parameter and add it to the option
		bool findNextParameter(IOption* option, int argc, char* argv[]);


	private:
		//! The public class for the parser, where all options are stored
		Parser& pParser;
		//! Position in the list of arguments for the current token
		int pTokenIndex;
		//! Position in the list of arguments for parameters
		int pParameterIndex;
		//! The current argument
		const char* arg;
		//! A temporary CString
		const char* sub;
		//! A temporary buffer
		CString<YUNI_GETOPT_LONGNAME_MAX_LENGTH + 2, false> buffer;

	}; // class Context






	inline Context::TokenType Context::GetTokenType(const char* argv)
	{
		# ifndef YUNI_OS_MSVC
		return (*argv == '-')
			? ((argv[1] == '-') ? ttLongOption : ttShortOption)
			: ttParameter;
		# else
		return (*argv == '-' or *argv == '/')
			? ((argv[1] == '-') ? ttLongOption : ttShortOption)
			: ttParameter;
		# endif
	}


	inline Context::Context(Parser& parser) :
		pParser(parser),
		pTokenIndex(1),
		pParameterIndex(1)
	{
		pParser.pErrors = 0;
	}


	bool Context::findNextParameter(IOption* option, int argc, char* argv[])
	{
		assert(option != NULL);

		if (not option->requireAdditionalParameter())
		{
			option->enableFlag();
			return true;
		}

		while ((++pParameterIndex) < argc)
		{
			// We only want parameters
			if (ttParameter == GetTokenType(argv[pParameterIndex]))
			{
				// Adding the argument to the list of value of the option
				option->addValue(argv[pParameterIndex], static_cast<String::size_type>(::strlen(argv[pParameterIndex])));
				// This argument must not be used again as a parameter
				++pParameterIndex;
				return true;
			}
		}

		// If not found, it is an error
		++pParser.pErrors;
		return false;
	}


	void Context::optionIsUnknown(const char* name)
	{
		if (not pParser.pIgnoreUnknownArgs)
		{
			++pParser.pErrors;
			STD_CERR << "Error: The option `" << name << "` is unknown" << std::endl;
		}
	}


	void Context::parameterIsMissing(const char* name)
	{
		++pParser.pErrors;
		STD_CERR << "Error: The parameter for `" << name << "` is missing" << std::endl;
	}


	bool Context::operator () (int argc, char* argv[])
	{
		while (pTokenIndex < argc)
		{
			arg = argv[pTokenIndex];

			switch (GetTokenType(arg))
			{
				// The current argument seems to be a short name of an option
				case ttShortOption:
					{
						while ('\0' != *(++arg))
						{
							OptionsOrderedByShortName::iterator i = pParser.pShortNames.find(*arg);
							if (i != pParser.pShortNames.end())
							{
								if (not findNextParameter(i->second, argc, argv))
									std::cerr << "Error: The parameter is missing for `" << arg << "`" << std::endl;
							}
							else
							{
								if (*arg == 'h' or *arg == '?')
								{
									pParser.helpUsage(argv[0]);
									return false;
								}
								optionIsUnknown(arg);
							}
						}
						break;
					}

				// The current argument seems to be a long name of an option
				case ttLongOption:
					{
						++arg;
						++arg;
						if ('\0' == *arg) // End of options
							return (0 == pParser.pErrors);

						if ((sub = strchr(arg, '=')))
						{
							const uint size = static_cast<uint>(sub - arg);

							if (size < YUNI_GETOPT_LONGNAME_MAX_LENGTH)
							{
								buffer.assign(arg, size);
								arg += size;
								++arg;

								OptionsOrderedByLongName::iterator i = pParser.pLongNames.find(buffer.c_str());
								if (i != pParser.pLongNames.end())
								{
									i->second->addValue(arg, static_cast<String::size_type>(::strlen(arg)));
								}
								else
								{
									if (0 == ::strcmp(buffer.c_str(), "help"))
									{
										pParser.helpUsage(argv[0]);
										return false;
									}
									optionIsUnknown(buffer.c_str());
								}
							}
							else
							{
								STD_CERR << "Error: name too long" << std::endl;
								++pParser.pErrors;
							}
						}
						else
						{
							OptionsOrderedByLongName::iterator i = pParser.pLongNames.find(arg);
							if (i != pParser.pLongNames.end())
							{
								if (not findNextParameter(i->second, argc, argv))
									parameterIsMissing(arg);
							}
							else
							{
								if (0 == ::strcmp(arg, "help"))
								{
									pParser.helpUsage(argv[0]);
									return false;
								}
								optionIsUnknown(arg);
							}
						}
						break;
					}

					// The current argument is a parameter actually (not attached to any option)
				case ttParameter:
					{
						if (pTokenIndex >= pParameterIndex)
						{
							pParameterIndex = pTokenIndex;
							if (pParser.pRemains)
								pParser.pRemains->addValue(arg, static_cast<String::size_type>(::strlen(arg)));
						}
						break;
					}
			}
			++pTokenIndex;
		}

		return (0 == pParser.pErrors);
	}




} // namespace GetOptImpl
} // namespace Private
} // namespace Yuni









namespace Yuni
{
namespace GetOpt
{

	static const char* ExtractFilenameOnly(const char* argv)
	{
		const char* result = argv;
		while ('\0' != *argv)
		{
			if ('\\' == *argv or '/' == *argv)
			{
				result = argv;
				++result;
			}
			++argv;
		}
		return result;
	}




	Parser::Parser() :
		pRemains(nullptr),
		pErrors(0),
		pIgnoreUnknownArgs(false)
	{}



	Parser::~Parser()
	{
		if (not pAllOptions.empty())
		{
			OptionList::iterator end = pAllOptions.end();
			for (OptionList::iterator i = pAllOptions.begin(); i != end; ++i)
				delete *i;
		}

		delete pRemains;
	}


	void Parser::clear()
	{
		if (not pAllOptions.empty())
		{
			OptionList::iterator end = pAllOptions.end();
			for (OptionList::iterator i = pAllOptions.begin(); i != end; ++i)
				delete *i;

			// clear-and-minimize idiom
			OptionsOrderedByShortName emptyShort;
			OptionsOrderedByLongName  emptyLong;
			OptionList emptyOptions;

			emptyShort.swap(pShortNames);
			emptyLong.swap(pLongNames);
			emptyOptions.swap(pAllOptions);

			// the whole content of these 3 containers will be destroyed here
		}

		delete pRemains;
		pRemains = nullptr;
	}


	bool Parser::operator () (int argc, char* argv[])
	{
		Private::GetOptImpl::Context context(*this);
		return context(argc, argv);
	}


	void Parser::helpUsage(const char* argv0)
	{
		assert(argv0 != NULL); // just in case

		std::cout.write("Usage: ", 7);
		std::cout << ExtractFilenameOnly(argv0);
		std::cout.write(" [OPTION]...", 12);

		if (pRemains)
			std::cout.write(" [FILE]...\n", 11);
		else
			std::cout << '\n';

		if (not pAllOptions.empty())
		{
			OptionList::const_iterator end = pAllOptions.end();
			OptionList::const_iterator i = pAllOptions.begin();

			// Add a space if the first option is not a paragraph
			// In this case the user would do what he wants
			if (not dynamic_cast<const Private::GetOptImpl::Paragraph*>(*i))
				std::cout << '\n';

			for (; i != end; ++i)
				(*i)->helpUsage(std::cout);
		}

		// Help
		if (pLongNames.end() == pLongNames.find("help"))
		{
			if (pShortNames.end() == pShortNames.find('h'))
				Private::GetOptImpl::DisplayHelpForOption(std::cout, 'h', "help", "Display this help and exit");
			else
				Private::GetOptImpl::DisplayHelpForOption(std::cout, ' ', "help", "Display this help and exit");
		}

		std::cout << std::endl; // flush
	}



	void Parser::appendShortOption(IOption* option, char shortname)
	{
		// In the list with all other options
		pAllOptions.push_back(option);
		// The short name
		if (shortname != ' ' and shortname != '\0')
			pShortNames[shortname] = option;
	}


	void Parser::appendOption(IOption* option, char shortname)
	{
		const String& longname = option->longName();
		if (not longname.empty())
		{
			// The long name of an option must not be equal to 1
			// There is an ambiguity on Windows : /s : a long or short name ?
			# ifndef NDEBUG
			assert(longname.size() != 1 and "The long name of an option must be igreater than 1 (ambigous on Windows)");
			# else
			if (longname.size() == 1) // ambigous on Windows, must not continue
				return;
			# endif

			pLongNames[longname.c_str()] = option;
		}

		// In the list with all other options
		pAllOptions.push_back(option);

		// The short name
		if (shortname != ' ' and shortname != '\0')
			pShortNames[shortname] = option;
	}




} // namespace GetOpt
} // namespace Yuni
