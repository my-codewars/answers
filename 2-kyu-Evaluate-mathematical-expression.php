function calc(string $s): float {
  $s = preg_replace('#[^0-9.()/*+-]#', '', $s);
  $f = '(?:[+-]?[0-9]+(?:\.[0-9]+)?)' ;
  $o = function($s){ do $s = str_replace(['++', '--', '+-', '-+'], ['+', '+', '-', '-'], $s, $c); while($c); return $s ; };
  $ev = function($a) use(&$ev, $f, $o){
    do $a[1] = preg_replace_callback('#\(([^()]*)\)#', $ev, $a[1], -1, $c); while($c);
    do $a[1] = preg_replace_callback("#($f)([/*])($f)#", function($a){
      return '+' . number_format($a[2] == '*' ? $a[1] * $a[3] : $a[1] / $a[3], 20, '.', '');
    }, $o($a[1]), -1, $c); while($c);
    do $a[1] = preg_replace_callback("#($f)([+-])($f)#", function($a){
      return '+' . number_format($a[2] == '+' ? $a[1] + $a[3] : $a[1] - $a[3], 20, '.', '');
    }, $a[1], -1, $c); while($c);
    do $a[1] = str_replace(['++', '--', '+-', '-+'], ['+', '+', '-', '-'], $a[1], $c); while($c);
    return number_format($o($a[1]), 20, '.', ''); ;
  } ;
  return $ev([ 1 => $s, ]) ;
}
