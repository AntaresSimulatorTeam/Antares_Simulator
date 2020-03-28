#include<matrix.h>
#include<yuni/core/system/stdint.h>

#include<string>
#include<vector>
using namespace std;
using namespace Yuni;
using namespace Antares;


struct TSNumbersPredicate
{
	uint32 operator () (uint32 value) const
	{
		return value + 1;
	}
};

class global_JIT_manager
{
public:
	global_JIT_manager(bool turn_to) : JIT_initial(JIT::enabled)
	{
		JIT::enabled = turn_to;
	}
	~global_JIT_manager() { JIT::enabled = JIT_initial; }
private:
	bool JIT_initial;
};

template<class T = double, class ReadWriteT = T>
class Matrix_enhanced : public Matrix<T, ReadWriteT>
{
public:
	Matrix_enhanced() : Matrix<T, ReadWriteT>() {}
	Matrix_enhanced(uint height, uint width) : Matrix<T, ReadWriteT>(height, width) {}

	Matrix_enhanced(uint height, uint width, const vector<T>& vec)	: Matrix<T, ReadWriteT>()
	{
		BOOST_REQUIRE_EQUAL(height * width, vec.size());
		reset(width, height, true);
		uint count = 0;
		for (uint j = 0; j < height; j++)
			for (uint i = 0; i < width; i++)
			{
				entry[i][j] = vec[count];
				count++;
			}
	}
};


template<class T = double, class ReadWriteT = T>
class Matrix_load_bypass : public Matrix_enhanced<T, ReadWriteT>
{
public:
	Matrix_load_bypass() :
		Matrix_enhanced(),
		loadFromCSVFile_called(false) 
	{};

	Matrix_load_bypass(uint height, uint width) :
		Matrix_enhanced<T, ReadWriteT>(height, width),
		loadFromCSVFile_called(false)
	{};

	Matrix_load_bypass(uint height, uint width, const vector<T>& vec) :
		Matrix_enhanced<T, ReadWriteT>(height, width, vec),
		loadFromCSVFile_called(false)
	{};

	virtual bool loadFromCSVFile(
		const AnyString& filename,
		uint minWidth, uint maxHeight, 
		uint options = optNone,
		BufferType* buffer = NULL)
	{
		loadFromCSVFile_called = true;
		return true;
	}

public:
	bool loadFromCSVFile_called;
};

