#ifndef __YUNI_DOCMAKE_JOB_H__
# define __YUNI_DOCMAKE_JOB_H__

# include <yuni/yuni.h>
# include <yuni/core/string.h>
# include <yuni/job/job.h>
# include <yuni/job/queue.h>
# include <vector>
# include "article.h"



class CompileJob : public Yuni::Job::IJob
{
public:
	//! \name Constructor & Destructor
	//@{
	/*!
	** \brief Constructor
	*/
	CompileJob(const Yuni::String& input, const Yuni::String& htdocs);
	//! Destructor
	virtual ~CompileJob();
	//@}

	void add(const Yuni::String& filename) {pSources.push_back(filename);}


private:
	virtual void onExecute();

	/*!
	** \brief
	**
	*/
	bool analyzeArticle();

	/*!
	** \brief Try to extract the order contained within a given path
	*/
	bool extractOrder(const Yuni::String& path);

private:
	const Yuni::String& pInput;
	const Yuni::String& pHtdocs;
	Yuni::String::Vector  pSources;

	ArticleData pArticle;
	//! Temporary string
	Yuni::String pTmp;

}; // class CompileJob


extern Yuni::Job::QueueService<>  queueService;


#endif // __YUNI_DOCMAKE_JOB_H__
