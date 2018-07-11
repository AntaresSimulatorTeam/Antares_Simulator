#pragma once

// !!! Do not use includes here


struct Model
{

	// Forward declarations
	class NullIterator;
	class UTF8Iterator;
	class ByteIterator;


	class NullIterator
	{
	public:
		//! Type of the original string
		typedef CStringType StringType;

		//! The type of the orignal object
		typedef UTF8::Char value_type;
		//! An uintegral type
		typedef typename CStringType::size_type  size_type;
		//! A signed integral type
		typedef typename CStringType::difference_type  difference_type;

		enum
		{
			iteratorModelForCString = 1,
			//! A non-zero value if the iterator can go forward
			canGoForward  = true,
			//! A non-zero value if the iterator can go backward
			canGoBackward = false,
		};


	public:
		/*!
		** \brief Get the current offset in the string
		*/
		Size offset() const {return StringType::npos;}

		/*!
		** \brief Get a reference to the original string
		*/
		const StringType& string() const {return pString;}

		/*!
		** \brief Get the current offset in the string
		**
		** This method should not be directly used and is only useful
		** for other external iterators
		*/
		Size rawOffset() const {return StringType::npos;}

		/*!
		** \brief Get the current value of the iterator
		*/
		char value() const
		{
			return '\0';
		}


	protected:
		NullIterator(const StringType& s) :
			pString(s)
		{}

		NullIterator(const NullIterator& copy) :
			pString(copy.pString)
		{}

		template<class ModelT>
		NullIterator(const ModelT& copy) :
			pString(copy.string())
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

		template<class ModelT>
		static bool equals(const ModelT& model)
		{
			return (StringType::npos == model.rawOffset());
		}

		template<class ModelT>
		difference_type distance(const ModelT& model)
		{
			return pString.size() - model.offset();
		}

		// UTF8::Char& operator * ()

		// UTF8::Char* operator -> ()

	private:
		//! Reference to the original string
		const StringType& pString;

	}; // class NullIterator



	class UTF8Iterator
	{
	public:
		//! Type of the original string
		typedef CStringType StringType;

		//! The type of the orignal object
		typedef UTF8::Char value_type;
		//! An uintegral type
		typedef typename CStringType::size_type  size_type;
		//! A signed integral type
		typedef typename CStringType::difference_type  difference_type;

		enum
		{
			iteratorModelForCString = 1,
			//! A non-zero value if the iterator can go forward
			canGoForward  = true,
			//! A non-zero value if the iterator can go backward
			canGoBackward = false,
		};

	public:
		/*!
		** \brief Get the current offset in the string
		*/
		Size offset() const
		{
			return (pOffset >= pString.size())
				? (pString.size() - pChar.size())
				: (pOffset - pChar.size());}

		/*!
		** \brief Get a reference to the original string
		*/
		StringType& string() {return pString;}
		const StringType& string() const {return pString;}

		/*!
		** \brief Get the current offset in the string
		**
		** This method should not be directly used and is only useful
		** for other external iterators
		*/
		Size rawOffset() const {return pOffset;}


		bool findFirstNonSpace(Size end)
		{
			do
			{
				if (pChar == ' ' || pChar == '\t' || pChar == '\n' || pChar == '\r')
				{
					forward();
					if (pOffset >= end)
						return false;
				}
				else
					return true;
			}
			while (true);
			return true;
		}

		bool find(const char c, Size endOffset)
		{
			do
			{
				if (pChar != c)
				{
					forward();
					if (pOffset > endOffset)
						return false;
				}
				else
					return true;
			}
			while (true);
			return true;
		}

		/*!
		** \brief Get the current value of the iterator
		*/
		UTF8::Char value() const
		{
			return pChar;
		}


	protected:
		UTF8Iterator(const StringType& s, size_type offset) :
			pOffset(offset),
			pString(const_cast<StringType&>(s))
		{
			// We must forward once to got the first char
			forward();
		}
		UTF8Iterator(const NullIterator& s) :
			pOffset(s.rawOffset()),
			pString(const_cast<StringType&>(s.string()))
		{}

		explicit UTF8Iterator(const StringType& s) :
			pOffset(0),
			pString(const_cast<StringType&>(s))
		{}

		UTF8Iterator(const UTF8Iterator& copy) :
			pChar(copy.pChar),
			pOffset(copy.rawOffset()),
			pString(const_cast<StringType&>(copy.pString))
		{}

