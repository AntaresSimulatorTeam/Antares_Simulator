#include<matrix.h>
#include<vector>
// #include<yuni/core/system/stdint.h>

using namespace std;
using namespace Antares;

template<class T = double, class ReadWriteT = T>
class Matrix_enhanced : public Matrix<T, ReadWriteT>
{
public:
	Matrix_enhanced() : Matrix<T, ReadWriteT>() {}
	Matrix_enhanced(uint height, uint width) : Matrix<T, ReadWriteT>(height, width) {}

	Matrix_enhanced(uint height, uint width, const vector<T>& vec) : Matrix<T, ReadWriteT>()
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
