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

#ifndef TSRMLS_CC
#define TSRMLS_CC // Do nothing since it has been removed since PHP 8
#endif

/* If you declare any globals in php_cg_bcode.h uncomment this:
ZEND_DECLARE_MODULE_GLOBALS(cg_bcode)
*/

/* True global resources - no need for thread safety here */
// static int le_cg_bcode;

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
		smart_str_free(&buf);
		RETURN_EMPTY_STRING();
	}
}
/* }}} */

static void php_bencode_decode_str(zval *return_value, char *str, size_t *pos, size_t *str_len) /* {{{ */
{
	size_t len = 0;
	smart_str buf = {0};
	
	while (*pos < *str_len && str[*pos] != ':') {
		smart_str_appendc(&buf, str[*pos]);
		(*pos)++;
	}
	
	if (str[*pos] != ':') {
		zend_error(E_WARNING, "Invalid bencoded-string, expected semicolon, stop at position %lu.", *pos);
		RETURN_NULL();
	}
	
	(*pos)++;
	
	smart_str_0(&buf);
	len = atoi(ZSTR_VAL(buf.s));
	smart_str_free(&buf);
	
	if (len > 0 && *pos + len - 1 < *str_len) {
		size_t i;
		for (i = 0; i < len; i++, (*pos)++) {
			smart_str_appendc(&buf, str[*pos]);
		}
		smart_str_0(&buf);
		RETVAL_STR(buf.s);
	} else {
		RETVAL_EMPTY_STRING();
	}
	
}
/* }}} */

static void php_bencode_decode_int(zval *return_value, char *str, size_t *pos, size_t *str_len) /* {{{ */
{
	int len = 0;
	double d;
	smart_str buf = {0};
	(*pos)++;
	while (*pos < *str_len && str[*pos] != PHP_BENCODE_END_STRUCTURE) {
		smart_str_appendc(&buf, str[*pos]);
		(*pos)++;
		len++;
	}
	smart_str_0(&buf);
	
	if (str[*pos] != PHP_BENCODE_END_STRUCTURE) {
		smart_str_free(&buf);
		zend_error(E_WARNING, "Invalid bencoded-integer, expected 'e'.");
		RETURN_NULL();
	}
	
	(*pos)++;
	
	ZVAL_STRINGL(return_value, ZSTR_VAL(buf.s), len);
	d = zend_strtod(ZSTR_VAL(buf.s), NULL);
	if (d <= ZEND_LONG_MAX && d >= ZEND_LONG_MIN) {
		convert_to_long(return_value);
	}
	smart_str_free(&buf);
	
}
/* }}} */

static void php_bencode_decode_list(zval *return_value, char *str, size_t *pos, size_t *str_len) /* {{{ */
{
	array_init(return_value);
	(*pos)++;
	while (*pos < *str_len && str[*pos] != PHP_BENCODE_END_STRUCTURE) {
		zval list_value;
		php_bencode_decode(&list_value, str, pos, str_len);
		zend_hash_next_index_insert(Z_ARRVAL_P(return_value), &list_value);
	}
	(*pos)++;
}
/* }}} */

static void php_bencode_decode_dict(zval *return_value, char *str, size_t *pos, size_t *str_len) /* {{{ */
{
	array_init(return_value);
	(*pos)++;
	while (*pos < *str_len && str[*pos] != PHP_BENCODE_END_STRUCTURE) {
		zval dict_key, dict_value;
		php_bencode_decode_str(&dict_key, str, pos, str_len);
		php_bencode_decode(&dict_value, str, pos, str_len);
		if (Z_TYPE(dict_key) == IS_NULL) break;
		if (Z_TYPE(dict_value) != IS_NULL) {
			add_assoc_zval(return_value, Z_STRVAL(dict_key), &dict_value);
		}
	}
	(*pos)++;
}
/* }}} */

