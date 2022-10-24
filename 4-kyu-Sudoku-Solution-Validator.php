function valid_solution(array $a): bool {
  $v = function($a){
    $n = count($a);
    foreach($a as $b => $c)
      if (count(array_flip($c)) != $n || count(array_flip(array_column($a, $b))) != $n)
        return false ;
    return true ;
  } ;
  if ($v($a)){
    for($i = 0; $i < 9; $i += 3){
      for($j = 0; $j < 9; $j += 3){
        $g = [ array_slice($a[$i], $j, 3),  array_slice($a[$i + 1], $j, 3),  array_slice($a[$i + 2], $j, 3),  ];
        $h = [ ... $g[0], ...$g[1], ...$g[2], ] ;
        if (count(array_flip($h)) != 9)
          return false ;
        if ($v($g) == false)
          return false ;
      }
    }
    return true ;
  }
  return false ;
}
