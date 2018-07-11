
// !!! Do not use includes here !!!


/*!
** \brief An iterator
*/
class Iterator
{
public:
	enum
	{
		canGoForward = true,
		canGoBackward = false
	};

	typedef int difference_type;
	typedef T value_type;

public:
	Iterator()
		:pCurrent(NULL)
	{}
	Iterator(const Iterator& rhs)
		:pCurrent(rhs.pCurrent)
	{}
	Iterator(Item* c)
		:pCurrent(c)
	{}

	bool valid() const
	{
		return pCurrent;
	}

	/*!
	** \brief Increment the iterator by the distance 1
	*/
	void forward()
	{
		pCurrent = ((pCurrent) ? pCurrent->next : NULL);
	}

	/*!
	** \brief Increment the iterator by the distance n
	*/
	void forward(difference_type n)
	{
		while (n-- > 0 and pCurrent)
			pCurrent = pCurrent->next;
	}

	/*!
	** \brief Get the distance between this iterator and another one
	*/
	difference_type distance(const Iterator& rhs) const
	{
		Item* iter1 = pCurrent;
		Item* iter2 = rhs.pCurrent;
		difference_type distance = 0;
		while (iter2 != pCurrent and iter1 != rhs.pCurrent)
		{
			++iter1;
			++iter2;
			++distance;
		}
		return distance;
	}

	reference_type operator * ()
	{
		return pCurrent->data;
	}

	const_reference_type operator * () const
	{
		return pCurrent->data;
	}

	pointer_type operator -> ()
	{
		return &pCurrent->data;
	}

	const_pointer_type operator -> () const
	{
		return &pCurrent->data;
	}

	bool equals(const Iterator& rhs) const
	{
		return (pCurrent == rhs.pCurrent);
	}

	void reset(const Iterator& rhs)
	{
		pCurrent = rhs.pCurrent;
	}

private:
	Item* pCurrent;

	friend class LinkedList<T,Alloc>;
};
