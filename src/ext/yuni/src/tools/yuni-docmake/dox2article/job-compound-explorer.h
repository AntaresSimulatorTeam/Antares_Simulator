#ifndef __YUNI_TOOL_DOCMAKE_DOXYGEN_2_ARTICLE_JOB_COMPOUND_EXPLORER_H__
# define __YUNI_TOOL_DOCMAKE_DOXYGEN_2_ARTICLE_JOB_COMPOUND_EXPLORER_H__

# include <yuni/yuni.h>
# include "job.h"
# include "compound.h"


namespace Yuni
{
namespace Edalene
{
namespace Dox2Article
{
namespace Job
{


	class CompoundExplorer : public Dox2Article::Job::IJob
	{
	public:
		/*!
		** \brief Explorer all known symbols
		*/
		static void Dispatch();

	public:
		//! \name Constructor & Destructor
		//@{
		/*!
		** \brief Default constructor
		**
		** \param compound The compound to analyze
		*/
		CompoundExplorer(const Compound::Ptr& compound);
		//! Destructor
		virtual ~CompoundExplorer();
		//@}


	protected:
		virtual void onExecute();

	private:
		//! Compound to analyze
		Compound::Ptr pCompound;

	}; // class CompoundExplorer






} // namespace Job
} // namespace Dox2Article
} // namespace Edalene
} // namespace Yuni

#endif // __YUNI_TOOL_DOCMAKE_DOXYGEN_2_ARTICLE_JOB_COMPOUND_EXPLORER_H__
