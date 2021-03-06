#include <benchmark/benchmark.h>
#include <boost/multiprecision/cpp_int.hpp>
#include <random>
#include <tuple>
#pragma comment(lib, "shlwapi.lib")
using std::pair;
using std::tuple;

pair<uint64_t, int64_t> int128_mul(int64_t _a, int64_t _b);

using namespace boost::multiprecision;
int128_t int128_mul2(int64_t a, int64_t b)
{
	return (int128_t)a * (int128_t)b;
}

std::mt19937_64 mtg{ std::random_device{}() };

void mul128_arguments_applier(benchmark::internal::Benchmark* b)
{
	using i32 = std::numeric_limits<int32_t>;
	using i64 = std::numeric_limits<int64_t>;
	b->Args({ 0, i32::max(), 0, i32::max() });
	b->Args({ i32::min(), 0, i32::min(), 0 });
	b->Args({ i32::min(), i32::max(), i32::min(), i32::max() });
	b->Args({ 0, i64::max(), 0, i64::max() });
	b->Args({ i64::min(), 0, i64::min(), 0 });
	b->Args({ i64::min(), i64::max(), i64::min(), i64::max() });
	b->Args({ i32::min(), i32::max(), i64::min(), i64::max() });
}

void fix32_int128_mul(benchmark::State& state)
{
	std::uniform_int_distribution<int64_t> uida{ state.range(0), state.range(1) };
	std::uniform_int_distribution<int64_t> uidb{ state.range(2), state.range(3) };
	for (auto _ : state)
	{
		state.PauseTiming();
		auto a = uida(mtg);
		auto b = uidb(mtg);
		state.ResumeTiming();
		int128_t x = 0;
		x.backend().resize(4, 4);
		auto r = x.backend().size();
		benchmark::DoNotOptimize(int128_mul(a, b));
	}
}

//BENCHMARK(fix32_int128_mul)->Apply(mul128_arguments_applier);

void boost_int128_mul(benchmark::State& state)
{
	std::uniform_int_distribution<int64_t> uida{ state.range(0), state.range(1) };
	std::uniform_int_distribution<int64_t> uidb{ state.range(2), state.range(3) };
	for (auto _ : state)
	{
		state.PauseTiming();
		auto a = uida(mtg);
		auto b = uidb(mtg);
		state.ResumeTiming();
		benchmark::DoNotOptimize(int128_mul2(a, b));
	}
}

//BENCHMARK(boost_int128_mul)->Apply(mul128_arguments_applier);


tuple<uint64_t, int64_t, int64_t> int128_div_rem(uint64_t low, int64_t high, int64_t divisor);

int128_t int128_div_rem2(int128_t a, int64_t b)
{
	return a / b;
}

void div_rem128_arguments_applier(benchmark::internal::Benchmark* b)
{
	using i32 = std::numeric_limits<int32_t>;
	using i64 = std::numeric_limits<int64_t>;
	b->Args({ i64::min(), i64::max(), i64::min(), -1, 1, i64::max() }); //negative / positive
	b->Args({ i64::min(), i64::max(), i64::min(), -1, i64::min(), -1 }); //negative / negative
	b->Args({ i64::min(), i64::max(), 0, i64::max(), 1, i64::max() }); //positive / positive
	b->Args({ i64::min(), i64::max(), i64::min(), i64::max(), i64::min(), i64::max() }); //mix
	b->Args({ i64::min(), i64::max(), 0, 0, 1, i64::max() }); //64bit+ / 64bit+
	b->Args({ i64::min(), i64::max(), 0, 0, i64::min(), -1 }); //64bit+ / 64bit-
	b->Args({ i64::min(), i64::max(), -1, -1, 1, i64::max() }); //64bit- / 64bit+
	b->Args({ i64::min(), i64::max(), -1, -1, i64::min(), -1 }); //64bit- / 64bit-
	b->Args({ 0, 0, 0, i64::max(), 1, i64::max() }); //hi64bit+ / 64bit+
	b->Args({ 0, 0, 0, i64::max(), i64::min(), -1 }); //hi64bit+ / 64bit-
	b->Args({ 0, 0, i64::min(), -1, 1, i64::max() }); //hi64bit- / 64bit+
	b->Args({ 0, 0, i64::min(), -1, i64::min(), -1 }); //hi64bit- / 64bit-
}

