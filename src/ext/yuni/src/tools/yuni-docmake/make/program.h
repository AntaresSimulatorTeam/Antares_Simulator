#ifndef __YUNI_DOCMAKE_MAKE_H__
# define __YUNI_DOCMAKE_MAKE_H__

# include <yuni/yuni.h>
# include <yuni/core/string.h>
# include <yuni/io/file.h>


namespace Yuni
{
namespace Tool
{
namespace DocMake
{


	class Program
	{
	public:
		//! \name Constructor & Destructor
		//@{
		/*!
		** \brief Default constructor
		*/
		Program();
		//! Destructor
		~Program();
		//@}

		bool parseCommandLine(int argc, char** argv);

		void findAllSourceFiles();

		bool readProfile(const String& filename);

		void printInformations() const;

	public:
		//! The input folder
		static String input;
		//! The htdocs folder
		static String htdocs;
		//! The index filename
		static String indexFilename;
		//! The web root (ex: http://www.libyuni.org)
		static String webroot;
		//! The profile
		static String profile;
		//! The target
		static String target;
		static String indexCacheFilename;

		static unsigned int nbJobs;
		bool printVersion;
		static bool debug;
		static bool verbose;
		static bool clean;
		static bool shortUrl;
		static bool quiet;

	private:
		//! Profile file
		Yuni::IO::File::Stream pProfileFile;

	}; // class Make





} // namespace DocMake
} // namespace Tool
} // namespace Yuni

#endif // __YUNI_DOCMAKE_MAKE_H__
