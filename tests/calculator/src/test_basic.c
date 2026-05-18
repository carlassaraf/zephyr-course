/*
 * Basic Calculator Unit Tests
 *
 * Run with:
 *   west twister -T tests/calculator -p native_sim
 */

#include <zephyr/ztest.h>
#include "calculator.h"

static void before(void *f)
{
	ARG_UNUSED(f);
	calculator_reset();
}

/*
 * ============================================================================
 * Test Suite: calculator_last_result
 *
 * Tests for the last-result cache: calculator_get_last_result() contract
 * (empty cache, NULL guard, reset via calculator_reset()) and how arithmetic
 * operations update the cache.
 * The before hook resets the calculator state before every test.
 * ============================================================================
 */
ZTEST_SUITE(calculator_last_result, NULL, NULL, before, NULL, NULL);

ZTEST(calculator_last_result, test_no_result_before_first_op)
{
	int result;

	zassert_equal(calculator_get_last_result(&result), -ENODATA,
		      "No cached result before first operation");
}

ZTEST(calculator_last_result, test_null_pointer)
{
	zassert_equal(calculator_get_last_result(NULL), -EINVAL,
		      "NULL pointer must return -EINVAL");
}

ZTEST(calculator_last_result, test_add_updates_cache)
{
	int result, cached;

	zassert_ok(calculator_add(3, 4, &result), "Add must succeed");
	zassert_ok(calculator_get_last_result(&cached), "Cache must have a result");
	zassert_equal(cached, result, "Cached result must match add output");
}

ZTEST(calculator_last_result, test_overwrite_on_second_op)
{
	int result, cached;

	zassert_ok(calculator_add(1, 2, &result), "First op must succeed");
	zassert_ok(calculator_mul(5, 6, &result), "Second op must succeed");
	zassert_ok(calculator_get_last_result(&cached), "Cache must have a result");
	zassert_equal(cached, 30, "Cache must reflect the last operation");
}

ZTEST(calculator_last_result, test_failed_op_does_not_update)
{
	int result, cached;

	zassert_ok(calculator_add(7, 3, &result), "Add must succeed");
	zassert_ok(calculator_get_last_result(&cached), "Cache must be 10 after add");
	zassert_equal(cached, 10, "Cache must be 10 after add(7, 3)");

	/* Division by zero fails; the cache must stay at 10 */
	zassert_not_ok(calculator_div(10, 0, &result), "Div by zero must fail");
	zassert_ok(calculator_get_last_result(&cached), "Cache must still be valid");
	zassert_equal(cached, 10, "Cache must not change after a failed operation");
}

ZTEST(calculator_last_result, test_reset_clears_cache)
{
	int result;

	zassert_ok(calculator_add(1, 2, &result), "Add must succeed");
	calculator_reset();
	zassert_equal(calculator_get_last_result(&result), -ENODATA,
		      "Cache must be cleared after reset");
}

/*
 * ============================================================================
 * Test Suite: calculator_add
 *
 * Tests for the calculator_add() function.
 * ============================================================================
 */
ZTEST_SUITE(calculator_add, NULL, NULL, before, NULL, NULL);

ZTEST(calculator_add, test_positive_numbers)
{
	int result;

	zassert_ok(calculator_add(2, 3, &result), "Addition should succeed");
	zassert_equal(result, 5, "2 + 3 should equal 5");
}

ZTEST(calculator_add, test_negative_numbers)
{
	int result;

	zassert_ok(calculator_add(-2, -3, &result), "Addition should succeed");
	zassert_equal(result, -5, "-2 + -3 should equal -5");
}

ZTEST(calculator_add, test_mixed_signs)
{
	int result;

	zassert_ok(calculator_add(-10, 5, &result), "Addition should succeed");
	zassert_equal(result, -5, "-10 + 5 should equal -5");
}

ZTEST(calculator_add, test_zero)
{
	int result;

	zassert_ok(calculator_add(0, 0, &result), "Addition should succeed");
	zassert_equal(result, 0, "0 + 0 should equal 0");

	zassert_ok(calculator_add(42, 0, &result), "Addition should succeed");
	zassert_equal(result, 42, "42 + 0 should equal 42");
}

ZTEST(calculator_add, test_null_pointer)
{
	int ret = calculator_add(1, 2, NULL);

	zassert_equal(ret, -EINVAL, "NULL result pointer should return -EINVAL");
}

/*
 * ============================================================================
 * Test Suite: calculator_sub
 *
 * Tests for the calculator_sub() function.
 * ============================================================================
 */
