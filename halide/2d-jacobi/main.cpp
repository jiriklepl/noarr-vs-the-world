#include <chrono>
#include <iostream>
#include <random>
#include <utility>

#include <halide14/Halide.h>

using namespace Halide;

int main() {
	int size = 2000;
	int iterations = 200;
	Var x, y, t;

	Func f;

	f(x, y, t) = random_float();

	RDom r(1, size - 2, 1, size - 2);
	int current = 0;

	for (int i = 0; i < iterations; i++) {
		int next = 1 - current;

		f(r.x, r.y, next) = .2f * (f(r.x, r.y, current) +
			f(r.x - 1, r.y, current) + f(r.x + 1, r.y, current) +
			f(r.x, r.y - 1, current) + f(r.x, r.y + 1, current));

		current = next;
	}

	Buffer<float> buffer{size, size, 2};

	f.compile_jit();

	// warmup
	f.realize(buffer);
	f.realize(buffer);
	f.realize(buffer);

	auto start = std::chrono::high_resolution_clock::now();

	f.realize(buffer);

	auto end = std::chrono::high_resolution_clock::now();

	auto dur = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);

	for (int y = 0; y < buffer.height(); y++) {
		for (int x = 0; x < buffer.width(); x++) {
			std::cout << buffer(x, y) << std::endl;
		}
	}

	std::cerr << "time: " << dur << std::endl;
}
