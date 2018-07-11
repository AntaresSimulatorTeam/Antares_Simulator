#ifndef __YUNI_DOCMAKE_LOGS_H__
# define __YUNI_DOCMAKE_LOGS_H__

# include <yuni/yuni.h>
# include <yuni/core/logs.h>
# include <yuni/core/logs/decorators/applicationname.h>



typedef Yuni::Logs::StdCout<>  DocMakeLogsHandlers;
typedef /*Yuni::Logs::Time<*/
		Yuni::Logs::ApplicationName<
		Yuni::Logs::VerbosityLevel<Yuni::Logs::Message<>
		> > /*>*/  DocMakeLogsDecorators;


typedef Yuni::Logs::Logger<DocMakeLogsHandlers, DocMakeLogsDecorators>   DocMakeLogs;


extern DocMakeLogs  logs;


#endif // __YUNI_DOCMAKE_LOGS_H__
