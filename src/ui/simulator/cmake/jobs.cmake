

set(SRC_UI_JOBS
		toolbox/dispatcher/study.h
		toolbox/dispatcher/study.cpp

		# Jobs
		toolbox/jobs.h
		toolbox/jobs/job.h
		toolbox/jobs/job.cpp

		# Execute
		toolbox/execute.h
		toolbox/execute/execute.h
		toolbox/execute/execute.cpp
)
add_Library(libantares-ui-jobs ${SRC_UI_JOBS})


target_link_libraries(libantares-ui-jobs  PUBLIC ${wxWidgets_LIBRARIES})
target_link_libraries(libantares-ui-jobs PRIVATE libantares-ui-common)

