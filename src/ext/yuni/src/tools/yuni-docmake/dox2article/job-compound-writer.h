#ifndef __YUNI_TOOL_DOCMAKE_DOXYGEN_2_ARTICLE_JOB_COMPOUND_WRITER_H__
# define __YUNI_TOOL_DOCMAKE_DOXYGEN_2_ARTICLE_JOB_COMPOUND_WRITER_H__

# include <yuni/yuni.h>
# include "job.h"
# include "compound.h"
# include <map>


namespace Yuni
{
namespace Edalene
{
namespace Dox2Article
{
namespace Job
{



	class CompoundWriter : public Yuni::Edalene::Dox2Article::Job::IJob
	{
	public:
		//! Ordered section
		typedef std::map<String, Section::Vector> OrderedSection;

	public:
		/*!
		** \brief Explorer all known symbols
		*/
		static void Dispatch();

	public:
		//! \name Constructor & Destructor
		//@{
		/*!
		** \brief Constructor
		*/
		explicit CompoundWriter(Compound::Ptr& compound);
		//! Destructor
		virtual ~CompoundWriter();
		//@}

	protected:
		virtual void onExecute();

	private:
		void buildClass();
		void buildNamespace();

		//! Create the ordered list (Yuni Coding's style) of all sections
		void buildSectionMapping(OrderedSection& map, bool isAbstract);

		void appendClassIndex(Clob& output, bool isPublic, CompoundType compoundType, const Clob& data);
		void appendClassSection(const Section& section, bool isAbstract);
		void prepareClassSubtitle(const Section& section);
		void appendClassFunction(const Member& member, bool isPublic);
		void appendClassTypedef(const Member& member, bool isPublic);
		void appendClassVariable();

	private:
		Compound::Ptr pCompound;

		// Temporary buffer for stream output
		Clob out;
		//! Temporary stream output (protected / public | compound type)
		Clob outC[2][kdMax];
		Clob fileOut;

		// Some temporary variables for class building
		String lastVisibility;
		String sectionName;
		String name;
		String type;
		String paramType;
		String paramName;
		String id;
		String toggle;
		String subtitle;
		String visibility;
		char umlSymbol;

	}; // class CompoundWriter








} // namespace Job
} // namespace Dox2Article
} // namespace Edalene
} // namespace Yuni

#endif // __YUNI_TOOL_DOCMAKE_DOXYGEN_2_ARTICLE_JOB_COMPOUND_WRITER_H__
