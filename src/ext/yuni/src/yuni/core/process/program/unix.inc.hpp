#pragma once
// file included by program.cpp

#ifndef YUNI_OS_WINDOWS
#include <sys/poll.h>
#include <sys/time.h>
#ifdef YUNI_OS_LINUX
# include <sys/prctl.h>
#endif
#include <iostream>




namespace Yuni
{
namespace Process
{

	namespace // anonymous
	{

		static inline char* duplicateString(const AnyString& string)
		{
			const uint bytes = static_cast<uint>(sizeof(char)) * string.size();
			char* copy = reinterpret_cast<char*>(::malloc(bytes + 1));
			if (copy)
			{
				YUNI_MEMCPY(copy, bytes, string.c_str(), bytes);
				copy[string.size()] = '\0';
			}
			return copy;
		}


		static inline bool closeFD(int& fd)
		{
			if (fd >= 0)
			{
				int r = ::close(fd);
				fd = -1;
				return (0 == r);
			}
			return true;
		}


	} // anonymous namespace






	inline void Program::ThreadMonitor::cleanupAfterChildTermination()
	{
		// stop the thread dedicated to handle the timeout
		if (procinfo.timeoutThread)
		{
			procinfo.timeoutThread->stop();
			delete procinfo.timeoutThread;
			procinfo.timeoutThread = nullptr;
		}

		// close all remaining fd
		closeFD(channels.infd[0]);
		closeFD(channels.errd[0]);
		closeFD(channels.outfd[1]);
	}


	bool Program::ThreadMonitor::spawnProcess()
	{
		//! argv0 for the subprocess
		const char* const argv0 = procinfo.executable.c_str();
		char** args;
		{
			uint count = static_cast<uint>(procinfo.arguments.size());
			if (count)
			{
				// if there are additional arguments, argv0 must be duplicated
				args = reinterpret_cast<char**>(::malloc(sizeof(char*) * (count + 2)));
				if (YUNI_LIKELY(args))
				{
					args[0] = duplicateString(procinfo.executable);
					for (uint i = 0; i != count; ++i)
						args[i + 1] = duplicateString(procinfo.arguments[i]);
					args[count + 1] = nullptr;
				}
				else
					return false;
			}
			else
				args = nullptr;
		}


		// The parent is going to write into
		// pipe(channels.outfd)
		// The parent is going to read from
		// pipe(channels.infd)
		// The parent is going to read from (cerr)
		// pipe(channels.errd)
		if (/*input*/pipe(channels.outfd) or /*output*/pipe(channels.infd) or pipe(channels.errd))
		{
			switch (errno)
			{
				case EFAULT:
					std::cerr << "pipe failed: invalid file descriptor\n";
					break;
				case EINVAL:
					std::cerr << "pipe failed: invalid flags\n";
					break;
				case EMFILE:
					std::cerr << "pipe failed: Too many file descriptors are in use by the process\n";
					break;
				case ENFILE:
					std::cerr << "pipe failed: The system limit on the total number of open files has been reached\n";
					break;
			}
			return false;
		}


		// Getting the start time of execution
		pStartTime = currentTime();

		// ** FORK **
		if (-1 == (pid = fork()))
		{
			std::cerr << "fork failed\n";
			return false;
		}

		if (0 == pid) // CHILD
		{
			// Code executed from the child process
			closeFD(channels.outfd[1]);
			closeFD(channels.infd[0]);
			closeFD(channels.errd[0]);
			::close(0);
			::close(1);
			::close(2);

			::dup2(channels.outfd[0], 0); // stdin  is pipeout
			::dup2(channels.infd[1],  1); // stdout is pipeout
			::dup2(channels.errd[1],  2); // stderr is pipeout

			closeFD(channels.outfd[0]);
			closeFD(channels.infd[1]);
			closeFD(channels.errd[1]);

			// restoring signal handlers
			::signal(SIGTERM, SIG_DFL);
			::signal(SIGINT,  SIG_DFL);
			::signal(SIGPIPE, SIG_DFL);
			::signal(SIGCHLD, SIG_DFL);


			#ifdef YUNI_OS_LINUX
			// Kill this process if the parent dies
			::prctl(PR_SET_PDEATHSIG, SIGHUP);
			#endif

			if (not procinfo.workingDirectory.empty())
			{
				// ignoring return value
				if (not Yuni::IO::Directory::Current::Set(procinfo.workingDirectory))
					std::cerr << "invalid working directory: " << procinfo.workingDirectory << std::endl;
			}

			int status = ::execvp(argv0, args); // shall never returns
			_exit(status);
		}
		else // PARENT
		{
			// Code executed from the parent process
			closeFD(channels.outfd[0]);
			closeFD(channels.infd[1]);
			closeFD(channels.errd[1]);

			// make the standard output non-blocking
			//::fcntl(channels.infd[0], F_SETFL, O_NONBLOCK);
			//::fcntl(channels.errd[0], F_SETFL, O_NONBLOCK);

			// cleanup
			if (args)
			{
				for (char** string = args; *string; ++string)
					::free(*string);
				::free(args);
			}

			//write(outfd[1], "2^32\n",5); // Write to child’s stdin

			// The mutex has already been locked and should be unlocked to let the main
			procinfo.processInput = channels.outfd[1];
			procinfo.processID = pid;

			// timeout for the sub process
			procinfo.createThreadForTimeoutWL();
		}

		return true;
	}


