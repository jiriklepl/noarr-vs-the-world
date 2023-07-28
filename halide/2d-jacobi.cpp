#include <chrono>
#include <iomanip>
#include <iostream>

#include <halide14/Halide.h>

#include "defines.hpp"
#include "jacobi-2d.hpp"

using num_t = DATA_TYPE;

int main() {
	// problem size
	int n = N;
	int t = TSTEPS;

	Halide::Var x, y, z;
	Halide::Func f, in_f;

	Halide::Buffer<num_t> buffer{n, n, 2};

	/*
		- first 4 arguments are the bounds of the "image"
		- the next 2 emulate the A and B buffers in the original code
		- the last 2 specify the number of time steps
	*/
	Halide::RDom r(1, n - 2, 1, n - 2, 0, 2, 0, t);

	// set initial values
	in_f(x, y, z) = (Halide::cast<num_t>(x) * (y + 3 - z) + 3 - z) / n;

	f(x, y, z) = buffer(x, y, z); // load initial values
	f(r.x, r.y, r.z) = (Halide::Expr)(num_t).2 * (f(r.x, r.y, 1 - r.z) +
		f(r.x, r.y - 1, 1 - r.z) + f(r.x, r.y + 1, 1 - r.z) +
		f(r.x + 1, r.y, 1 - r.z) + f(r.x - 1, r.y, 1 - r.z));

	// warmup
	f.realize(buffer);
	f.realize(buffer);
	f.realize(buffer);

	// store initial values
	in_f.realize(buffer);

	auto start = std::chrono::high_resolution_clock::now();

	f.realize(buffer);

	auto end = std::chrono::high_resolution_clock::now();

	auto dur = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);

	std::cout << std::fixed << std::setprecision(2);
	for (int x = 0; x < buffer.width(); x++) {
		for (int y = 0; y < buffer.height(); y++) {
			std::cout << buffer(x, y, 1) << std::endl;
		}
	}

	std::cerr << "time: " << dur << std::endl;
}
