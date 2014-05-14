/*
  +----------------------------------------------------------------------+
  | PHP Version 5                                                        |
  +----------------------------------------------------------------------+
  | Author: Yahya Mohammad <mfyahya@gmail.com>                           | 
  +----------------------------------------------------------------------+
*/

/* $Id$ */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "php.h"
#include "ext/standard/info.h"
#include "php_itl.h"

#include <itl/prayer.h>
#include <itl/hijri.h>

/* If you declare any globals in php_itl.h uncomment this:
ZEND_DECLARE_MODULE_GLOBALS(itl)
*/

static int _itl_set_method(zval **method_in, Method *method); 

/* {{{ itl_functions[]
 *
 * Every user visible function must have an entry in itl_functions[].
 */
zend_function_entry itl_functions[] = {
	PHP_FE(itl_get_prayer_times, NULL)
	PHP_FE(itl_get_north_qibla, NULL)
	PHP_FE(itl_h_date, NULL)
	PHP_FE(itl_g_date, NULL)
	{NULL, NULL, NULL} /* Must be the last line in itl_functions[] */
};
/* }}} */

/* {{{ itl_module_entry
 */
zend_module_entry itl_module_entry = {
#if ZEND_MODULE_API_NO >= 20010901
	STANDARD_MODULE_HEADER,
#endif
	"itl",
	itl_functions,
	PHP_MINIT(itl),
	PHP_MSHUTDOWN(itl),
	NULL,
	NULL,
	PHP_MINFO(itl),
#if ZEND_MODULE_API_NO >= 20010901
	"0.1", /* Replace with version number for your extension */
#endif
	STANDARD_MODULE_PROPERTIES
};
/* }}} */

#ifdef COMPILE_DL_ITL
ZEND_GET_MODULE(itl)
#endif

/* {{{ PHP_MINIT_FUNCTION
 */
PHP_MINIT_FUNCTION(itl)
{
	return SUCCESS;
}
/* }}} */

/* {{{ PHP_MSHUTDOWN_FUNCTION
 */
PHP_MSHUTDOWN_FUNCTION(itl)
{
	return SUCCESS;
}
/* }}} */

/* {{{ PHP_MINFO_FUNCTION
 */
PHP_MINFO_FUNCTION(itl)
{
	php_info_print_table_start();
	php_info_print_table_header(2, "itl support", "enabled");
	php_info_print_table_end();
}
/* }}} */

/* {{{ proto array itl_get_prayer_times(double longitude, double latitude,
 * double gmtDiff, mixed method, integer day, integer month, integer year,
 * boolean dst, double seaLevel = 0, double pressure = 1010, double temperature
 * = 10)
   Get prayer times for a specific location, method and date */
PHP_FUNCTION(itl_get_prayer_times)
{
	Prayer prayer_times_list[6];
	Location location;
	Date date;
	Method method;
	long method_num = 0;
	zend_bool dst = 0;
	zval *method_in;

	int i;
	zval *prayer_time[6];

	location.seaLevel    = ITL_DEFAULT_SEALEVEL;
	location.pressure    = ITL_DEFAULT_PRESSURE;
	location.temperature = ITL_DEFAULT_TEMPERATURE;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "dddzlll|bddd",
		&location.degreeLong,
		&location.degreeLat,
		&location.gmtDiff,
		&method_in,
		&date.day,
		&date.month,
		&date.year,
		&dst,
		&location.seaLevel,
		&location.pressure,
		&location.temperature) == FAILURE
	) {
		RETURN_NULL();
	}
	location.dst = dst ? 1 : 0;

	switch (Z_TYPE_P(method_in)) {
		case IS_NULL:
			getMethod(6, &method); /* use default 6 (umm_alqurra) */
			break;
		case IS_LONG:
			method_num = Z_LVAL_P(method_in);
			if (method_num < 1 || method_num > 7) {
				php_error_docref(NULL TSRMLS_CC, E_WARNING,
					"Invalid calculation method: %d. Must be between 1 and 7", method_num);
				RETURN_NULL();
			}
			getMethod(method_num, &method);
			break;
		case IS_ARRAY:
			convert_to_array_ex(&method_in);
			if (_itl_set_method(&method_in, &method) == -1) {
				php_error_docref(NULL TSRMLS_CC, E_WARNING,
					"Invalid method parameters passed");
				RETURN_NULL();
			};
			break;
		default:
			php_error_docref(NULL TSRMLS_CC, E_WARNING,
				"Invalid method parameter. Must be an integer, array or NULL.");
	}

	getPrayerTimes(&location, &method, &date, (Prayer *) &prayer_times_list);

	array_init(return_value);
	for (i = 0; i < 6; i++) {
		ALLOC_INIT_ZVAL(prayer_time[i]);
		array_init(prayer_time[i]);
		add_assoc_long(prayer_time[i], "hour",   prayer_times_list[i].hour);
		add_assoc_long(prayer_time[i], "minute", prayer_times_list[i].minute);
		add_assoc_long(prayer_time[i], "second", prayer_times_list[i].second);
		add_next_index_zval(return_value, prayer_time[i]);
	}
	return;
}
/* }}} */

