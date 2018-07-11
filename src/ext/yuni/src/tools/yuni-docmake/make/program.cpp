
#include <yuni/yuni.h>
#include "program.h"
#include <yuni/core/system/cpu.h>
#include <yuni/core/math.h>
#include <yuni/core/getopt.h>
#include "../logs.h"


#define SEP  IO::Separator


namespace Yuni
{
namespace Tool
{
namespace DocMake
{


	unsigned int Program::nbJobs = 1;
	bool Program::debug    = false;
	bool Program::verbose  = false;
	bool Program::clean    = false;
	bool Program::shortUrl = false;
	bool Program::quiet    = false;

	String Program::input;
	String Program::htdocs;
	String Program::indexFilename = "index.html";
	String Program::webroot = "/";
	String Program::target;
	String Program::profile;
	String Program::indexCacheFilename;




	Program::Program() :
		printVersion(false)
	{
	}


	Program::~Program()
	{
		if (pProfileFile.opened())
		{
			pProfileFile.unlock();
			pProfileFile.close();
		}
	}


	bool Program::parseCommandLine(int argc, char** argv)
	{
		// The total number of CPU
		const unsigned int cpuCount = System::CPU::Count();
		// The most appropriate number of simultaneous jobs
		nbJobs = (cpuCount > 4) ? 4 : cpuCount;

		// The Command line parser
		GetOpt::Parser opts;

		// Compiler
		opts.addParagraph("\nOptions:");
		opts.remainingArguments(profile);
		opts.add(profile       ,'p', "profile", "The profile");
		opts.add(target        ,'t', "target", "target");
		opts.addFlag(clean     ,'c', "clean", "Clean the index db");
		opts.addFlag(shortUrl  ,'s', "short-url", "Use short url");
		opts.add(nbJobs        ,'j', "jobs", String() << "Number of concurrent jobs (default: " << nbJobs
			<< ", max: " << cpuCount  << ')');

		// Help
		opts.addParagraph("\nHelp");
		opts.addFlag(quiet, 'q', "quiet", "Do not print any message, only warnings and errors");
		opts.addFlag(verbose, ' ', "verbose", "Print any error message");
		opts.addFlag(debug, ' ', "debug", "Print debug messages");
		opts.addFlag(printVersion, 'v', "version", "Print the version and exit");

		if (!opts(argc, argv))
		{
			int status = opts.errors() ? EXIT_FAILURE /*error*/ : 0;
			if (verbose || debug)
				logs.error() << "Error when parsing the command line";
			exit(status);
			return false;
		}

		if (!profile || !IO::File::Exists(profile))
		{
			if (!profile)
				logs.error() << "Please provide a profile";
			else
				logs.error() << "Profile not found : " << profile;
			return false;
		}

		// Read the profile
		// The profile will be locked for prevent any concurrent use
		if (!readProfile(profile))
			return false;

		if (!input)
		{
			logs.error() << "Please specify an input folder (--input, see --help for more informations)";
			return false;
		}
		if (!htdocs)
		{
			logs.error() << "Please specify a htdocs folder (--htdocs, see --help for more informations)";
			return false;
		}

		// Normalize the input / htdocs
		{
			String tmp;
			IO::MakeAbsolute(tmp, input);
			IO::Normalize(input, tmp);
			IO::MakeAbsolute(tmp, htdocs);
			IO::Normalize(htdocs, tmp);
			if (!indexCacheFilename)
				indexCacheFilename << htdocs << SEP << ".edalene-cache.edlndb";
			IO::MakeAbsolute(tmp, indexCacheFilename);
			IO::Normalize(indexCacheFilename, tmp);

			if (!IO::Directory::Exists(input))
			{
				logs.error() << "IO Error: Directory does not exist: " << input;
				return false;
			}
			if (!IO::Directory::Exists(htdocs))
			{
				logs.error() << "IO Error: Directory does not exist: " << htdocs;
				return false;
			}
		}

		nbJobs = Math::MinMax<unsigned int>(nbJobs, 1, cpuCount);
		return true;
	}


	void Program::printInformations() const
	{
		logs.info() << "profile : " << profile;
		if (!target)
			logs.info() << "target  : no-target";
		else
			logs.info() << "target  : [" << target << ']';

		logs.info() << "   source : " << input;
		logs.info() << "   htdocs : " << htdocs;
		logs.info() << "   cache  : " << indexCacheFilename;
		logs.info() << "   directory-index : " << indexFilename;
		logs.info() << "   short-url : " << (shortUrl ? "true" : "false");
		logs.info();
	}


	bool Program::readProfile(const String& filename)
	{
		if (pProfileFile.opened())
		{
			pProfileFile.unlock();
			pProfileFile.close();
		}
		if (!pProfileFile.open(filename))
		{
			logs.error() << "impossible to read the profile : " << filename;
			return false;
		}

		pProfileFile.lockExclusive();

		target.toLower();
		Clob buffer;
		String key;
		String value;
		String currentProfile;
		bool skip = true;
		while (pProfileFile.readline(buffer))
		{
			if (!buffer)
				continue;
			buffer.extractKeyValue(key, value, true);
			if (!key)
				continue;

			if (key == "[")
			{
				if (!currentProfile && value != "global")
				{
					logs.error() << "invalid profile : the first section must be 'global' for global options";
					return false;
				}
				currentProfile = value;
				if (value == "global")
					skip = false;
				else
					skip = (value != target);
				continue;
			}

			if (skip)
				continue;

			if (key == "source")
			{
				input = value;
				continue;
			}
			if (key == "htdocs")
			{
				htdocs = value;
				continue;
			}
			if (key == "default")
			{
				if (!target)
					target = value;
				continue;
			}
			if (key == "short-url")
			{
				shortUrl = value.to<bool>();
				continue;
			}
			if (key == "directory-index")
			{
				indexFilename = value;
				continue;
			}
			if (key == "cache")
			{
				indexCacheFilename = value;
				continue;
			}

			logs.warning() << "profile " << profile << ": unknown key '" << key << "'";
		}

		return true;
	}






} // namespace DocMake
} // namespace Tool
} // namespace Yuni

