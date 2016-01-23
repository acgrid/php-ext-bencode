--TEST--
Check for structured types
--SKIPIF--
<?php if (!extension_loaded("cg_bcode")) print "skip"; ?>
--FILE--
<?php
var_dump(bencode(['' => 'foo', 5 => 'ze']));

echo "Done\n";
?>
--XFAIL--
Unstable sorting on empty string with index key, need further compare with hash index.
--EXPECT--
string(16) "d0:3:foo1:52:zee"
Done
