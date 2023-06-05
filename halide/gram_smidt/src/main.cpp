#include <chrono>
#include <iostream>
#include <random>
#include <utility>

#include <halide14/Halide.h>

using namespace Halide;

int main() {
    int dim = 200;
    int count = 200;

    Var x, y;
    Func v, u;

    RDom traverse_dims(0, dim);

    v(x, y) = random_float();

    for (int i = 0; i < count; i++) {
        RDom traverse_both(i + 1, count - i - 1, 0, dim);

        v(i, dim) = 0.f;
        v(i, dim) += v(i, traverse_dims) * v(i, traverse_dims);
        v(i, traverse_dims) /= sqrt(v(i, dim));

        v(traverse_both.x, dim) = 0.f;
        v(traverse_both.x, dim) += v(traverse_both.x, traverse_both.y) * v(i, traverse_both.y);

        v(traverse_both.x, traverse_both.y) -= v(traverse_both.x, dim) * v(i, traverse_both.y);

    }

    Buffer<float> buffer{count, dim + 1};

    v.realize(buffer);
    v.realize(buffer);
    v.realize(buffer);

    auto start = std::chrono::high_resolution_clock::now();

    v.realize(buffer);

    auto end = std::chrono::high_resolution_clock::now();


    auto dur = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);

    for (int y = 0; y < count; y++) {
        for (int x = 0; x < dim; x++) {
            std::cout << buffer(y, x) << ',';
        }
    }

    std::cerr << "time: " << dur << std::endl;
}
