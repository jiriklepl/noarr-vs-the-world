#include <chrono>
#include <iostream>
#include <random>
#include <utility>

#include "noarr/structures_extended.hpp"
#include "noarr/structures/interop/bag.hpp"
#include "noarr/structures/extra/traverser.hpp"
#include "noarr/structures/interop/serialize_data.hpp"

template<class A, class B>
auto kernel(A a, B b) {
	using noarr::neighbor;
	return [a, b](auto state) {
		b[state] = .2f * (a[state] +
			a[neighbor<'i'>(state, -1)] + a[neighbor<'i'>(state, +1)] +
			a[neighbor<'j'>(state, -1)] + a[neighbor<'j'>(state, +1)]);
	};
}

template<class A, class B, class It>
auto run_test(A a, B b, It it) {
	std::random_device rd;
	std::mt19937 gen(rd());
	std::uniform_real_distribution<> dis(0.f, 1.f);

	noarr::traverser(a).for_each([&, a = a.get_ref()](auto state){ a[state] = dis(gen); });
	noarr::traverser(b).for_each([&, b = b.get_ref()](auto state){ b[state] = 0.f; });

	auto start = std::chrono::high_resolution_clock::now();

	auto from = a.get_ref();
	auto to = b.get_ref();

	auto traverser = noarr::traverser(it, a, b);
	traverser
		.order(noarr::symmetric_spans<'i', 'j'>(traverser.get_struct(), 1, 1))
		.template for_dims<'t'>([&](auto inner) {
			inner.template for_each<'i','j'>(kernel(from, to));
			std::swap(from, to);
		});
	// output is in `from`

	auto end = std::chrono::high_resolution_clock::now();

	auto dur = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);

	return std::tuple{from, dur};
}

int main() {
	auto iterations = noarr::lit<200>;
	auto size = noarr::lit<2000>;
	auto layout = noarr::scalar<float>() ^ noarr::sized_vectors<'j', 'i'>(size, size);

	auto a = noarr::make_bag(layout);
	auto b = noarr::make_bag(layout);
	auto it = noarr::scalar<void>() ^ noarr::bcast<'t'>(iterations);

	// warm-up
	run_test(a.get_ref(), b.get_ref(), it);
	run_test(a.get_ref(), b.get_ref(), it);
	run_test(a.get_ref(), b.get_ref(), it);

	auto [out, dur] = run_test(a.get_ref(), b.get_ref(), it);

	noarr::serialize_data(std::cout, out);

	std::cerr << "time: " << dur << std::endl;
}
