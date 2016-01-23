--TEST--
Check for structured types
--SKIPIF--
<?php if (!extension_loaded("cg_bcode")) print "skip"; ?>
--FILE--
<?php
var_dump(bencode([]));
var_dump(bencode([1, 2, 3]));
var_dump(bencode(['x', 'y', '9']));
var_dump(bencode(['y', 5, -3]));
var_dump(bencode(['a' => 'x1', 'b' => 'y2', 'c' => 3]));
var_dump(bencode(['0' => 'foo', 5 => 'ze']));
var_dump(bencode(['a' => 'foo', '' => 'ze']));
var_dump(bencode([[]]));
var_dump(bencode([[],['b' => 9, 'c' => 'z']]));
var_dump(bencode(['a' => ['f' => 'z'], 'b' => [1, 3, 2]]));
var_dump(bencode(new stdClass()));

echo "Done\n";
?>
--EXPECT--
string(2) "le"
string(11) "li1ei2ei3ee"
string(11) "l1:x1:y1:9e"
string(12) "l1:yi5ei-3ee"
string(22) "d1:a2:x11:b2:y21:ci3ee"
string(17) "d1:03:foo1:52:zee"
string(16) "d0:2:ze1:a3:fooe"
string(4) "llee"
string(18) "lled1:bi9e1:c1:zee"
string(27) "d1:ad1:f1:ze1:bli1ei3ei2eee"
string(2) "de"
Done
