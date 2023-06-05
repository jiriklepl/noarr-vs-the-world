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

    f(x, y) = random_float();

    RDom r(1, size - 2, 1, size - 2);

    for (int i = 0; i < iterations; i++)
        f(r.x, r.y) = .2f * (f(r.x, r.y) +
            f(r.x - 1, r.y) + f(r.x + 1, r.y) +
            f(r.x, r.y - 1) + f(r.x, r.y + 1));

    Buffer<float> buffer{size, size};

    f.compile_jit();

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
