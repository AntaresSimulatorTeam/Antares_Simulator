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
#pragma once
#include "../../../yuni.h"
#include "../../../core/string.h"
#include "../../../core/smartptr/intrusive.h"
#include "../../io.h"
#include "../../directory.h"
#include "detachedthread.h"



namespace Yuni
{
namespace IO
{
namespace Directory
{


	/*!
	** \brief Iterate through subfolders and files of a given directory
	**
	** This class is reentrant. Consequently, it will be thread-safe as long as the
	** user custom implementation is thread-safe.
	**
	** \code
	** #include <yuni/yuni.h>
	** #include <yuni/io/directory/iterator.h>
	** #include <iostream>
	**
	** using namespace Yuni;
	**
	**
	** class MyIterator : public IO::Directory::IIterator<true>
	** {
	** public:
	**	//! Flow
	**	typedef IO::Flow Flow;
	**
	** public:
	**	MyIterator() {}
	**	virtual ~MyIterator()
	**	{
	**		// For code robustness and to avoid corrupt vtable
	**		stop();
	**	}
	**
	** protected:
	**	virtual bool onStart(const String& rootFolder)
	**	{
	**		std::cout << " [+] " << rootFolder << std::endl;
	**		pCounter = 1;
	**		pFileCount = 0;
	**		pFolderCount = 0;
	**		pTotalSize = 0;
	**		return true;
	**	}
	**
	**	virtual Flow onBeginFolder(const String&, const String&, const String& name)
	**	{
	**		printSpaces();
	**		std::cout << " [+] " << name << std::endl;
	**		++pCounter;
	**		++pFolderCount;
	**		return IO::flowContinue;
	**	}
	**
	**	virtual void onEndFolder(const String&, const String&, const String&)
	**	{
	**		--pCounter;
	**	}
	**
	**	virtual Flow onFile(const String&, const String&, const String& name, uint64 size)
	**	{
	**		printSpaces();
	**		std::cout << "  -  " << name << " (" << size << " bytes)" << std::endl;
	**		++pFileCount;
	**		pTotalSize += size;
	**		return IO::flowContinue;
	**	}
	**
	**	virtual void onTerminate()
	**	{
	**		std::cout << "\n";
	**		std::cout << pFolderCount << " folder(s), " << pFileCount << " file(s),  "
	**			<< pTotalSize << " bytes" << std::endl;
	**	}
	**
	** private:
	**	void printSpaces()
	**	{
	**		for (uint i = 0; i != pCounter; ++i)
	**			std::cout << "    ";
	**	}
	**
	** private:
	**	uint pCounter;
	**	uint pFolderCount;
	**	uint pFileCount;
	**	uint64 pTotalSize;
	** };
	**
	**
	** int main()
	** {
	**		MyIterator iterator;
	**		iterator.add("/tmp");
	**		iterator.start();
	**		iterator.wait();
	**		return 0;
	** }
	** \endcode
	**
	** This class is thread-safe even when not in detached mode.
	** \tparam DetachedT True to perform the operation into a separate thread
	**
	** \internal When yuni is compiled without any threading support
	**    the detached mode is automatically disabled.
	*/
	template<bool DetachedT = true>
	class IIterator
		: public Yuni::IIntrusiveSmartPtr<IIterator<DetachedT>, true, Yuni::Policy::ObjectLevelLockable>
		, public Private::IO::Directory::Iterator::Interface
	{
	public:
		//! The class ancestor
		typedef typename Yuni::IIntrusiveSmartPtr<IIterator<DetachedT>, true, Yuni::Policy::ObjectLevelLockable>  Ancestor;
		//! The most suitable smart ptr for the class
		typedef typename Ancestor::template SmartPtrType<IIterator<DetachedT> >::Ptr  Ptr;

		enum
		{
			//! Detached mode
			# ifndef YUNI_NO_THREAD_SAFE
			detached = DetachedT,
			# else
			detached = false,
			# endif
			//! The default timeout for stopping a thread
			defaultTimeout = Thread::defaultTimeout,
		};

		//! Itself
		typedef IIterator<DetachedT> IteratorType;
		//! The threading policy
		typedef typename Ancestor::ThreadingPolicy ThreadingPolicy;

		//! Event: file / folder found
		typedef Bind<Flow (AnyString filename, AnyString parent, AnyString name, uint64 size)> OnNodeEvent;


	public:
		//! \name Constructor & Destructor
		//@{
		/*!
		** \brief Default Constructor
		*/
		IIterator();
		/*!
		** \brief Copy constructor
		*/
		IIterator(const IIterator& rhs);
		/*!
		** \brief Destructor
		*/
		virtual ~IIterator();
		//@}


		//! \name Search paths
		//@{
		/*!
		** \brief Add a new entry in the search paths
		*/
		void add(const AnyString& folder);

		/*!
		** \brief Clear the list of path
		*/
		void clear();
		//@}


		//! \name Execution flow
		//@{
		/*!
		** \brief Perform the traversing the root folder
		**
		** In not in detached mode, this method will block the calling thread
		** only the traversing is complete.
		** It will have no effect if a traversing is already currently in progress.
		** \return True if the thread has been started (detached mode)
		*/
		bool start();

		/*!
		** \brief Stop the traversing of the root folder
		**
		** \param timeout The timeout in milliseconds before killing the thread (detached mode only)
		** \return True if the thread has been stopped
		*/
		bool stop(uint timeout = (uint) defaultTimeout);

		/*!
		** \brief Wait for the end of the operation (infinite amount of time)
		**
		** This routine has no effect if not in detached mode.
		*/
		void wait();

		/*!
		** \brief Wait for the end of the operation
		**
		** This routine has no effect if not in detached mode.
		** \param timeout The timeout in milliseconds
		*/
		void wait(uint timeout);

		/*!
		** \brief Ask to Stop the traversing as soon as possible
		**
		** This routine has no effect if not in detached mode.
		*/
		void gracefulStop();

		/*!
		** \brief Get if the detached thread is currently running
		**
		** \return True if the thread is running. Always false if not in detached mode.
		*/
		bool started() const;
		//@}


		//! \name Operators
		//@{
		//! assignment
		IIterator& operator = (const IIterator& rhs);
		//@}


	protected:
		/*!
		** \brief Event: An iteration has started
		**
		** This method may be called from any thread in detached mode (the calling
		** thread otherwise, but always by the same thread).
		** \param root The given root path
		** \return False to cancel the operation
		*/
		virtual bool onStart(const String& root) override;

		/*!
		** \brief Event: The operation is complete
		**
		** This method may be called from any thread in detached mode (the calling
		** thread otherwise, but always by the same thread).
		** This method will not be called if the process has been canceled.
		** \see onAbort()
		*/
		virtual void onTerminate() override;

		/*!
		** \brief The process has been aborted
		**
		** This method may be called from any thread in detached mode (the calling
		** thread otherwise, but always by the same thread).
		*/
		virtual void onAbort() override;

		/*!
		** \brief Event: Starting to Traverse a new folder
		**
		** This method may be called from any thread in detached mode (the calling
		** thread otherwise, but always by the same thread).
		**
		** \param filename The full filename (ex: /path/to/my/file.txt)
		** \param parent The parent folder (ex: /path/to/my)
		** \param name The name of the folder found only (ex: file.txt)
		** \return itSkip to not go deeper in this folder
		*/
		virtual Flow onBeginFolder(const String& filename, const String& parent, const String& name) override;

		/*!
		** \brief Event: A folder has been traversed
		**
		** This method may be called from any thread in detached mode (the calling
		** thread otherwise, but always by the same thread).
		**
		** \param filename The full filename (ex: /path/to/my/file.txt)
		** \param parent The parent folder (ex: /path/to/my)
		** \param name The name of the folder found only (ex: file.txt)
		*/
		virtual void onEndFolder(const String& filename, const String& parent, const String& name) override;

		/*!
		** \brief Event: A file has been found
		**
		** This method may be called from any thread in detached mode (the calling
		** thread otherwise, but always by the same thread).
		**
		** \param filename The full filename (ex: /path/to/my/file.txt)
		** \param parent The parent folder
		** \param name The name of the file only
		** \param size Size in bytes
		** \return itAbort to abort the whole process, itSkip to skip the current folder and its sub-folders
		*/
		virtual Flow onFile(const String& filename, const String& parent, const String& name, uint64 size) override;

		/*!
		** \brief Event: It was impossible to open a folder
		**
		** This method may be called from any thread in detached mode (the calling
		** thread otherwise, but always by the same thread).
		**
		** \param filename The full filename (ex: /path/to/my/file.txt)
		** \return itAbort to abort the whole process, itContinue will be used otherwise.
		*/
		virtual Flow onError(const String& filename) override;

		/*!
		** \brief Event: It was impossible to the status of a file
		**
		** This method may be called from any thread in detached mode (the calling
		** thread otherwise, but always by the same thread).
		**
		** \param filename The full filename (ex: /path/to/my/file.txt)
		** \return itAbort to abort the whole process, itContinue will be used otherwise.
		*/
		virtual Flow onAccessError(const String& filename) override;


	private:
		# ifndef YUNI_NO_THREAD_SAFE
		typedef Yuni::Private::IO::Directory::Iterator::IDetachedThread  ThreadType;

		class DetachedThread final : public ThreadType
		{
		public:
			DetachedThread();
			virtual ~DetachedThread();
		};
		# endif

	private:
		//! The root folder
		String::VectorPtr pRootFolder;
		# ifndef YUNI_NO_THREAD_SAFE
		//! The de tached thread (only valid if detached != 0)
		ThreadType* pThread;
		# endif
		// Friend !
		friend void Yuni::Private::IO::Directory::Iterator::Traverse(
			Yuni::Private::IO::Directory::Iterator::Options&,
			Yuni::Private::IO::Directory::Iterator::IDetachedThread*);

	}; // class Iterator







