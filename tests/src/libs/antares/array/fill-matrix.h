

#ifndef __ANTARES_LIBS_ARRAY_MATRIX_FILL_MTX_H__
# define __ANTARES_LIBS_ARRAY_MATRIX_FILL_MTX_H__

#include<matrix.h>
#include<vector>

using namespace std;
using namespace Antares;

template<class T = double, class ReadWriteT = T>
class Matrix_easy_to_fill : public Matrix<T, ReadWriteT>
{
public:
	Matrix_easy_to_fill() : Matrix<T, ReadWriteT>() {}
	Matrix_easy_to_fill(uint height, uint width) : Matrix<T, ReadWriteT>(height, width) {}

	Matrix_easy_to_fill(uint height, uint width, const vector<T>& vec) : Matrix<T, ReadWriteT>()
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

#endif	// __ANTARES_LIBS_ARRAY_MATRIX_FILL_MTX_H__
