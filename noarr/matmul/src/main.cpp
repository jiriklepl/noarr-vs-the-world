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

template<class A, class B, class C>
auto kernel(A a, B b, C c) {
    return [=](auto inner) {
        inner.template for_each<'k'>([=](auto state) {
            c[state] += a[state] * b[state];
        });
    };
}

template<class A, class B, class C>
auto run_test(A a, B b, C c) {
    auto start = std::chrono::high_resolution_clock::now();

    noarr::traverser(a).for_each([a](auto state){ a[state] = (int)noarr::get_index<'i'>(state) - (int)noarr::get_index<'j'>(state); });
    noarr::traverser(b).for_each([b](auto state){ b[state] = noarr::get_index<'i'>(state) * noarr::get_index<'j'>(state); });
    noarr::traverser(c).for_each([c](auto state){ c[state] = 0.f; });

    auto a_renamed = a ^ noarr::rename<'j', 'k'>();
    auto b_renamed = b ^ noarr::rename<'i', 'k'>();

    auto traverser = noarr::traverser(a_renamed, b_renamed, c);
    traverser.template for_dims<'i', 'j'>(kernel(a_renamed, b_renamed, c));
    // output is in `from`

    auto end = std::chrono::high_resolution_clock::now();

    auto dur = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);

    return std::tuple{c, dur};
}

int main() {
    auto size = noarr::lit<2000>;

    auto layout = noarr::scalar<float>() ^ noarr::sized_vectors<'i', 'j'>(size, size);

    auto a = noarr::make_bag(layout);
    auto b = noarr::make_bag(layout);
    auto c = noarr::make_bag(layout);

    // warm-up
    run_test(a.get_ref(), b.get_ref(), c.get_ref());
    run_test(a.get_ref(), b.get_ref(), c.get_ref());
    run_test(a.get_ref(), b.get_ref(), c.get_ref());

    auto [out, dur] = run_test(a.get_ref(), b.get_ref(), c.get_ref());

    noarr::serialize_data(std::cout, out);

    std::cerr << "time: " << dur << std::endl;
}
