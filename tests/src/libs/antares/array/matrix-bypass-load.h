

#ifndef __ANTARES_LIBS_ARRAY_MATRIX_BYPASS_LOAD_H__
# define __ANTARES_LIBS_ARRAY_MATRIX_BYPASS_LOAD_H__


#include<fill-matrix.h>

using namespace Yuni;

namespace Antares
{
	namespace UnitTests
	{
		struct PredicateIdentity
		{
			template<class U> inline U operator () (const U& value) const
			{
				return value;
			}
		};
	}
}

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



template<class T = double, class ReadWriteT = T>
class Matrix_mock_load_to_buffer : public Matrix_easy_to_fill<T, ReadWriteT>
{
public:
	Matrix_mock_load_to_buffer() :
		Matrix_easy_to_fill(),
		fake_mtx_heigth_(0),
		fake_mtx_width_(0),
		fake_mtx_precision_(0),
		fake_mtx_print_dimensions_(false),
		fake_mtx_error_when_loading_(IO::errNone)
	{};

	Matrix_mock_load_to_buffer(uint height, uint width) :
		Matrix_easy_to_fill<T, ReadWriteT>(height, width),
		fake_mtx_heigth_(0),
		fake_mtx_width_(0),
		fake_mtx_precision_(0),
		fake_mtx_print_dimensions_(false),
		fake_mtx_error_when_loading_(IO::errNone)
	{};

	Matrix_mock_load_to_buffer(uint height, uint width, const vector<T>& vec) :
		Matrix_easy_to_fill<T, ReadWriteT>(height, width, vec),
		fake_mtx_heigth_(0),
		fake_mtx_width_(0),
		fake_mtx_precision_(0),
		fake_mtx_print_dimensions_(false),
		fake_mtx_error_when_loading_(IO::errNone)
	{};
	
	virtual IO::Error loadFromFileToBuffer(BufferType & buffer, const AnyString& filename, uint64 hardlimit) const
	{
		Matrix_easy_to_fill<double, double> fake_mtx(fake_mtx_heigth_, fake_mtx_width_, fake_mtx_vec_);
		Antares::UnitTests::PredicateIdentity predicate;
		fake_mtx.saveToFileDescriptor(buffer, fake_mtx_precision_, fake_mtx_print_dimensions_, predicate);

		return fake_mtx_error_when_loading_;
	}

	void fake_matrix_to_load(uint height, uint width, vector<ReadWriteT> v) 
	{	
		fake_mtx_vec_ = v;
		fake_mtx_heigth_ = height;
		fake_mtx_width_ = width;
	}

	void fake_mtx_to_be_loaded__set_precision(uint precision) { fake_mtx_precision_ = precision; }
	void fake_mtx_to_be_loaded__print_dimensions(bool print_dims) { fake_mtx_print_dimensions_ = print_dims; }
	void error_when_loading_fake_mtx(IO::Error err) { fake_mtx_error_when_loading_ = err; }

public:
	vector<ReadWriteT> fake_mtx_vec_;
	uint fake_mtx_heigth_;
	uint fake_mtx_width_;
	IO::Error fake_mtx_error_when_loading_;
	uint fake_mtx_precision_;
	bool fake_mtx_print_dimensions_;
};

#endif	// __ANTARES_LIBS_ARRAY_MATRIX_BYPASS_LOAD_H__