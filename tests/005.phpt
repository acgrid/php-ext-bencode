--TEST--
Check for string decode
--SKIPIF--
<?php if (!extension_loaded("cg_bcode")) print "skip"; ?>
--FILE--
<?php

var_dump(bdecode(''));
var_dump(bdecode('i1e'));
var_dump(bdecode('i0e'));
var_dump(bdecode('i-1e'));
var_dump(bdecode('i9999999999999999999999999999999999999999999999999999999999999999e'));
var_dump(bdecode('0:'));
var_dump(bdecode('3:xyz'));

echo "Done\n";
?>
--EXPECT--
NULL
int(1)
int(0)
int(-1)
string(64) "9999999999999999999999999999999999999999999999999999999999999999"
string(0) ""
string(3) "xyz"
Done
