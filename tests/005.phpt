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
var_dump(bdecode('le'));
var_dump(bdecode('li2e3:123i0ee'));
var_dump(bdecode('lli3ee1:ae'));
var_dump(bdecode('de'));
var_dump(bdecode('d1:a3:1231:bi3e1:cl1:x1:y1:zee'));

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
array(0) {
}
array(3) {
  [0]=>
  int(2)
  [1]=>
  string(3) "123"
  [2]=>
  int(0)
}
array(2) {
  [0]=>
  array(1) {
    [0]=>
    int(3)
  }
  [1]=>
  string(1) "a"
}
array(0) {
}
array(3) {
  ["a"]=>
  string(3) "123"
  ["b"]=>
  int(3)
  ["c"]=>
  array(3) {
    [0]=>
    string(1) "x"
    [1]=>
    string(1) "y"
    [2]=>
    string(1) "z"
  }
}
Done
