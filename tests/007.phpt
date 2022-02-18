--TEST--
Check for non bencode string
--SKIPIF--
<?php if (!extension_loaded("cg_bcode")) print "skip"; ?>
--FILE--
<?php

bdecode('xxxxxx');
?>
--EXPECTF--

Warning: Invalid bencoded-string, expected semicolon, stop at position 6. in %s on line %d