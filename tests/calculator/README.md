# Unit Tests — `calculator`

Unit tests for the `calculator` arithmetic module, demonstrating Ztest
assertion families on `native_sim` and `nrf54l15dk/nrf54l15/cpuapp`.

## What is tested

The `calculator` module provides basic arithmetic operations on `int`s plus a
last-result cache:

```c
void calculator_reset(void);
int  calculator_get_last_result(int *result);
int  calculator_add(int a, int b, int *result);
int  calculator_sub(int a, int b, int *result);
int  calculator_mul(int a, int b, int *result);
int  calculator_div(int a, int b, int *result);
```

All functions except `calculator_reset` return `0` on success or a negative
errno on failure.

## Code structure

```
tests/calculator/
├── CMakeLists.txt    # Links calculator.c directly into the test binary
├── Kconfig           # Sources Kconfig.zephyr for the test build
├── prj.conf          # CONFIG_ZTEST=y, CONFIG_ZTEST_SHUFFLE=y, CONFIG_LOG=y
├── testcase.yaml     # Three scenarios: shuffled, ordered, hw
├── README.md         # This file
└── src/
    └── test_basic.c  # All six suites
```

## Test suites

### `test_basic.c`

| Suite                    | What it covers                                                                                                              |
| ------------------------ | --------------------------------------------------------------------------------------------------------------------------- |
| `calculator_add`         | Positive, negative, mixed-sign operands; identity with zero; NULL pointer guard                                             |
| `calculator_sub`         | Positive and negative results; NULL pointer guard                                                                           |
| `calculator_mul`         | Positive and negative operands; multiplication by zero; NULL pointer guard                                                  |
| `calculator_div`         | Exact division; integer truncation; divide-by-zero; result not modified on error; negative dividend truncation              |
| `calculator_last_result` | `get_last_result` contract (empty cache, NULL guard, post-reset); cache updated after ops; failed op leaves cache unchanged |
| `calculator_edge_cases`  | `zexpect_*` and `zassume_*`: chained multi-op sequence                                                                      |

## Techniques demonstrated

- **`zassert_*`** — fatal assertions that stop the test on first failure:
  `zassert_ok`, `zassert_equal`, `zassert_not_ok`
- **`zexpect_*`** — non-fatal variants; used in `test_chained_operations`
  so all failures in a multi-step sequence are visible in a single run
- **`zassume_*`** — skip (not fail) a test when a setup precondition is
  not met; used to guard the precondition step in chained tests

## Platforms

- `native_sim` — full Zephyr OS compiled to a native Linux executable.
  `CONFIG_ZTEST_SHUFFLE=y` (default scenario) randomizes suite and test
  order to catch hidden ordering dependencies.
- `nrf54l15dk/nrf54l15/cpuapp` — same binary on real hardware (build only
  without `--device-testing`).

## Running the tests

```bash
# All scenarios (shuffled + ordered) on native_sim
west twister -T tests/calculator -p native_sim

# Verbose per-test output
west twister -T tests/calculator -p native_sim -v

# Deterministic order — useful for live demos or failure bisection
west twister -T tests/calculator \
    -s example.unit.calculator.ordered

# Build only (fastest compile check, no execution)
west twister -T tests/calculator -p native_sim -b

# Hardware (requires connected nRF54L15DK)
west twister -T tests/calculator \
    -p nrf54l15dk/nrf54l15/cpuapp \
    --device-testing --device-serial /dev/ttyACM0

# Coverage report
west twister -T tests/calculator -p native_sim \
    --coverage --coverage-tool gcovr \
    --coverage-basedir app/modules/calculator
```

## Expected output

### Twister summary

Output of `west twister -T tests/calculator -p native_sim`:
```
INFO    - 2 of 2 executed test configurations passed (100.00%), 0 built (not run), 0 failed, 0 errored, with no warnings in 15.13 seconds.
INFO    - 46 of 46 executed test cases passed (100.00%) on 1 out of total 1115 platforms (0.09%).
```

### native_sim output

Output of executing tests on `native_sim` by running the compiled executable directly `./twister-out/native_sim_native/host/example.unit.calculator.ordered/zephyr/zephyr.exe`:

