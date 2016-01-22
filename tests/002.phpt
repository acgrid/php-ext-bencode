--TEST--
Check for encoding primitive types
--SKIPIF--
<?php if (!extension_loaded("cg_bcode")) print "skip"; ?>
--FILE--
<?php
var_dump(bencode(null));
var_dump(bencode(true));
var_dump(bencode(false));
var_dump(bencode(0));
var_dump(bencode(65535));
var_dump(bencode(-65535));
var_dump(bencode(1.0));
var_dump(bencode(""));
var_dump(bencode("123"));
var_dump(bencode("a\nb"));
var_dump(bencode("あ"));

echo "Done\n";
?>
--EXPECT--
string(0) ""
string(3) "i1e"
string(3) "i0e"
string(3) "i0e"
string(7) "i65535e"
string(8) "i-65535e"
string(3) "i1e"
string(2) "0:"
string(5) "3:123"
string(5) "3:a
b"
string(5) "3:あ"
Done
