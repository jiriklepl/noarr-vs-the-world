#include <chrono>
#include <iomanip>
#include <iostream>

#include <noarr/structures_extended.hpp>
#include <noarr/structures/interop/bag.hpp>
#include <noarr/structures/extra/traverser.hpp>
#include <noarr/structures/interop/serialize_data.hpp>

#include "defines.hpp"
#include "jacobi-2d.hpp"

using num_t = DATA_TYPE;

namespace {

auto run_kernel(std::size_t steps, auto A, auto B) {
	using noarr::neighbor;
	auto traverser = noarr::traverser(A, B).order(noarr::bcast<'t'>(steps));

	traverser
		.order(noarr::symmetric_spans<'i', 'j'>(traverser.top_struct(), 1, 1))
		.template for_dims<'t'>([=](auto inner) {
			inner.for_each([=](auto state) {
				B[state] = (num_t).2 * (
					A[state] +
					A[neighbor<'j'>(state, -1)] +
					A[neighbor<'j'>(state, +1)] +
					A[neighbor<'i'>(state, +1)] +
					A[neighbor<'i'>(state, -1)]);
			});

			inner.for_each([=](auto state) {
				A[state] = (num_t).2 * (
					B[state] +
					B[neighbor<'j'>(state, -1)] +
					B[neighbor<'j'>(state, +1)] +
					B[neighbor<'i'>(state, +1)] +
					B[neighbor<'i'>(state, -1)]);
			});
		});
}

auto run_test(std::size_t steps, auto A, auto B) {
	auto n = A | noarr::get_length<'i'>();

	noarr::traverser(A, B) | [=](auto state) {
		auto [i, j] = noarr::get_indices<'i', 'j'>(state);

		A[state] = ((num_t)i * (j + 2) + 2) / n;
		B[state] = ((num_t)i * (j + 3) + 3) / n;
	};

	auto start = std::chrono::high_resolution_clock::now();

	run_kernel(steps, A, B);

	auto end = std::chrono::high_resolution_clock::now();

	auto dur = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);

	return std::tuple{A, dur};
}

}

int main() {
	// problem size
	std::size_t n = N;
	std::size_t t = TSTEPS;

	// data
	auto A = noarr::make_bag(noarr::scalar<num_t>() ^ noarr::sized_vectors<'i', 'j'>(n, n));
	auto B = noarr::make_bag(noarr::scalar<num_t>() ^ noarr::sized_vectors<'i', 'j'>(n, n));

	// warm-up
	run_test(t, A.get_ref(), B.get_ref());
	run_test(t, A.get_ref(), B.get_ref());
	run_test(t, A.get_ref(), B.get_ref());

	auto [out, dur] = run_test(t, A.get_ref(), B.get_ref());

	std::cout << std::fixed << std::setprecision(2);
	noarr::serialize_data(std::cout, out ^ noarr::hoist<'i'>());

	std::cerr << "time: " << dur << std::endl;
}