	void Program::ThreadMonitor::waitForSubProcess()
	{
		// alias flag to determine whether there is a custom stream handler or not
		const bool hasStream = !(!stream);
		// is a buffer required ?
		const bool captureOutput = (hasStream or pRedirectToConsole);

		// a 4K buffer seems the most efficient size
		enum { bufferSize = 4096 };
		// buffer for reading std::cout and std::cerr
		char* const buffer = (captureOutput) ? (char*)::malloc(sizeof(char) * bufferSize) : nullptr;
		if (YUNI_UNLIKELY(!buffer and captureOutput)) // allocation failed
		{
			pExitStatus = -127; // notify - fatal error
			pEndTime = currentTime();
			return;
		}

		// information for 'poll'
		struct pollfd pfds[2];


		// loop for reading on the inputs, then for checking the process id
		do
		{
			// try to read something from the inputs until both are invalid
			// to avoid unwanted behaviors from system buffers, especially on linux.
			// (Otherwise the full output may not be retrieved - especially the final part)
			while (captureOutput)
			{
				// reset poll structure
				pfds[0].fd = channels.infd[0]; // std::cout
				pfds[0].events = POLLIN | POLLNVAL | POLLHUP;
				pfds[1].fd = channels.errd[0]; // std::cerr
				pfds[1].events = POLLIN | POLLNVAL | POLLHUP;

				// waiting for some changes on the file descriptors
				int rp = ::poll(pfds, (nfds_t) 2, /*no timeout*/ -1);
				if (rp < 0)
					break;

				// flag to remember if something has been read from std::cerr
				//
				// \internal Checking first std::cerr then std::cout which will try to continue
				// whatever the previous state was since the standard input is more likely to
				// provide some content.
				bool hasInputOnStdcerr = false;

				if (0 != pfds[1].revents) // stdcerr
				{
					// std::cerr - an empty buffer is also an error
					ssize_t stdcerrsize = ::read(channels.errd[0], buffer, bufferSize - 1);
					if (stdcerrsize > 0)
					{
						if (pRedirectToConsole)
							std::cerr.write(buffer, (std::streamsize) stdcerrsize);
						if (hasStream)
						{
							// just in case - if the calling code uses ::strlen on the buffer
							buffer[stdcerrsize] = '\0';
							stream->onErrorRead(AnyString(buffer, static_cast<uint>(stdcerrsize)));
						}

						hasInputOnStdcerr = true;
					}
				}

				if (0 != pfds[0].revents) // stdcout
				{
					// std::cout - an empty buffer is also an error
					ssize_t stdcoutsize = ::read(channels.infd[0], buffer, bufferSize - 1);
					if (stdcoutsize > 0)
					{
						if (pRedirectToConsole)
						{
							std::cout.write(buffer, static_cast<std::streamsize>(stdcoutsize));
							std::cout << std::flush;
						}
						if (hasStream)
						{
							// just in case - if the calling code uses ::strlen on the buffer
							buffer[stdcoutsize] = '\0';
							stream->onRead(AnyString(buffer, static_cast<uint>(stdcoutsize)));
						}

						// something has been read from std::cout - trying again anyway
						continue;
					}
				}

				// nothing on std::cout and actually nothing on std::cerr: aborting
				if (not hasInputOnStdcerr)
					break;
			}


			// failed to read something from std::cout and std::cerr
			// the process is likely already dead
			{
				int status;
				int wpid = ::waitpid(pid, &status, /*WNOHANG |*/ WUNTRACED | WCONTINUED);
				if (wpid > 0)
				{
					// the child process has stopped
					// getting the execution time ASAP to have the most precise value even in debug mode
					// (especially when the precision is set to 'milliseconds')
					pEndTime = currentTime();

					if (WIFEXITED(status))
						pExitStatus = WEXITSTATUS(status);
					else if (WIFSIGNALED(status))
					{
						pExitStatus = -127;
						pKilled = true;
					}
					break;
				}
				else
				{
					if (wpid == -1) // the process is already dead
					{
						// getting the execution time ASAP to have the most precise value even in debug mode
						// (especially when the precision is set to 'milliseconds')
						pEndTime = currentTime();
						break;
					}
				}
				// break;
			}
		}
		while (true);

		// release the buffer
		::free(buffer);

		// end of the chrono
		assert(pEndTime != 0 and "pEndTime is not properly initialized");
	}


	void Program::ThreadMonitor::onKill()
	{
		// the thread has been killed (this should really never happen)
		// killing the sub process if any
		bool killed = false;

		// try to kill the attached child process if any
		procinfo.mutex.lock();
		if (procinfo.processID > 0)
		{
			// killing the sub-process, until it is really dead
			if (procinfo.sendSignal<false>(SIGKILL))
			{
				int status;
				::waitpid(procinfo.processID, &status, /*WNOHANG |*/ WUNTRACED | WCONTINUED);
			}
			// getting the current time as soon as possible
			pEndTime = currentTime();
			// the process existed and has been killed
			killed = true;
			// reset the pid
			procinfo.processID = 0;

			cleanupAfterChildTermination();
		}
		procinfo.mutex.unlock();

		theProcessHasStopped(killed, -127);
	}





} // namespace Process
} // namespace Yuni

#endif // YUNI_OS_WINDOWS
