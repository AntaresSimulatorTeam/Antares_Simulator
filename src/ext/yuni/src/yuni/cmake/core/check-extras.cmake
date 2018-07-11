
# macro va_copy
check_cxx_source_compiles("#include <stdarg.h>
	int main() {va_list a, b; va_copy(a, b);}" YUNI_HAS_VA_COPY)


# GCC
if (NOT MSVC)
	check_cxx_source_compiles("
		int main() {   int i = 3;   int j = __sync_add_and_fetch(&i, 1);   return 0; } " YUNI_HAS_SYNC_ADD_AND_FETCH)

	check_cxx_source_compiles("
		int main() { int a = 1;  if (__builtin_expect(!!(a == 1), 1)) a = 0;   return a; } " YUNI_HAS_GCC_BUILTIN_EXPECT)

	check_cxx_source_compiles("
		int main() { __builtin_popcount(0xFFFFFFFF); return 0; } " YUNI_HAS_GCC_BUILTIN_POPCOUNT)

endif()


if (NOT MSVC)
	check_c_compiler_flag("-w" YUNI_HAS_GCC_NOWARNING)
else()
	check_c_compiler_flag("/nowarn" YUNI_HAS_VS_NOWARNING)
endif()


# posix_fallocate - Linux only
if (UNIX)
	check_cxx_source_compiles("
		#include <fcntl.h>
		int main() {
			return (0 != posix_fallocate(42, 0, 1024)) ? EXIT_FAILURE : 0;
		} " YUNI_HAS_POSIX_FALLOCATE)
endif()


# pthread_getthreadid_np()
if (UNIX)
	check_cxx_source_compiles("
		#include <pthread.h>
		int main() {
			pthread_getthreadid_np();
			return 0;
		} " YUNI_HAS_PTHREAD_GETTHREADID_NP)
endif()

# pthread_attr_setstacksize()
if (UNIX)
	check_cxx_source_compiles("
		#include <pthread.h>
		int main() {
			pthread_attr_setstacksize(NULL, 4096);
			return 0;
		} " YUNI_HAS_PTHREAD_ATTR_SETSTACKSIZE)
endif()
