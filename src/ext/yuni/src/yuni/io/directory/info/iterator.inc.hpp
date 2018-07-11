#ifndef __YUNI_IO_DIRECTORY_INFO_ITERATOR_H__
# define __YUNI_IO_DIRECTORY_INFO_ITERATOR_H__


// !!! Do not use includes here

public:

	struct Model
	{

		// Forward declarations
		class NullIterator;
		template<uint FlagsT> class Iterator;


		class NullIterator
		{
		public:
			//! The type of the orignal object
			typedef String value_type;
			//! An uintegral type
			typedef uint64  uint64ype;
			//! A signed integral type
			typedef sint64  difference_type;

			enum
			{
				//! A non-zero value if the iterator can go forward
				canGoForward  = true,
				//! A non-zero value if the iterator can go backward
				canGoBackward = false,
			};


		protected:
			NullIterator()
			{}

			NullIterator(const NullIterator&)
			{}

			template<class ModelT> NullIterator(const ModelT&)
			{}

			static void forward(difference_type)
			{
				// Do nothing
			}

			template<class ModelT>
			static void reset(const ModelT&)
			{
				// Do nothing
			}

			bool equals(const NullIterator&)
			{
				return true;
			}

			template<class ModelT> bool equals(const ModelT& model)
			{
				return (model.pData == NULL);
			}

			template<class ModelT>
			difference_type distance(const ModelT&) const
			{
				return 0;
			}

			// ::Char& operator * ()

			// ::Char* operator -> ()

		}; // class NullIterator



		/*!
		** \brief
		**
		** \tparam FlagsT See enum Yuni::IO::Directory::Info::OptionIterator
		** \see enum Yuni::IO::Directory::Info::OptionIterator
		*/
		template<uint FlagsT>
		class Iterator
		{
		public:
			//! The type of the orignal object
			typedef String value_type;
			//! An uintegral type
			typedef uint64 uint64ype;
			//! A signed integral type
			typedef sint64 difference_type;

			enum
			{
				//! A non-zero value if the iterator can go forward
				canGoForward  = true,
				//! A non-zero value if the iterator can go backward
				canGoBackward = false,
			};

		public:
			bool isFile() const
			{
				return Private::IO::Directory::IteratorDataIsFile(pData);
			}

			bool isFolder() const
			{
				return Private::IO::Directory::IteratorDataIsFolder(pData);
			}

			const String& filename() const
			{
				return Private::IO::Directory::IteratorDataFilename(pData);
			}

			const String& parent() const
			{
				return Private::IO::Directory::IteratorDataParentName(pData);
			}

			//! Size in bytes
			uint64 size() const
			{
				return Private::IO::Directory::IteratorDataSize(pData);
			}

			//! Date of the last modification
			sint64 modified() const
			{
				return Private::IO::Directory::IteratorDataModified(pData);
			}

			bool valid() const
			{
				return pData != NULL;
			}

			bool operator ! () const
			{
				return !pData;
			}

		protected:
			Iterator() :
				pData(NULL)
			{}
			template<class StringT> explicit Iterator(const StringT& directory)
			{
				// Initializing
				pData = Private::IO::Directory::IteratorDataCreate(AnyString(directory), FlagsT);
				// We must forward once to get the first item
				forward();
			}

			Iterator(const NullIterator&) :
				pData(NULL)
			{}

			Iterator(const Iterator& copy) :
				pData(Private::IO::Directory::IteratorDataCopy(copy.pData))
			{
			}

			~Iterator()
			{
				Private::IO::Directory::IteratorDataFree(pData);
			}

			void forward()
			{
				pData = Private::IO::Directory::IteratorDataNext(pData);
			}

			void forward(difference_type n)
			{
				while (n > 0)
				{
					forward();
					--n;
				}
			}

			template<class ModelT>
			void reset(const ModelT& model)
			{
				if (pData)
					Private::IO::Directory::IteratorDataFree(pData);
				pData = Private::IO::Directory::IteratorDataCopy(model.pData);
			}

			template<class ModelT>
			difference_type distance(const ModelT&) const
			{
				return 0;
			}

			bool equals(const NullIterator&) const
			{
				return (pData == NULL);
			}

			template<class ModelT>
			bool equals(const ModelT& model) const
			{
				return (not pData and not model.pData);
			}

			const String& operator * ()
			{
				assert(pData != NULL);
				return Private::IO::Directory::IteratorDataName(pData);
			}

			const String* operator -> ()
			{
				assert(pData != NULL);
				return &Private::IO::Directory::IteratorDataName(pData);
			}

			Iterator& operator = (const Iterator& copy)
			{
				Private::IO::Directory::IteratorDataFree(pData);
				pData = Private::IO::Directory::IteratorDataCopy(copy.pData);
				return *this;
			}

		private:
			//! Platform-dependant data implementation
			Private::IO::Directory::IteratorData* pData;

		}; // class Iterator



	}; // class Model



#endif // __YUNI_IO_DIRECTORY_INFO_ITERATOR_H__