		void forward()
		{
			pString.utf8next<true>(pOffset, pChar);
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
			pOffset = model.rawOffset();
			if (!pOffset)
				forward();
			else
			{
				--pOffset;
				forward();
			}
		}

		template<class ModelT>
		difference_type distance(const ModelT& model) const
		{
			return offset() - model.offset();
		}

		template<class ModelT>
		bool equals(const ModelT& model) const
		{
			return (pOffset == model.rawOffset());
		}


		UTF8::Char& operator * ()
		{
			return pChar;
		}

		const UTF8::Char& operator * () const
		{
			return pChar;
		}

		UTF8::Char* operator -> ()
		{
			return &pChar;
		}

		const UTF8::Char* operator -> () const
		{
			return &pChar;
		}

	private:
		UTF8::Char pChar;
		//! Offset in the string
		Size pOffset;
		//! Reference to the original
		StringType& pString;

	}; // class UTF8Iterator



	class ByteIterator /*final*/
	{
	public:
		//! Type of the original string
		typedef CStringType StringType;

		//! The type of the orignal object
		typedef char value_type;
		//! An unsigned  integral type
		typedef typename CStringType::size_type  size_type;
		//! A signed integral type
		typedef typename CStringType::difference_type  difference_type;

		enum
		{
			iteratorModelForCString = 1,
			//! A non-zero value if the iterator can go forward
			canGoForward  = true,
			//! A non-zero value if the iterator can go backward
			canGoBackward = false,
		};

	public:
		/*!
		** \brief Get the current offset in the string
		*/
		Size offset() const {return pOffset;}

		/*!
		** \brief Get a reference to the original string
		*/
		StringType& string() {return pString;}
		const StringType& string() const {return pString;}

		/*!
		** \brief Get the current offset in the string
		**
		** This method should not be directly used and is only useful
		** for other external iterators
		*/
		Size rawOffset() const {return pOffset;}


		bool findFirstNonSpace(Size end)
		{
			do
			{
				if (pString[pOffset] == ' ' || pString[pOffset] == '\t' || pString[pOffset] == '\n' || pString[pOffset] == '\r')
				{
					forward();
					if (pOffset >= end)
						return false;
				}
				else
					return true;
			}
			while (true);
			return true;
		}

		bool find(const char c, Size endOffset)
		{
			do
			{
				if (pString[pOffset] != c)
				{
					forward();
					if (pOffset > endOffset)
						return false;
				}
				else
					return true;
			}
			while (true);
			return true;
		}


		/*!
		** \brief Get the current value of the iterator
		*/
		char value() const
		{
			return static_cast<char>(pString.at(pOffset));
		}


	protected:
		ByteIterator(const StringType& s, size_type offset)
			:pOffset(offset), pString(const_cast<StringType&>(s))
		{}
		ByteIterator(const NullIterator& s)
			:pOffset(s.rawOffset()),
			pString(const_cast<StringType&>(s.string()))
		{}

		explicit ByteIterator(const StringType& s)
			:pOffset(0), pString(const_cast<StringType&>(s))
		{}
		ByteIterator(const ByteIterator& copy)
			:pOffset(copy.rawOffset()),
			pString(const_cast<StringType&>(copy.pString))
		{}
		ByteIterator(const UTF8Iterator& copy)
			:pOffset(copy.offset()),
			pString(const_cast<StringType&>(copy.string()))
		{}

		void forward()
		{
			if (pOffset != StringType::npos && ++pOffset >= pString.size())
				pOffset = StringType::npos;
		}

		void forward(difference_type n)
		{
			pOffset += n;
		}

		template<class ModelT>
		void reset(const ModelT& model)
		{
			pOffset = model.rawOffset();
		}

		template<class ModelT>
		difference_type distance(const ModelT& model) const
		{
			return offset() - model.offset();
		}

		template<class ModelT>
		bool equals(const ModelT& model) const
		{
			return (pOffset == model.rawOffset());
		}

		char& operator * ()
		{
			//assert(pOffset < pString.size());
			return pString[pOffset];
		}

		char operator * () const
		{
			//assert(pOffset < pString.size());
			return pString[pOffset];
		}

		char* operator -> ()
		{
			//assert(pOffset < pString.size());
			return &(pString[pOffset]);
		}

		const char* operator -> () const
		{
			//assert(pOffset < pString.size());
			return &(pString[pOffset]);
		}

	private:
		//! Offset in the string
		size_type pOffset;
		//! Reference to the original
		StringType& pString;

	}; // class ByteIterator


}; // class Model
