#include<fill-matrix.h>


template<class T = double, class ReadWriteT = T>
class Matrix_load_bypass : public Matrix_easy_to_fill<T, ReadWriteT>
{
public:
	Matrix_load_bypass() :
		Matrix_easy_to_fill(),
		loadFromCSVFile_called(false)
	{};

	Matrix_load_bypass(uint height, uint width) :
		Matrix_easy_to_fill<T, ReadWriteT>(height, width),
		loadFromCSVFile_called(false)
	{};

	Matrix_load_bypass(uint height, uint width, const vector<T>& vec) :
		Matrix_easy_to_fill<T, ReadWriteT>(height, width, vec),
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