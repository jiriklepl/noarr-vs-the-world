#include <chrono>
#include <iostream>
#include <random>
#include <utility>

#include <halide14/Halide.h>

using namespace Halide;

int main() {
    int size = 2'000;

    Var x, y;
    Func dist;

    RDom r(0, size, 0, size, 0, size);

    dist(x, y) = 2.f * x + y;
    dist(x, x) = 0.f;

    dist(r.x, r.y) = min(dist(r.x, r.y), dist(r.x, r.z) + dist(r.z, r.y));

    Buffer<float> buffer{size, size};

    dist.realize(buffer);
    dist.realize(buffer);
    dist.realize(buffer);

    auto start = std::chrono::high_resolution_clock::now();

    dist.realize(buffer);

    auto end = std::chrono::high_resolution_clock::now();


    auto dur = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);

    for (int y = 0; y < buffer.width(); y++) {
        for (int x = 0; x < buffer.height(); x++) {
            std::cout << buffer(x, y) << std::endl;
        }
    }

    std::cerr << "time: " << dur << std::endl;
}
