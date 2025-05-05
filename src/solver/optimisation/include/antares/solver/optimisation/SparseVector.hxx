#include <vector>

template<class T>
class SparseVector
{
public:
    std::size_t size() const
    {
        return isExtracted ? v.size() : m.size();
    }

    T& operator[](std::size_t idx)
    {
        isExtracted = false;
        m.push_back({idx, 0});
        return m.back().second;
    }

    T* data() const
    {
        extract();
        return v.data();
    }

    std::vector<T>& extract() const
    {
        if (!isExtracted)
        {
            isExtracted = true;
            v.assign(m.size(), 0.);
            for (auto [index, coeff]: m)
            {
                v[index] = coeff;
            }
            m.clear();
        }
        return v;
    }

private:
    mutable bool isExtracted = false;
    mutable std::vector<std::pair<int, T>> m;
    mutable std::vector<T> v;
};
