func () : i32 {

  !i32 mutval = 1;
  mutval = 2;

  i8 imutval = 2;
  imutval = 3; // compile error
  
  bool imutdec;
  imutdec = false;
}
