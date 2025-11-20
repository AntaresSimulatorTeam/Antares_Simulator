#pragma once

namespace Yuni::Private::BindImpl
{
template<bool>
struct MoveConstructor final
{
    template<class A, class B>
    static void SwapBind(A& self, B& rhs)
    {
        // instanciating the swap method only when the type requires it
        self.swap(rhs.pHolder);
    }
};

template<>
struct MoveConstructor<false> final
{
    template<class A, class B>
    static void SwapBind(A&, B&)
    {
        // empty on purpose
    }
};

} // namespace Yuni::Private::BindImpl
