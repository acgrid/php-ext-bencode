--TEST--
Check for non-string key in a dict
--SKIPIF--
<?php if (!extension_loaded("cg_bcode")) print "skip"; ?>
--FILE--
<?php

bdecode('di0e0:e');
?>
--EXPECTF--

Warning: Invalid bencoded-string, expected semicolon, stop at position 7. in %s on line %d