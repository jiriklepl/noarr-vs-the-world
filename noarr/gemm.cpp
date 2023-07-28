#include <chrono>
#include <iomanip>
#include <iostream>

#include <noarr/structures_extended.hpp>
#include <noarr/structures/interop/bag.hpp>
#include <noarr/structures/extra/traverser.hpp>
#include <noarr/structures/interop/serialize_data.hpp>

#include "defines.hpp"
#include "gemm.hpp"

using num_t = DATA_TYPE;

namespace {

auto run_test(num_t alpha, num_t beta, auto C, auto A, auto B) {
	alpha = (num_t)1.5;
	beta = (num_t)1.2;

	noarr::traverser(C) | [=](auto state) {
		auto [i, j] = noarr::get_indices<'i', 'j'>(state);
		C[state] = (num_t)((i * j + 1) % (C | noarr::get_length<'i'>())) / (C | noarr::get_length<'i'>());
	};

	noarr::traverser(A) | [=](auto state) {
		auto [i, k] = noarr::get_indices<'i', 'k'>(state);
		A[state] = (num_t)(i * (k + 1) % (A | noarr::get_length<'k'>())) / (A | noarr::get_length<'k'>());
	};

	noarr::traverser(B) | [=](auto state) {
		auto [k, j] = noarr::get_indices<'k', 'j'>(state);
		B[state] = (num_t)(k * (j + 2) % (B | noarr::get_length<'j'>())) / (B | noarr::get_length<'j'>());
	};

	auto start = std::chrono::high_resolution_clock::now();

	noarr::traverser(C, A, B)
		.template for_dims<'i', 'j'>([=](auto inner) {
			auto state = inner.state();

			C[state] *= beta;

			inner | [=](auto state) {
				C[state] += alpha * A[state] * B[state];
			};
		});

	auto end = std::chrono::high_resolution_clock::now();

	auto dur = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);

	return std::tuple{C, dur};
}

} // namespace

int main() {
	// problem size
	std::size_t ni = NI, nj = NJ, nk = NK;

	// input data
	num_t alpha, beta;

	auto C = noarr::make_bag(noarr::scalar<num_t>() ^ noarr::sized_vectors<'i', 'j'>(ni, nj));
	auto A = noarr::make_bag(noarr::scalar<num_t>() ^ noarr::sized_vectors<'i', 'k'>(ni, nk));
	auto B = noarr::make_bag(noarr::scalar<num_t>() ^ noarr::sized_vectors<'k', 'j'>(nk, nj));

	// warm-up
	run_test(alpha, beta, C.get_ref(), A.get_ref(), B.get_ref());
	run_test(alpha, beta, C.get_ref(), A.get_ref(), B.get_ref());
	run_test(alpha, beta, C.get_ref(), A.get_ref(), B.get_ref());

	auto [out, dur] = run_test(alpha, beta, C.get_ref(), A.get_ref(), B.get_ref());

	std::cout << std::fixed << std::setprecision(2);
	noarr::serialize_data(std::cout, out ^ noarr::hoist<'i'>());

	std::cerr << "time: " << dur << std::endl;
}
