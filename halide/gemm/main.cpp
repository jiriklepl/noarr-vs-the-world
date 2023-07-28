#include <chrono>
#include <iostream>
#include <random>
#include <utility>

#include <halide14/Halide.h>

using namespace Halide;

int main() {
	int size = 2000;
	Var x, y;

	Func a, b, c;

	a(x, y) = x - y;
	b(x, y) = x * y;
	c(x, y) = 0.f;

	RDom r(0, size);

	c(x, y) += a(x, r) * b(r, y);

	Buffer<float> buffer{size, size};

	c.realize(buffer);
	c.realize(buffer);
	c.realize(buffer);

	auto start = std::chrono::high_resolution_clock::now();

	c.realize(buffer);

	auto end = std::chrono::high_resolution_clock::now();


	auto dur = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);

	for (int y = 0; y < buffer.width(); y++) {
		for (int x = 0; x < buffer.height(); x++) {
			std::cout << buffer(x, y) << std::endl;
		}
	}

	std::cerr << "time: " << dur << std::endl;
}
