#include <chrono>
#include <iomanip>
#include <iostream>

#include <halide14/Halide.h>

#include "defines.hpp"
#include "gemm.hpp"

using num_t = DATA_TYPE;

int main() {
	// problem size
	int ni = NI, nj = NJ, nk = NK;

	Halide::Var x, y;
	Halide::Expr alpha = Halide::Expr((num_t)1.5), beta = Halide::Expr((num_t)1.2);
	Halide::Func c, a, b, in_c, in_a, in_b;

	Halide::Buffer<num_t> C({ni, nj}, "C");
	Halide::Buffer<num_t> B({nk, nj}, "B");
	Halide::Buffer<num_t> A({ni, nk}, "A");

	in_c(x, y) = Halide::cast<num_t>((x * y + 1) % ni) / ni;
	in_a(x, y) = Halide::cast<num_t>(x * (y + 1) % nk) / nk;
	in_b(x, y) = Halide::cast<num_t>(x * (y + 2) % nj) / nj;

	c(x, y) = C(x, y);
	a(x, y) = A(x, y);
	b(x, y) = B(x, y);

	Halide::RDom r(0, nk);

	c(x, y) *= beta;
	c(x, y) += alpha * a(x, r) * b(r, y);

	c.realize(C);
	c.realize(C);
	c.realize(C);

	in_c.realize(C);
	in_a.realize(A);
	in_b.realize(B);

	auto start = std::chrono::high_resolution_clock::now();

	c.realize(C);

	auto end = std::chrono::high_resolution_clock::now();

	auto dur = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);

	std::cout << std::fixed << std::setprecision(2);
	for (int x = 0; x < C.width(); x++) {
		for (int y = 0; y < C.height(); y++) {
			std::cout << C(x, y) << std::endl;
		}
	}

	std::cerr << "time: " << dur << std::endl;
}
