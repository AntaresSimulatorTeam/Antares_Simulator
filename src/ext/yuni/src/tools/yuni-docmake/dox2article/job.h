#ifndef __YUNI_TOOL_DOCMAKE_DOXYGEN_2_ARTICLE_JOB_H__
# define __YUNI_TOOL_DOCMAKE_DOXYGEN_2_ARTICLE_JOB_H__

# include <yuni/yuni.h>
# include "job.h"
# include <yuni/job/job.h>
# include <yuni/job/queue.h>


namespace Yuni
{
namespace Edalene
{
namespace Dox2Article
{
namespace Job
{


	class IJob : public Yuni::Job::IJob
	{
	public:
		//! \name Constructor & Destructor
		//@{
		/*!
		** \brief Default constructor
		*/
		IJob() {}
		/*!
		** \brief Destructor
		*/
		virtual ~IJob() {}
		//@}

	}; // class IJob






	//! The queue service
	extern Yuni::Job::QueueService<>  queueService;






} // namespace Job
} // namespace Dox2Article
} // namespace Edalene
} // namespace Yuni

#endif // __YUNI_TOOL_DOCMAKE_DOXYGEN_2_ARTICLE_JOB_H__
