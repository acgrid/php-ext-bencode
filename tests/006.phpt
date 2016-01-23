--TEST--
Check for non-string key in a dict
--SKIPIF--
<?php if (!extension_loaded("cg_bcode")) print "skip"; ?>
--FILE--
<?php

var_dump(bdecode('di0e0:e'));

echo "Done\n";
?>
--EXPECTF--

Warning: Invaild bencoded-string, expected semicolon, stop at position 7. in %s on line %d
array(0) {
}
Done