int128_t make_int128_t(uint64_t lo, uint64_t hi)
{
	return (((int128_t)(int64_t)hi) << 64) | (int128_t)lo;
}

void fix32_int128_div_rem(benchmark::State& state)
{
	std::uniform_int_distribution<int64_t> uida{ state.range(0), state.range(1) };
	std::uniform_int_distribution<int64_t> uidb{ state.range(2), state.range(3) };
	std::uniform_int_distribution<int64_t> uidc{ state.range(4), state.range(5) };
	for (auto _ : state)
	{
		state.PauseTiming();
		auto a = uida(mtg);
		auto b = uidb(mtg);
		auto c = uidc(mtg);
		while (c == 0)
		{
			c = uidc(mtg);
		}
		state.ResumeTiming();
		benchmark::DoNotOptimize(int128_div_rem((uint64_t)a, b, c));
	}
}

//BENCHMARK(fix32_int128_div_rem)->Apply(div_rem128_arguments_applier);

void boost_int128_div_rem(benchmark::State& state)
{
	std::uniform_int_distribution<int64_t> uida{ state.range(0), state.range(1) };
	std::uniform_int_distribution<int64_t> uidb{ state.range(2), state.range(3) };
	std::uniform_int_distribution<int64_t> uidc{ state.range(4), state.range(5) };
	for (auto _ : state)
	{
		state.PauseTiming();
		auto a = uida(mtg);
		auto b = uidb(mtg);
		auto c = uidc(mtg);
		while (c == 0)
		{
			c = uidc(mtg);
		}
		state.ResumeTiming();
		benchmark::DoNotOptimize(int128_div_rem2(make_int128_t((uint64_t)a, b), c));
	}
}

//BENCHMARK(boost_int128_div_rem)->Apply(div_rem128_arguments_applier);
#ifdef _WIN64
struct retval {
	uint64_t lo, hi, rem;
};

extern "C" retval uint128div(uint64_t lo, uint64_t hi, uint64_t div);
void asm_int128_div_rem(benchmark::State& state)
{
	std::uniform_int_distribution<int64_t> uida{ state.range(0), state.range(1) };
	std::uniform_int_distribution<int64_t> uidb{ state.range(2), state.range(3) };
	std::uniform_int_distribution<int64_t> uidc{ state.range(4), state.range(5) };
	for (auto _ : state)
	{
		state.PauseTiming();
		auto a = uida(mtg);
		auto b = uidb(mtg);
		auto c = uidc(mtg);
		while (c == 0)
		{
			c = uidc(mtg);
		}
		state.ResumeTiming();
		benchmark::DoNotOptimize(uint128div((uint64_t)a, b, c));
	}
}

//BENCHMARK(asm_int128_div_rem)->Apply(div_rem128_arguments_applier);
#endif

int clzll(uint64_t value);

int64_t fixmath32_fix32_div(int64_t a, int64_t b)
{
	// This uses a hardware 64/64 bit division multiple times, until we have
	// computed all the bits in (a<<33)/b. Usually this takes 1-3 iterations.

	uint64_t remainder = (a >= 0) ? a : (-a);
	uint64_t divider = (b >= 0) ? b : (-b);
	uint64_t quotient = 0;
	int bit_pos = 33;

	// Kick-start the division a bit.
	// This improves speed in the worst-case scenarios where N and D are large
	// It gets a lower estimate for the result by N/(D >> 17 + 1).
	// if (divider & 0xFFFFFFF000000000LL)
	// {
	//     uint64_t shifted_div = ((divider >> 17) + 1);
	//     quotient = remainder / shifted_div;
	//     remainder -= ((uint64_t)quotient * divider) >> 17;
	// }

	// If the divider is divisible by 2^n, take advantage of it.
	while (!(divider & 0xF) && bit_pos >= 4) {
		divider >>= 4;
		bit_pos -= 4;
	}

	while (remainder && bit_pos >= 0) {
		// Shift remainder as much as we can without overflowing
		int shift = clzll(remainder);
		if (shift > bit_pos) shift = bit_pos;
		remainder <<= shift;
		bit_pos -= shift;

		uint64_t div = remainder / divider;
		remainder = remainder % divider;
		quotient += div << bit_pos;

		remainder <<= 1;
		bit_pos--;
	}

	quotient++;

	int64_t result = quotient >> 1;

	// Figure out the sign of the result
	if ((a ^ b) & 0x8000000000000000LL) {
		result = -result;
	}

	return result;
}

