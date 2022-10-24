function same_structure_as(array $a, array $b): bool {
  $f = function(&$a){ is_array($a) || ($a = null) ; };
  array_walk_recursive($a, $f);
  array_walk_recursive($b, $f);
  return $a === $b ;
}
