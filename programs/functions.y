func foo () : i32 {
  return 69;
}

func bar (i32 val1, i32 val2) : i32 {
  return val1 + val2;
}

func () : i32 {
  return foo();
}
