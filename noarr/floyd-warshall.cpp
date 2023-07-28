#include <algorithm>
#include <chrono>
#include <iomanip>
#include <iostream>

#include <noarr/structures_extended.hpp>
#include <noarr/structures/interop/bag.hpp>
#include <noarr/structures/extra/traverser.hpp>
#include <noarr/structures/interop/serialize_data.hpp>

#include "defines.hpp"
#include "floyd-warshall.hpp"

using num_t = DATA_TYPE;

namespace {

// initialization function
void init_array(auto path) {
	// path: i x j

	noarr::traverser(path) | [=](auto state) {
		auto [i, j] = noarr::get_indices<'i', 'j'>(state);

		path[state] = i * j % 7 + 1;

		if ((i + j) % 13 == 0 || (i + j) % 7 == 0 || (i + j) % 11 == 0)
			path[state] = 999;
	};
}


// computation kernel
void kernel_floyd_warshall(auto path) {
	// path: i x j
	auto path_start_k = path ^ noarr::rename<'i', 'k'>();
	auto path_end_k = path ^ noarr::rename<'j', 'k'>();

	noarr::traverser(path, path_start_k, path_end_k)
		.template for_dims<'k'>([=](auto inner) {
			inner | [=](auto state) {
				path[state] = std::min(path_start_k[state] + path_end_k[state], path[state]);
			};
		});
}

} // namespace

int main() {
	// problem size
	std::size_t n = N;

	// problem data
	auto path = noarr::make_bag(noarr::scalar<float>() ^ noarr::sized_vectors<'i', 'j'>(n, n));

	// warm-up
	kernel_floyd_warshall(path.get_ref());
	kernel_floyd_warshall(path.get_ref());
	kernel_floyd_warshall(path.get_ref());

	// initialization
	init_array(path.get_ref());

	auto start = std::chrono::high_resolution_clock::now();

	kernel_floyd_warshall(path.get_ref());

	auto end = std::chrono::high_resolution_clock::now();

	auto dur = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);

	std::cout << std::fixed << std::setprecision(2);
	noarr::serialize_data(std::cout, path.get_ref() ^ noarr::hoist<'i'>());

	std::cerr << "time: " << dur << std::endl;
}