```
*** Booting Zephyr OS build v4.2.0 ***
Running TESTSUITE calculator_add
===================================================================
START - test_mixed_signs
 PASS - test_mixed_signs in 0.000 seconds
===================================================================
START - test_negative_numbers
 PASS - test_negative_numbers in 0.000 seconds
===================================================================
START - test_null_pointer
[00:00:00.000,000] <err> calculator: result pointer is NULL
 PASS - test_null_pointer in 0.000 seconds
===================================================================
START - test_positive_numbers
 PASS - test_positive_numbers in 0.000 seconds
===================================================================
START - test_zero
 PASS - test_zero in 0.000 seconds
===================================================================
TESTSUITE calculator_add succeeded
Running TESTSUITE calculator_div
===================================================================
START - test_divide_by_zero
[00:00:00.000,000] <wrn> calculator: division by zero
 PASS - test_divide_by_zero in 0.000 seconds
===================================================================
START - test_exact_division
 PASS - test_exact_division in 0.000 seconds
===================================================================
START - test_integer_truncation
 PASS - test_integer_truncation in 0.000 seconds
===================================================================
START - test_negative_dividend_truncation
 PASS - test_negative_dividend_truncation in 0.000 seconds
===================================================================
START - test_null_pointer
[00:00:00.000,000] <err> calculator: result pointer is NULL
 PASS - test_null_pointer in 0.000 seconds
===================================================================
TESTSUITE calculator_div succeeded
Running TESTSUITE calculator_edge_cases
===================================================================
START - test_chained_operations
 PASS - test_chained_operations in 0.000 seconds
===================================================================
TESTSUITE calculator_edge_cases succeeded
Running TESTSUITE calculator_last_result
===================================================================
START - test_add_updates_cache
 PASS - test_add_updates_cache in 0.000 seconds
===================================================================
START - test_failed_op_does_not_update
[00:00:00.000,000] <wrn> calculator: division by zero
 PASS - test_failed_op_does_not_update in 0.000 seconds
===================================================================
START - test_no_result_before_first_op
[00:00:00.000,000] <wrn> calculator: no result cached yet
 PASS - test_no_result_before_first_op in 0.000 seconds
===================================================================
START - test_overwrite_on_second_op
 PASS - test_overwrite_on_second_op in 0.000 seconds
===================================================================
START - test_reinit_clears_cache
[00:00:00.000,000] <wrn> calculator: no result cached yet
 PASS - test_reinit_clears_cache in 0.000 seconds
===================================================================
TESTSUITE calculator_last_result succeeded
Running TESTSUITE calculator_mul
===================================================================
START - test_by_zero
 PASS - test_by_zero in 0.000 seconds
===================================================================
START - test_negative_numbers
 PASS - test_negative_numbers in 0.000 seconds
===================================================================
START - test_null_pointer
[00:00:00.000,000] <err> calculator: result pointer is NULL
 PASS - test_null_pointer in 0.000 seconds
===================================================================
START - test_positive_numbers
 PASS - test_positive_numbers in 0.000 seconds
===================================================================
TESTSUITE calculator_mul succeeded
Running TESTSUITE calculator_sub
===================================================================
START - test_negative_result
 PASS - test_negative_result in 0.000 seconds
===================================================================
START - test_null_pointer
[00:00:00.000,000] <err> calculator: result pointer is NULL
 PASS - test_null_pointer in 0.000 seconds
===================================================================
START - test_positive_result
 PASS - test_positive_result in 0.000 seconds
===================================================================
TESTSUITE calculator_sub succeeded

------ TESTSUITE SUMMARY START ------

SUITE PASS - 100.00% [calculator_add]: pass = 5, fail = 0, skip = 0, total = 5 duration = 0.000 seconds
 - PASS - [calculator_add.test_mixed_signs] duration = 0.000 seconds
 - PASS - [calculator_add.test_negative_numbers] duration = 0.000 seconds
 - PASS - [calculator_add.test_null_pointer] duration = 0.000 seconds
 - PASS - [calculator_add.test_positive_numbers] duration = 0.000 seconds
 - PASS - [calculator_add.test_zero] duration = 0.000 seconds

SUITE PASS - 100.00% [calculator_div]: pass = 5, fail = 0, skip = 0, total = 5 duration = 0.000 seconds
 - PASS - [calculator_div.test_divide_by_zero] duration = 0.000 seconds
 - PASS - [calculator_div.test_exact_division] duration = 0.000 seconds
 - PASS - [calculator_div.test_integer_truncation] duration = 0.000 seconds
 - PASS - [calculator_div.test_negative_dividend_truncation] duration = 0.000 seconds
 - PASS - [calculator_div.test_null_pointer] duration = 0.000 seconds

SUITE PASS - 100.00% [calculator_edge_cases]: pass = 1, fail = 0, skip = 0, total = 1 duration = 0.000 seconds
 - PASS - [calculator_edge_cases.test_chained_operations] duration = 0.000 seconds

SUITE PASS - 100.00% [calculator_last_result]: pass = 5, fail = 0, skip = 0, total = 5 duration = 0.000 seconds
 - PASS - [calculator_last_result.test_add_updates_cache] duration = 0.000 seconds
 - PASS - [calculator_last_result.test_failed_op_does_not_update] duration = 0.000 seconds
 - PASS - [calculator_last_result.test_no_result_before_first_op] duration = 0.000 seconds
 - PASS - [calculator_last_result.test_overwrite_on_second_op] duration = 0.000 seconds
 - PASS - [calculator_last_result.test_reinit_clears_cache] duration = 0.000 seconds

SUITE PASS - 100.00% [calculator_mul]: pass = 4, fail = 0, skip = 0, total = 4 duration = 0.000 seconds
 - PASS - [calculator_mul.test_by_zero] duration = 0.000 seconds
 - PASS - [calculator_mul.test_negative_numbers] duration = 0.000 seconds
 - PASS - [calculator_mul.test_null_pointer] duration = 0.000 seconds
 - PASS - [calculator_mul.test_positive_numbers] duration = 0.000 seconds

SUITE PASS - 100.00% [calculator_sub]: pass = 3, fail = 0, skip = 0, total = 3 duration = 0.000 seconds
 - PASS - [calculator_sub.test_negative_result] duration = 0.000 seconds
 - PASS - [calculator_sub.test_null_pointer] duration = 0.000 seconds
 - PASS - [calculator_sub.test_positive_result] duration = 0.000 seconds

------ TESTSUITE SUMMARY END ------

===================================================================
RunID: 45b7dde3c7139239e8834982fa7228ec
PROJECT EXECUTION SUCCESSFUL
```
