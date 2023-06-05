#include <chrono>
#include <iostream>
#include <random>
#include <utility>

#include "noarr/structures/extra/shortcuts.hpp"
#include "noarr/structures/structs/scalar.hpp"
#include "noarr/structures_extended.hpp"
#include "noarr/structures/interop/bag.hpp"
#include "noarr/structures/extra/traverser.hpp"
#include "noarr/structures/interop/serialize_data.hpp"



template<class V, class U>
float inner_product(V v, U u) {
    float result = 0;

    noarr::traverser(u, v).for_each([&result, u, v](auto state) {
        result += u[state] * v[state];
    });

    return result;
}

template<class V>
auto run_test(V v) {
    //std::random_device rd;
    std::mt19937 gen(42);
    std::uniform_real_distribution<> dis(0.f, 1.f);

    auto start = std::chrono::high_resolution_clock::now();

    auto traverser = noarr::traverser(v);

    traverser.for_each([&, v](auto state){ v[state] = dis(gen); });

    traverser.template for_each<'c'>([=](auto state) {
        auto base_vector = v ^ noarr::fix(state);
        float den = inner_product(base_vector, base_vector);

        traverser
            .order(noarr::shift<'c'>(noarr::get_index<'c'>(state) + 1))
            .template for_each<'c'>([=](auto other_state) {
                auto vector = v ^ noarr::fix(other_state);
                float num = inner_product(vector, base_vector);

                noarr::traverser(vector).for_each([vector, base_vector, alpha = num / den](auto state) {
                    vector[state] -= alpha * base_vector[state];
                });
            });

        noarr::traverser(base_vector).for_each([base_vector, norm = std::sqrt(den)](auto state) {
            base_vector[state] /= norm;
        });
    });

    auto end = std::chrono::high_resolution_clock::now();

    auto dur = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);

    return dur;
}

template<class V>
auto test_test(V v) {
    auto traverser = noarr::traverser(v);

    traverser.template for_each<'c'>([=](auto state) {
        traverser
            .order(noarr::slice<'c'>(0, noarr::get_index<'c'>(state)))
            .template for_each<'c'>([=](auto other_state) {
                auto vector = v ^ noarr::fix(state);
                auto base_vector = v ^ noarr::fix(other_state);

                float should_be_zero = inner_product(vector, base_vector);
                float should_be_one = inner_product(vector, vector);

                std::cerr << should_be_zero << ' ' << should_be_one << std::endl;
            });
    });
}

int main() {
    auto dim = noarr::lit<500>;
    auto count = noarr::lit<500>;

    auto layout = noarr::scalar<float>() ^ noarr::sized_vectors<'c', 'd'>(count, dim);

    auto v = noarr::make_bag(layout);

    // warm-up
    run_test(v.get_ref());
    run_test(v.get_ref());
    run_test(v.get_ref());

    auto dur = run_test(v.get_ref());

    noarr::serialize_data(std::cout, v);

    std::cerr << "time: " << dur << std::endl;
}
