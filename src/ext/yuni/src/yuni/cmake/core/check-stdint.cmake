
# int64
check_cxx_source_compiles("#include <stdint.h>
	int main() {int64_t a; return 0;}" YUNI_HAS_INT64_T)
# int128
check_cxx_source_compiles("#include <stdint.h>
	int main() {__int128 a; unsigned __int128 b; return 0;}" YUNI_HAS_INT128_T)

# long
if(MSVC)
	check_cxx_source_compiles("
		#include <iostream>
		#include <stdio.h>
		void foo(unsigned int a) {std::cout << a;}
		void foo(int a) {std::cout << a;}
		void foo(unsigned long a) {std::cout << a;}
		void foo(long a) {std::cout << a;}
		void foo(__int64 a) {std::cout << a;}
		int main() {return 0;}"
		YUNI_HAS_LONG)
else(MSVC)
	if(YUNI_HAS_SYS_TYPES_H)
		check_cxx_source_compiles("
			#include <iostream>
			#include <sys/types.h>
			#include <stdio.h>
			void foo(unsigned int a) {std::cout << a;}
			void foo(int a) {std::cout << a;}
			void foo(unsigned long a) {std::cout << a;}
			void foo(long a) {std::cout << a;}
			void foo(int64_t a) {std::cout << a;}
			int main() {return 0;}"
			YUNI_HAS_LONG)
	else(YUNI_HAS_SYS_TYPES_H)
		check_cxx_source_compiles("
			#include <iostream>
			#include <stdio.h>
			void foo(unsigned int a) {std::cout << a;}
			void foo(int a) {std::cout << a;}
			void foo(unsigned long a) {std::cout << a;}
			void foo(long a) {std::cout << a;}
			void foo(int64_t a) {std::cout << a;}
			int main() {return 0;}"
			YUNI_HAS_LONG)
	endif(YUNI_HAS_SYS_TYPES_H)
endif(MSVC)


if (YUNI_HAS_SYS_TYPES_H)
	check_c_source_compiles(
		"#include <sys/types.h>
		int main() {uint r = 0; return (int) r;}"
		YUNI_HAS_UINT)
	check_c_source_compiles(
		"#include <sys/types.h>
		int main() {ssize_t r = 0; return (int) r;}"
		YUNI_HAS_SSIZE_T)
else()
	check_c_source_compiles(
		"int main() {uint r = 0; return (int) r;}"
		YUNI_HAS_UINT)
	check_c_source_compiles(
		"int main() {ssize_t r = 0; return (int) r;}"
		YUNI_HAS_SSIZE_T)
endif()