ZTEST_SUITE(calculator_sub, NULL, NULL, before, NULL, NULL);

ZTEST(calculator_sub, test_positive_result)
{
	int result;

	zassert_ok(calculator_sub(5, 3, &result), "Subtraction should succeed");
	zassert_equal(result, 2, "5 - 3 should equal 2");
}

ZTEST(calculator_sub, test_negative_result)
{
	int result;

	zassert_ok(calculator_sub(3, 5, &result), "Subtraction should succeed");
	zassert_equal(result, -2, "3 - 5 should equal -2");
}

ZTEST(calculator_sub, test_null_pointer)
{
	/* zassert_not_ok: any non-zero return value is an error */
	int ret = calculator_sub(1, 2, NULL);

	zassert_not_ok(ret, "NULL result pointer should return an error");
}

/*
 * ============================================================================
 * Test Suite: calculator_mul
 *
 * Tests for the calculator_mul() function.
 * ============================================================================
 */
ZTEST_SUITE(calculator_mul, NULL, NULL, before, NULL, NULL);

ZTEST(calculator_mul, test_positive_numbers)
{
	int result;

	zassert_ok(calculator_mul(3, 4, &result), "Multiplication should succeed");
	zassert_equal(result, 12, "3 * 4 should equal 12");
}

ZTEST(calculator_mul, test_negative_numbers)
{
	int result;

	zassert_ok(calculator_mul(-3, 4, &result), "Multiplication should succeed");
	zassert_equal(result, -12, "-3 * 4 should equal -12");
}

ZTEST(calculator_mul, test_by_zero)
{
	int result;

	zassert_ok(calculator_mul(100, 0, &result), "Multiplication should succeed");
	zassert_equal(result, 0, "100 * 0 should equal 0");
}

ZTEST(calculator_mul, test_null_pointer)
{
	int ret = calculator_mul(1, 2, NULL);

	zassert_equal(ret, -EINVAL, "NULL result pointer should return -EINVAL");
}

/*
 * ============================================================================
 * Test Suite: calculator_div
 *
 * Tests for the calculator_div() function, including division by zero
 * and integer truncation.
 * ============================================================================
 */
ZTEST_SUITE(calculator_div, NULL, NULL, before, NULL, NULL);

ZTEST(calculator_div, test_exact_division)
{
	int result;

	zassert_ok(calculator_div(10, 2, &result), "Division should succeed");
	zassert_equal(result, 5, "10 / 2 should equal 5");
}

ZTEST(calculator_div, test_integer_truncation)
{
	int result;

	zassert_ok(calculator_div(7, 3, &result), "Division should succeed");
	zassert_equal(result, 2, "7 / 3 should equal 2 (integer truncation)");
}

ZTEST(calculator_div, test_divide_by_zero)
{
	int result = 999;

	int ret = calculator_div(10, 0, &result);

	zassert_equal(ret, -EINVAL, "Division by zero should return -EINVAL");
	zassert_equal(result, 999, "Result should not be modified on error");
}

ZTEST(calculator_div, test_null_pointer)
{
	int ret = calculator_div(1, 2, NULL);

	zassert_not_ok(ret, "NULL result pointer should return an error");
}

ZTEST(calculator_div, test_negative_dividend_truncation)
{
	int result;

	zassert_ok(calculator_div(-10, 3, &result), "Division should succeed");
	zassert_equal(result, -3, "-10 / 3 should equal -3 (truncation toward zero)");
}

/*
 * ============================================================================
 * Test Suite: calculator_edge_cases
 *
 * Covers remaining assertion macros (zassume_*, zexpect_*) and multi-step scenarios. 
 * ============================================================================
 */
ZTEST_SUITE(calculator_edge_cases, NULL, NULL, before, NULL, NULL);

/* zexpect_*: chain of operations - all failures reported, test still finishes */
ZTEST(calculator_edge_cases, test_chained_operations)
{
	int result;

	zassume_ok(calculator_add(10, 20, &result), "precondition: add must succeed");
	zexpect_equal(result, 30, "Expected 10 + 20 = 30, got %d", result);

	zexpect_ok(calculator_sub(result, 5, &result), "sub should succeed");
	zexpect_equal(result, 25, "Expected 30 - 5 = 25, got %d", result);

	zexpect_ok(calculator_mul(result, 2, &result), "mul should succeed");
	zexpect_equal(result, 50, "Expected 25 * 2 = 50, got %d", result);

	zexpect_ok(calculator_div(result, 10, &result), "div should succeed");
	zexpect_equal(result, 5, "Expected 50 / 10 = 5, got %d", result);
}
