#include <chrono>
#include <iomanip>
#include <iostream>

#include <halide14/Halide.h>

#include "defines.hpp"
#include "floyd-warshall.hpp"

using num_t = DATA_TYPE;

int main() {
	// problem size
	int n = N;

	Halide::Var x, y;
	Halide::Func path, in_path;

	Halide::Buffer<num_t> buffer{n, n};

	Halide::RDom r(0, n, 0, n, 0, n);

	// set initial values
	in_path(x, y) = Halide::cast<num_t>(x * y % 7 + 1);
	in_path(x, y) = Halide::select(
		(x + y) % 13 == 0 || (x + y) % 7 == 0 || (x + y) % 11 == 0,
		999,
		in_path(x, y));

	path(x, y) = buffer(x, y); // load initial values
	path(r.x, r.y) = Halide::min(path(r.z, r.y) + path(r.x, r.z), path(r.x, r.y));

	// warmup
	path.realize(buffer);
	path.realize(buffer);
	path.realize(buffer);

	// store initial values
	in_path.realize(buffer);

	auto start = std::chrono::high_resolution_clock::now();

	path.realize(buffer);

	auto end = std::chrono::high_resolution_clock::now();

	auto dur = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);

	std::cout << std::fixed << std::setprecision(2);
	for (int x = 0; x < buffer.height(); x++) {
		for (int y = 0; y < buffer.width(); y++) {
			std::cout << buffer(x, y) << std::endl;
		}
	}

	std::cerr << "time: " << dur << std::endl;
}
