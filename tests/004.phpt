--TEST--
Check for dict key sorting
--SKIPIF--
<?php if (!extension_loaded("cg_bcode")) print "skip"; ?>
--FILE--
<?php
$a = new stdClass();
$a->x = 'a';
$a->c = 'o';

var_dump(bencode(['c' => 8, 'b' => 9, 'a' => 9]));
var_dump(bencode($a));

echo "Done\n";
?>
--EXPECT--
string(20) "d1:ai9e1:bi9e1:ci8ee"
string(14) "d1:c1:o1:x1:ae"
Done