	/*!
	** \brief Convenient helper on IIterator for walking through files and folders
	**
	** Example for iterating through all files and folders:
	** \code
	** auto walker = IO::Directory::CreateWalker("/tmp/some-folder",
	**	[](AnyString filename, AnyString parent, AnyString name, uint64 size) -> Flow
	**	{
	**		std::cout << " . found file " << filename << " (" << size << " bytes)\n";
	**		return IO::flowContinue;
	**	},
	**	[](AnyString pathname, AnyString parent, AnyString name, uint64) -> Flow
	**	{
	**		std::cout << " . found folder " << pathname;
	**		return IO::flowContinue;
	**	});
	**
	** walker->start();
	** walker->wait();
	** \endcode
	**
	**
	** Example for iterating through all files and folders and looking for a string:
	** \code
	** Job::QueueService queueservice;

	** auto walker = IO::Directory::CreateWalker("/tmp/some-folder",
	**	[](const String& filename, const String& parent, const String& name, uint64 size) -> Flow
	**	{
	**		if (size > 0)
	**		{
	**			async(queueservice, []()
	**			{
	**				bool match = false;
	**				IO::File::ReadLineByLine(filename, [](const AnyString& line)
	**				{
	**					if (line.icontaines("needle"))
	**					{
	**						match = true;
	**						return false; // stop
	**					}
	**					return true;
	**				});
	**
	**				if (match)
	**					std::cout << " . found in file " << filename << " (" << size << " bytes)\n";
	**			});
	**		}
	**		return IO::flowContinue;
	**	},
	**	[](const String& filename, const String& parent, const String& name, uint64) -> Flow
	**	{
	**		std::cout << " . found folder " << pathname;
	**		return IO::flowContinue;
	**	});
	**
	** walker->start();
	** walker->wait();
	** queueservice.wait();
	** \endcode
	*/
	//IIterator<true>::Ptr CreateWalker(const AnyString& folder,
	//	const IIterator<true>::OnNodeEvent& onFile, const IIterator<true>::OnNodeEvent& onFolder);





} // namespace Directory
} // namespace IO
} // namespace Yuni

# include "iterator.hxx"

