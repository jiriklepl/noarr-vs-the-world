#include <chrono>
#include <iostream>
#include <utility>

#include "noarr/structures_extended.hpp"
#include "noarr/structures/interop/bag.hpp"
#include "noarr/structures/extra/traverser.hpp"
#include "noarr/structures/interop/serialize_data.hpp"

template<class Dist>
auto run_test(Dist dist) {
	auto start = std::chrono::high_resolution_clock::now();

	noarr::traverser(dist).for_each([dist](auto state){ dist[state] = 2 * noarr::get_index<'i'>(state) + noarr::get_index<'j'>(state); });
	noarr::traverser(dist).template for_each<'i'>([&, dist](auto state){ dist[state & noarr::idx<'j'>(noarr::get_index<'i'>(state))] = 0.f; });

	auto to = dist ^ noarr::rename<'j', 'k'>();
	auto from = dist ^ noarr::rename<'i', 'k'>();

	noarr::traverser(dist, to, from).for_each([=](auto state) {
		dist[state] = std::min(dist[state], to[state] + from[state]);
	});

	auto end = std::chrono::high_resolution_clock::now();

	auto dur = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);

	return dur;
}

int main() {
	auto size = noarr::lit<2'000>;

	auto layout = noarr::scalar<float>() ^ noarr::sized_vectors<'i', 'j'>(size, size);

	auto dist = noarr::make_bag(layout);

	// warm-up
	run_test(dist.get_ref());
	run_test(dist.get_ref());
	run_test(dist.get_ref());

	auto dur = run_test(dist.get_ref());

	noarr::serialize_data(std::cout, dist);

	std::cerr << "time: " << dur << std::endl;
}
