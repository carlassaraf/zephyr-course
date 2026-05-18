/*
 * Calculator Module - Implementation
 */

#include "calculator.h"

#include <errno.h>
#include <stddef.h>
#include <zephyr/logging/log.h>

LOG_MODULE_REGISTER(calculator);

static struct {
	int  last_result;
	bool has_result;
} calc;

void calculator_reset(void)
{
	calc.last_result = 0;
	calc.has_result  = false;
}

int calculator_get_last_result(int *result)
{
	if (result == NULL) {
		LOG_ERR("result pointer is NULL");
		return -EINVAL;
	}
	if (!calc.has_result) {
		LOG_WRN("no result cached yet");
		return -ENODATA;
	}
	*result = calc.last_result;

	return 0;
}

int calculator_add(int a, int b, int *result)
{
	if (result == NULL) {
		LOG_ERR("result pointer is NULL");
		return -EINVAL;
	}
	*result          = a + b;
	calc.last_result = *result;
	calc.has_result  = true;

	return 0;
}

int calculator_sub(int a, int b, int *result)
{
	if (result == NULL) {
		LOG_ERR("result pointer is NULL");
		return -EINVAL;
	}
	*result          = a - b;
	calc.last_result = *result;
	calc.has_result  = true;

	return 0;
}

int calculator_mul(int a, int b, int *result)
{
	if (result == NULL) {
		LOG_ERR("result pointer is NULL");
		return -EINVAL;
	}
	*result          = a * b;
	calc.last_result = *result;
	calc.has_result  = true;

	return 0;
}

int calculator_div(int a, int b, int *result)
{
	if (result == NULL) {
		LOG_ERR("result pointer is NULL");
		return -EINVAL;
	}
	if (b == 0) {
		LOG_WRN("division by zero");
		return -EINVAL;
	}
	*result          = a / b;
	calc.last_result = *result;
	calc.has_result  = true;

	return 0;
}
