/*
  +----------------------------------------------------------------------+
  | PHP Version 7                                                        |
  +----------------------------------------------------------------------+
  | Copyright (c) 1997-2015 The PHP Group                                |
  +----------------------------------------------------------------------+
  | This source file is subject to version 3.01 of the PHP license,      |
  | that is bundled with this package in the file LICENSE, and is        |
  | available through the world-wide-web at the following url:           |
  | http://www.php.net/license/3_01.txt                                  |
  | If you did not receive a copy of the PHP license and are unable to   |
  | obtain it through the world-wide-web, please send a note to          |
  | license@php.net so we can mail you a copy immediately.               |
  +----------------------------------------------------------------------+
  | Author:                                                              |
  +----------------------------------------------------------------------+
*/

/* $Id$ */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "php.h"
#include "ext/standard/info.h"
#include "zend_smart_str.h"
#include "php_cg_bcode.h"

/* If you declare any globals in php_cg_bcode.h uncomment this:
ZEND_DECLARE_MODULE_GLOBALS(cg_bcode)
*/

/* True global resources - no need for thread safety here */
static int le_cg_bcode;

/* Remove the following function when you have successfully modified config.m4
   so that your module can be compiled into PHP, it exists only for testing
   purposes. */

/* Every user-visible function in PHP should document itself in the source */
/* {{{ proto string confirm_cg_bcode_compiled(string arg)
   Return a string to confirm that the module is compiled in */
PHP_FUNCTION(confirm_cg_bcode_compiled)
{
	char *arg = NULL;
	size_t arg_len, len;
	zend_string *strg;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "s", &arg, &arg_len) == FAILURE) {
		return;
	}

	strg = strpprintf(0, "Congratulations! You have successfully modified ext/%.78s/config.m4. Module %.78s is now compiled into PHP.", "cg_bcode", arg);

	RETURN_STR(strg);
}
/* }}} */
/* The previous line is meant for vim and emacs, so it can correctly fold and
   unfold functions in source code. See the corresponding marks just before
   function definition, where the functions purpose is also documented. Please
   follow this convention for the convenience of others editing your code.
*/

/* {{{ proto string bencode(mixed php_variable)
    */
PHP_FUNCTION(bencode)
{
	int argc = ZEND_NUM_ARGS();
	zval *php_variable = NULL;
	smart_str buf = {0};

	if (zend_parse_parameters(argc TSRMLS_CC, "z", &php_variable) == FAILURE) 
		return;

	php_bencode_encode(&buf, php_variable);
	smart_str_0(&buf);
	if (buf.s) {
		RETURN_STR(buf.s);
	} else {
		RETURN_EMPTY_STRING();
	}
}
/* }}} */

/* {{{ proto mixed bdecode(string bencoded_str)
    */
PHP_FUNCTION(bdecode)
{
	char *bencoded_str = NULL;
	int argc = ZEND_NUM_ARGS();
	size_t bencoded_str_len;

	if (zend_parse_parameters(argc TSRMLS_CC, "s", &bencoded_str, &bencoded_str_len) == FAILURE) 
		return;

	php_error(E_WARNING, "bdecode: not yet implemented");
}
/* }}} */

static inline int php_bencode_is_valid_double(double d) /* {{{ */
{
	return !zend_isinf(d) && !zend_isnan(d);
}
/* }}} */

PHP_CG_BCODE_API void php_bencode_encode(smart_str *buf, zval *val)
{
again:
	switch (Z_TYPE_P(val))
	{
		case IS_NULL:
			break;
		case IS_TRUE:
			smart_str_appendl(buf, "i1e", 3);
			break;
		case IS_FALSE:
			smart_str_appendl(buf, "i0e", 3);
			break;
		case IS_LONG:
			smart_str_appendc(buf, 'i');
			smart_str_append_long(buf, Z_LVAL_P(val));
			smart_str_appendc(buf, 'e');
			break;
		case IS_DOUBLE:
			smart_str_appendc(buf, 'i');
			if(php_bencode_is_valid_double(Z_DVAL_P(val))){
				smart_str_append_long(buf, zval_get_long(val));
			}else{
				zend_error(E_WARNING, "cg_bcode: Cannot convert infinite or NaN to bencode, encoded as 0");
				smart_str_appendc(buf, '0');
			}
			smart_str_appendc(buf, 'e');
			break;
		case IS_STRING:
			smart_str_append_long(buf, Z_STRLEN_P(val));
            smart_str_appendc(buf, ':');
            smart_str_appendl(buf, Z_STRVAL_P(val), Z_STRLEN_P(val));
			break;
		case IS_ARRAY:
			break;
		case IS_REFERENCE:
			val = Z_REFVAL_P(val);
			goto again;
		default:
			zend_error(E_WARNING, "cg_bcode: Unsuppported variable type.");
			break;
	}
}

/* {{{ PHP_MINIT_FUNCTION
 */
PHP_MINIT_FUNCTION(cg_bcode)
{
	/* If you have INI entries, uncomment these lines
	REGISTER_INI_ENTRIES();
	*/
	return SUCCESS;
}
/* }}} */

/* {{{ PHP_MSHUTDOWN_FUNCTION
 */
PHP_MSHUTDOWN_FUNCTION(cg_bcode)
{
	/* uncomment this line if you have INI entries
	UNREGISTER_INI_ENTRIES();
	*/
	return SUCCESS;
}
/* }}} */

/* {{{ PHP_MINFO_FUNCTION
 */
PHP_MINFO_FUNCTION(cg_bcode)
{
	php_info_print_table_start();
	php_info_print_table_row(2, "cg_bcode support", "enabled");
	php_info_print_table_row(2, "bencode version", PHP_CG_BCODE_VERSION);
	php_info_print_table_end();

	/* Remove comments if you have entries in php.ini
	DISPLAY_INI_ENTRIES();
	*/
}
/* }}} */

/* {{{ cg_bcode_functions[]
 *
 * Every user visible function must have an entry in cg_bcode_functions[].
 */
const zend_function_entry cg_bcode_functions[] = {
	PHP_FE(confirm_cg_bcode_compiled,	NULL)		/* For testing, remove later. */
	PHP_FE(bencode,	NULL)
	PHP_FE(bdecode,	NULL)
	PHP_FE_END	/* Must be the last line in cg_bcode_functions[] */
};
/* }}} */

/* {{{ cg_bcode_module_entry
 */
zend_module_entry cg_bcode_module_entry = {
	STANDARD_MODULE_HEADER,
	"cg_bcode",
	cg_bcode_functions,
	PHP_MINIT(cg_bcode),
	PHP_MSHUTDOWN(cg_bcode),
	NULL,		/* Replace with NULL if there's nothing to do at request start */
	NULL,	/* Replace with NULL if there's nothing to do at request end */
	PHP_MINFO(cg_bcode),
	PHP_CG_BCODE_VERSION,
	STANDARD_MODULE_PROPERTIES
};
/* }}} */

#ifdef COMPILE_DL_CG_BCODE
#ifdef ZTS
ZEND_TSRMLS_CACHE_DEFINE();
#endif
ZEND_GET_MODULE(cg_bcode)
#endif

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */
