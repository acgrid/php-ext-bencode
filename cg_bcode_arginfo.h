/* This is a generated file, edit the .stub.php file instead.
 * Stub hash: 1d82ef5050afde6481b30b0c25b95f34fb4f72fd */

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_bencode, 0, 1, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, input, IS_MIXED, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_bdecode, 0, 1, IS_MIXED, 0)
	ZEND_ARG_TYPE_INFO(0, encoded, IS_STRING, 0)
ZEND_END_ARG_INFO()


ZEND_FUNCTION(bencode);
ZEND_FUNCTION(bdecode);


static const zend_function_entry ext_functions[] = {
	ZEND_FE(bencode, arginfo_bencode)
	ZEND_FE(bdecode, arginfo_bdecode)
	ZEND_FE_END
};