PHP_CG_BCODE_API void php_bencode_decode(zval *return_value, char *str, size_t *pos, size_t *str_len) /* {{{ */
{
	if (*str_len > 0 && *pos < *str_len) {
		switch (str[*pos]) {
			case PHP_BENCODE_TYPE_LIST:
				php_bencode_decode_list(return_value, str, pos, str_len);
				break;
			case PHP_BENCODE_TYPE_DICTIONARY:
				php_bencode_decode_dict(return_value, str, pos, str_len);
				break;
			case PHP_BENCODE_TYPE_INTEGER:
				php_bencode_decode_int(return_value, str, pos, str_len);
				break;
			default:
				php_bencode_decode_str(return_value, str, pos, str_len);
		}
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
	size_t pos = 0;

	if (zend_parse_parameters(argc TSRMLS_CC, "s", &bencoded_str, &bencoded_str_len) == FAILURE) 
		return;
	
	if (bencoded_str_len > 0) {
		php_bencode_decode(return_value, bencoded_str, &pos, &bencoded_str_len);
	} else {
		RETVAL_NULL();
	}
}
/* }}} */

static inline int php_bencode_is_valid_double(double d) /* {{{ */
{
	return !zend_isinf(d) && !zend_isnan(d);
}
/* }}} */

// Copied from ext/standard/array.c
static int php_bencode_key_compare_string(
#if PHP_VERSION_ID < 80000
	const void *a, const void *b
#else
	Bucket *f, Bucket *s
#endif
) /* {{{ */
{
#if PHP_VERSION_ID < 80000
	Bucket *f = (Bucket *) a;
	Bucket *s = (Bucket *) b;
#endif
	char *s1, *s2;
	size_t l1, l2;
	char buf1[MAX_LENGTH_OF_LONG + 1];
	char buf2[MAX_LENGTH_OF_LONG + 1];

	if (f->key) {
		s1 = f->key->val;
		l1 = f->key->len;
	} else {
		s1 = zend_print_long_to_buf(buf1 + sizeof(buf1) - 1, f->h);
		l1 = buf1 + sizeof(buf1) - 1 - s1;
	}
	if (s->key) {
		s2 = s->key->val;
		l2 = s->key->len;
	} else {
		s2 = zend_print_long_to_buf(buf2 + sizeof(buf2) - 1, s->h);
		l2 = buf2 + sizeof(buf2) - 1 - s1;
	}
	return zend_binary_strcmp(s1, l1, s2, l2);
}
/* }}} */

static void php_bencode_encode_array(smart_str *buf, zval *val) /* {{{ */
{
	int num_elements = 0;
	char mode;
	HashTable *ht;
	
	if (Z_TYPE_P(val) == IS_ARRAY) {
		ht = Z_ARRVAL_P(val);
	} else {
		ht = Z_OBJPROP_P(val);
		mode = PHP_BENCODE_TYPE_DICTIONARY;
	}
#if PHP_VERSION_ID < 70300
	if (ht && ZEND_HASH_GET_APPLY_COUNT(ht) > 1) {
#else
	if (GC_FLAGS(ht) & GC_PROTECTED) {
#endif
		zend_error(E_WARNING, "recursion detected");
		return;
	}
	num_elements = ht ? zend_hash_num_elements(ht) : 0;
	if (Z_TYPE_P(val) == IS_ARRAY) {
		if (num_elements > 0) {
			zend_string *key;
			zend_ulong index, idx;

			idx = 0;
			mode = PHP_BENCODE_TYPE_LIST;
			ZEND_HASH_FOREACH_KEY(ht, index, key) {
				if (key) {
					mode = PHP_BENCODE_TYPE_DICTIONARY;
					break;
				} else {
					if (index != idx) {
						mode = PHP_BENCODE_TYPE_DICTIONARY;
						break;
					}
				}
				idx++;
			} ZEND_HASH_FOREACH_END();
		}else{
			mode = PHP_BENCODE_TYPE_LIST;
		}
	}
	smart_str_appendc(buf, mode);
	if (num_elements > 0) {
		zend_string *key;
		zval *data;
		zend_ulong index;
		HashTable sorted_ht;
		
		if (mode == PHP_BENCODE_TYPE_DICTIONARY) {
			zend_hash_init(&sorted_ht, num_elements, NULL, NULL, 1);
			zend_hash_copy(&sorted_ht, ht, NULL);
			zend_hash_sort(&sorted_ht, php_bencode_key_compare_string, 0);
			ht = &sorted_ht;
		}
		
		ZEND_HASH_FOREACH_KEY_VAL_IND(ht, index, key, data) {
			ZVAL_DEREF(data);
			
			if (mode == PHP_BENCODE_TYPE_DICTIONARY) {
				if (key) {
					if (ZSTR_VAL(key)[0] == '\0' && Z_TYPE_P(val) == IS_OBJECT) {
						continue;
					}
					smart_str_append_long(buf, ZSTR_LEN(key));
					smart_str_appendc(buf, ':');
					smart_str_appendl(buf, ZSTR_VAL(key), ZSTR_LEN(key));
				} else {
					char key_buf[MAX_LENGTH_OF_LONG + 1];
					char *res = zend_print_long_to_buf(key_buf + sizeof(key_buf) - 1, index);
					smart_str_append_long(buf, key_buf + sizeof(key_buf) - 1 - res);
					smart_str_appendc(buf, ':');
					smart_str_appendl(buf, res, key_buf + sizeof(key_buf) - 1 - res);
				}
			}
			php_bencode_encode(buf, data);
			
		} ZEND_HASH_FOREACH_END();
		
		if (mode == PHP_BENCODE_TYPE_DICTIONARY) {
			zend_hash_destroy(&sorted_ht);
		}
	}
	smart_str_appendc(buf, PHP_BENCODE_END_STRUCTURE);
}
/* }}} */

PHP_CG_BCODE_API void php_bencode_encode(smart_str *buf, zval *val) /* {{{ */
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
			smart_str_appendc(buf, PHP_BENCODE_TYPE_INTEGER);
			smart_str_append_long(buf, Z_LVAL_P(val));
			smart_str_appendc(buf, PHP_BENCODE_END_STRUCTURE);
			break;
		case IS_DOUBLE:
			smart_str_appendc(buf, PHP_BENCODE_TYPE_INTEGER);
			if(php_bencode_is_valid_double(Z_DVAL_P(val))){
				smart_str_append_long(buf, zval_get_long(val));
			}else{
				zend_error(E_WARNING, "cg_bcode: Cannot convert infinite or NaN to bencode, encoded as 0");
				smart_str_appendc(buf, '0');
			}
			smart_str_appendc(buf, PHP_BENCODE_END_STRUCTURE);
			break;
		case IS_STRING:
			smart_str_append_long(buf, Z_STRLEN_P(val));
            smart_str_appendc(buf, ':');
            smart_str_appendl(buf, Z_STRVAL_P(val), Z_STRLEN_P(val));
			break;
		case IS_OBJECT:
		case IS_ARRAY:
			php_bencode_encode_array(buf, val);
			break;
		case IS_REFERENCE:
			val = Z_REFVAL_P(val);
			goto again;
		default:
			zend_error(E_WARNING, "cg_bcode: Unsuppported variable type.");
			break;
	}
}
/* }}} */


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
