fn main() {
  let mut n = 0;

  loop {
    n += 1;

    if n == 7 {
      continue;
    }

    if n > 10 {
      break;
    }

    println!("The value of of n is {}", n);
  }

}
