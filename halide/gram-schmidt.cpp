#include <chrono>
#include <iomanip>
#include <iostream>

#include <halide14/Halide.h>

#include "defines.hpp"
#include "gramschmidt.hpp"

using num_t = DATA_TYPE;

int main() {
	int ni = NI;
	int nj = NJ;

	// A: i x j
	// R: j x j
	// Q: i x j

	Halide::Var x, y;
	Halide::Func v, arq, in_arq;

	Halide::Buffer<num_t> ARQ{ni + nj + ni, nj};

	Halide::RDom tr_a(0, ni, 0, nj);

	in_arq(x, y) = (Halide::Expr)(num_t)0;
	in_arq(tr_a.x, tr_a.y) = ((Halide::cast<num_t>((tr_a.x * tr_a.y) % ni) / ni) * 100) + 10;

	arq(x, y) = ARQ(x, y);

	Halide::RDom traverse_dims(0, ni);

	for (int i = 0; i < nj; i++) {
		// R_diag
		arq(ni + i, i) = (Halide::Expr)(num_t)0;
		arq(ni + i, i) += arq(traverse_dims, i) * arq(traverse_dims, i);
		arq(ni + i, i) = Halide::cast<num_t>(Halide::sqrt(arq(ni + i, i)));

		arq(ni + nj + traverse_dims, i) = arq(traverse_dims, i) / arq(ni + i, i); // Q

#ifndef USE_LOOP
		Halide::RDom tr_both(0, ni, i + 1, nj - i - 1);

		Halide::Expr dot = arq(ni + nj + tr_both.x, i) * arq(tr_both.x, tr_both.y);
		arq(ni + i, tr_both.y) = (Halide::Expr)(num_t)0;
		arq(ni + i, tr_both.y) += dot;

		arq(tr_both.x, tr_both.y) = arq(tr_both.x, tr_both.y) - arq(ni + nj + tr_both.x, i) * arq(ni + i, tr_both.y);
#else
		for (int j = i + 1; j < nj; j++) {
			arq(ni + i, j) = (Halide::Expr)(num_t)0;
			arq(ni + i, j) += arq(ni + nj + traverse_dims, i) * arq(traverse_dims, j);

			arq(traverse_dims, j) = arq(traverse_dims, j) - arq(ni + nj + traverse_dims, i) * arq(ni + i, j);
		}
#endif
	}

	// warmup

	in_arq.realize(ARQ);

	auto start = std::chrono::high_resolution_clock::now();

	arq.realize(ARQ);

	auto end = std::chrono::high_resolution_clock::now();


	auto dur = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);

	std::cout << std::fixed << std::setprecision(2);
	for (int x = 0; x < ni; x++) {
		for (int y = 0; y < nj; y++) {
			std::cout << ARQ(ni + nj + x, y) << std::endl;
		}
	}

	std::cerr << "time: " << dur << std::endl;
}
