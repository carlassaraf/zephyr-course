/*
 * Calculator Module - Public API
 */

#ifndef CALCULATOR_H
#define CALCULATOR_H

#include <stdbool.h>

/**
 * @brief Reset the calculator state.
 *
 * Clears the last-result cache and prepares the module for use.
 */
void calculator_reset(void);

/**
 * @brief Retrieve the result of the most recent successful operation.
 *
 * @param result Pointer where the cached result will be stored.
 *
 * @return 0 on success,
 *         -EINVAL if result is NULL,
 *         -ENODATA if cache is empty
 *                  (calculator_reset was called or no operations have been performed yet).
 */
int calculator_get_last_result(int *result);

/**
 * @brief Add two integers.
 *
 * @return 0 on success, -EINVAL if result is NULL.
 */
int calculator_add(int a, int b, int *result);

/**
 * @brief Subtract two integers.
 *
 * @return 0 on success, -EINVAL if result is NULL.
 */
int calculator_sub(int a, int b, int *result);

/**
 * @brief Multiply two integers.
 *
 * @return 0 on success, -EINVAL if result is NULL.
 */
int calculator_mul(int a, int b, int *result);

/**
 * @brief Divide two integers (integer truncation toward zero).
 *
 * @return 0 on success, -EINVAL if result is NULL or b is 0.
 */
int calculator_div(int a, int b, int *result);

#endif /* CALCULATOR_H */