/* {{{ _itl_set_method 
 * Fills the Method structure using values from user's zval input.
 */
static int _itl_set_method(zval **method_in, Method *method)
{
	int i;
	zval **z_var;

	getMethod(6, method); /* initialize method structure to UMM_ALQURRA before
	                         setting individual members */
	/* fajr angle */
	if (zend_hash_index_find(Z_ARRVAL_PP(method_in), 0, (void **) &z_var) == SUCCESS) {
		if (Z_TYPE_PP(z_var) != IS_NULL) {
			SEPARATE_ZVAL(z_var);
			convert_to_double(*z_var);
			method->fajrAng = Z_DVAL_PP(z_var);
		}
	} else return -1;

	/* ishaa angle */
	if (zend_hash_index_find(Z_ARRVAL_PP(method_in), 1, (void **) &z_var) == SUCCESS) {
		if (Z_TYPE_PP(z_var) != IS_NULL) {
			SEPARATE_ZVAL(z_var);
			convert_to_double(*z_var);
			method->ishaaAng = Z_DVAL_PP(z_var);
		}
	} else return -1;

	/* imsaak angle */
	if (zend_hash_index_find(Z_ARRVAL_PP(method_in), 2, (void **) &z_var) == SUCCESS) {
		if (Z_TYPE_PP(z_var) != IS_NULL) {
			SEPARATE_ZVAL(z_var);
			convert_to_double(*z_var);
			method->imsaakAng = Z_DVAL_PP(z_var);
		}
	} else return -1;

	/* fajr interval */
	if (zend_hash_index_find(Z_ARRVAL_PP(method_in), 3, (void **) &z_var) == SUCCESS) {
		if (Z_TYPE_PP(z_var) != IS_NULL) {
			SEPARATE_ZVAL(z_var);
			convert_to_long(*z_var);
			method->fajrInv = Z_LVAL_PP(z_var);
		}
	} else return -1;

	/* ishaa interval */
	if (zend_hash_index_find(Z_ARRVAL_PP(method_in), 4, (void **) &z_var) == SUCCESS) {
		if (Z_TYPE_PP(z_var) != IS_NULL) {
			SEPARATE_ZVAL(z_var);
			convert_to_long(*z_var);
			method->ishaaInv = Z_LVAL_PP(z_var);
		}
	} else return -1;

	/* imsaak interval */
	if (zend_hash_index_find(Z_ARRVAL_PP(method_in), 5, (void **) &z_var) == SUCCESS) {
		if (Z_TYPE_PP(z_var) != IS_NULL) {
			SEPARATE_ZVAL(z_var);
			convert_to_long(*z_var);
			method->imsaakInv = Z_LVAL_PP(z_var);
		}
	} else return -1;

	/* rounding */
	if (zend_hash_index_find(Z_ARRVAL_PP(method_in), 6, (void **) &z_var) == SUCCESS) {
		if (Z_TYPE_PP(z_var) != IS_NULL) {
			SEPARATE_ZVAL(z_var);
			convert_to_long(*z_var);
			/* rounding method should be between 0 and 3. see prayer.h */
			if (Z_LVAL_PP(z_var) < 0 || Z_LVAL_PP(z_var) > 3) return -1;
			method->round = Z_LVAL_PP(z_var);
		}
	} else return -1;

	/* mathhab */
	if (zend_hash_index_find(Z_ARRVAL_PP(method_in), 7, (void **) &z_var) == SUCCESS) {
		if (Z_TYPE_PP(z_var) != IS_NULL) {
			SEPARATE_ZVAL(z_var);
			convert_to_long(*z_var);
			/* mathab 1:shafi, 2:hanafi, else error */
			if (Z_LVAL_PP(z_var) < 1 || Z_LVAL_PP(z_var) > 2) return -1;
			method->mathhab = Z_LVAL_PP(z_var);
		}
	} else return -1;

	/* set nearestLat */
	if (zend_hash_index_find(Z_ARRVAL_PP(method_in), 8, (void **) &z_var) == SUCCESS) {
		if (Z_TYPE_PP(z_var) != IS_NULL) {
			SEPARATE_ZVAL(z_var);
			convert_to_double(*z_var);
			method->nearestLat = Z_DVAL_PP(z_var);
		}
	} else return -1;

	/* set extreme method. see prayer.h */
	if (zend_hash_index_find(Z_ARRVAL_PP(method_in), 9, (void **) &z_var) == SUCCESS) {
		SEPARATE_ZVAL(z_var);
		convert_to_long(*z_var);
		method->extreme = Z_LVAL_PP(z_var);
	} else return -1;

	/* enable/disable offsets */
	if (zend_hash_index_find(Z_ARRVAL_PP(method_in), 10, (void **) &z_var) == SUCCESS) {
		if (Z_TYPE_PP(z_var) != IS_NULL) {
			SEPARATE_ZVAL(z_var);
			convert_to_long(*z_var);
			method->offset = Z_LVAL_PP(z_var);
		}
	} else return -1;

	/* get time offsets */
	if (zend_hash_index_find(Z_ARRVAL_PP(method_in), 11, (void **) &z_var) == SUCCESS) {
		if (Z_TYPE_PP(z_var) != IS_NULL) {
			SEPARATE_ZVAL(z_var);
			convert_to_array_ex(z_var);
			for (i = 0; i < 6; i++) {
				zval **z_temp;
				if (zend_hash_index_find(Z_ARRVAL_PP(z_var), i, (void **) &z_temp) == SUCCESS) {
					SEPARATE_ZVAL(z_temp);
					convert_to_double(*z_temp);
					method->offList[i] = Z_LVAL_PP(z_temp);
				}
			}
		}
	} else return -1;

	return 1;
}
/* }}} */

