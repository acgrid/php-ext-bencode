--TEST--
Check for non bencode string
--SKIPIF--
<?php if (!extension_loaded("cg_bcode")) print "skip"; ?>
--FILE--
<?php

var_dump(bdecode('xxxxxx'));

echo "Done\n";
?>
--EXPECTF--

Warning: Invaild bencoded-string, expected semicolon, stop at position 6. in %s on line %d
NULL
Done