void uint96div_arguments_applier(benchmark::internal::Benchmark* b)
{
	using u32 = std::numeric_limits<int32_t>;
	using u64 = std::numeric_limits<int64_t>;
	b->ArgNames({"a_hi", "a_lo", "b_hi", "b_lo"});
	b->Args({1, 1, 1, 1 }); //aa/bb
	b->Args({1, 1, 0, 1 }); //aa/0b
	b->Args({0, 1, 1, 1 }); //0a/bb
	b->Args({0, 1, 0, 1 }); //0a/0b
	b->Args({0, 1, 1, 0 }); //0a/b0
	b->Args({ 1, 0, 1, 0 }); //a0/b0
	b->Args({ 1, 1, 1, 0 }); //aa/b0
	b->Args({ 1, 0, 1, 1 }); //a0/bb
	b->Args({ 1, 0, 0, 1 }); //a0/0b
}

tuple<uint64_t, uint64_t, uint64_t> shifted_uint64_div(uint64_t a, uint64_t b);
void knuth_uint96_div(benchmark::State& state) {
	std::uniform_int_distribution<uint64_t> uid{ 1, 0x7FFF'FFFF };
	auto ahi = state.range(0);
	auto alo = state.range(1);
	auto bhi = state.range(2);
	auto blo = state.range(3);
	for (auto _ : state)
	{
		state.PauseTiming();
		auto a = (ahi ? uid(mtg) << 32 : 0) | (alo ? uid(mtg) : 0);
		auto b = (bhi ? uid(mtg) << 32 : 0) | (blo ? uid(mtg) : 0);
		state.ResumeTiming();
		benchmark::DoNotOptimize(shifted_uint64_div(a, b));
	}
	state.SetItemsProcessed(state.iterations());
}

BENCHMARK(knuth_uint96_div)->Apply(uint96div_arguments_applier);

void boost_uint96_div(benchmark::State& state) {
	std::uniform_int_distribution<uint64_t> uid{ 1, 0x7FFF'FFFF };
	auto ahi = state.range(0);
	auto alo = state.range(1);
	auto bhi = state.range(2);
	auto blo = state.range(3);
	for (auto _ : state)
	{
		state.PauseTiming();
		auto a = (ahi ? uid(mtg) << 32 : 0) | (alo ? uid(mtg) : 0);
		auto b = (bhi ? uid(mtg) << 32 : 0) | (blo ? uid(mtg) : 0);
		state.ResumeTiming();
		benchmark::DoNotOptimize((uint128_t(a) << 32) / b);
	}
	state.SetItemsProcessed(state.iterations());
}

BENCHMARK(boost_uint96_div)->Apply(uint96div_arguments_applier);

void fix32_int96_div(benchmark::State& state) {
	std::uniform_int_distribution<uint64_t> uid{ 1, 0x7FFF'FFFF };
	auto ahi = state.range(0);
	auto alo = state.range(1);
	auto bhi = state.range(2);
	auto blo = state.range(3);
	for (auto _ : state)
	{
		state.PauseTiming();
		auto a = (ahi ? uid(mtg)<<32 : 0) | (alo ? uid(mtg) : 0);
		auto b = (bhi ? uid(mtg)<<32 : 0) | (blo ? uid(mtg) : 0);
		state.ResumeTiming();
		benchmark::DoNotOptimize(int128_div_rem(a << 32, a >> 32, b));
	}
	state.SetItemsProcessed(state.iterations());
}

BENCHMARK(fix32_int96_div)->Apply(uint96div_arguments_applier);

void fixmath32_div(benchmark::State& state)
{
	std::uniform_int_distribution<uint64_t> uid{ 1, 0x7FFF'FFFF };
	auto ahi = state.range(0);
	auto alo = state.range(1);
	auto bhi = state.range(2);
	auto blo = state.range(3);
	for (auto _ : state) {
		state.PauseTiming();
		auto a = (ahi ? uid(mtg) << 32 : 0) | (alo ? uid(mtg) : 0);
		auto b = (bhi ? uid(mtg) << 32 : 0) | (blo ? uid(mtg) : 0);
		state.ResumeTiming();
		benchmark::DoNotOptimize(fixmath32_fix32_div(a, b));
	}
	state.SetItemsProcessed(state.iterations());
}

BENCHMARK(fixmath32_div)->Apply(uint96div_arguments_applier);

BENCHMARK_MAIN();