/* {{{ proto itl_get_north_qibla(double longitude, double latitude) 
 * Get Qibla direction measured from north anticlockwise */
PHP_FUNCTION(itl_get_north_qibla)
{
	Location location;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "dd",
		&location.degreeLong, &location.degreeLat) == FAILURE
	) {
		RETURN_NULL();
	}
	RETURN_DOUBLE(getNorthQibla(&location));
	
}
/* }}} */

/* {{{ proto array itl_h_date(integer day, integer month, interger year)
   Convert Gregorian date to Hijri */
PHP_FUNCTION(itl_h_date)
{
	long day, month, year;
	int error_lib = 0;
	sDate hDate;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "lll", &day, &month, &year) == FAILURE) {
		RETURN_NULL();
	}
	error_lib = h_date(&hDate, day, month, year);
	if (error_lib) RETURN_NULL();

	array_init(return_value);
	add_assoc_long(return_value, "day", hDate.day);
	add_assoc_long(return_value, "month", hDate.month);
	add_assoc_long(return_value, "year", hDate.year);
	add_assoc_long(return_value, "weekday", hDate.weekday);
	add_assoc_string(return_value, "units", hDate.units, 1);
	return;
}
/* }}} */

/* {{{ proto array itl_g_date(integer day, integer month, interger year)
   Convert Hijri date to Gregorian */
PHP_FUNCTION(itl_g_date)
{
	long day, month, year;
	int error_lib = 0;
	sDate gDate;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "lll", &day, &month, &year) == FAILURE) {
		RETURN_NULL();
	}
	error_lib = g_date(&gDate, day, month, year);
	if (error_lib) RETURN_NULL();

	array_init(return_value);
	add_assoc_long(return_value, "day", gDate.day);
	add_assoc_long(return_value, "month", gDate.month);
	add_assoc_long(return_value, "year", gDate.year);
	add_assoc_long(return_value, "weekday", gDate.weekday);
	add_assoc_string(return_value, "units", gDate.units, 1);
	return;
}
/* }}} */

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */
