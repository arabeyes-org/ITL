/*
  +----------------------------------------------------------------------+
  | PHP Version 5                                                        |
  +----------------------------------------------------------------------+
  | Author: Yahya Mohammad <mfyahya@gmail.com>                           | 
  +----------------------------------------------------------------------+
*/

/* $Id$ */

#ifndef PHP_ITL_H
#define PHP_ITL_H

/* default sealevel */
#define ITL_DEFAULT_SEALEVEL 0
/* astronomical standard pressue in milibars */
#define ITL_DEFAULT_PRESSURE 1010 
/* astronomical standard temperature in celsius */
#define ITL_DEFAULT_TEMPERATURE 10 

extern zend_module_entry itl_module_entry;
#define phpext_itl_ptr &itl_module_entry

#ifdef PHP_WIN32
#define PHP_ITL_API __declspec(dllexport)
#else
#define PHP_ITL_API
#endif

#ifdef ZTS
#include "TSRM.h"
#endif

PHP_MINIT_FUNCTION(itl);
PHP_MSHUTDOWN_FUNCTION(itl);
PHP_RINIT_FUNCTION(itl);
PHP_RSHUTDOWN_FUNCTION(itl);
PHP_MINFO_FUNCTION(itl);

PHP_FUNCTION(itl_get_prayer_times);
PHP_FUNCTION(itl_get_north_qibla);
PHP_FUNCTION(itl_h_date);
PHP_FUNCTION(itl_g_date);


/* 
  	Declare any global variables you may need between the BEGIN
	and END macros here:     

ZEND_BEGIN_MODULE_GLOBALS(itl)
	long  global_value;
	char *global_string;
ZEND_END_MODULE_GLOBALS(itl)
*/

/* In every utility function you add that needs to use variables 
   in php_itl_globals, call TSRMLS_FETCH(); after declaring other 
   variables used by that function, or better yet, pass in TSRMLS_CC
   after the last function argument and declare your utility function
   with TSRMLS_DC after the last declared argument.  Always refer to
   the globals in your function as ITL_G(variable).  You are 
   encouraged to rename these macros something shorter, see
   examples in any other php module directory.
*/

#ifdef ZTS
#define ITL_G(v) TSRMG(itl_globals_id, zend_itl_globals *, v)
#else
#define ITL_G(v) (itl_globals.v)
#endif

#endif	/* PHP_ITL_H */


/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */
