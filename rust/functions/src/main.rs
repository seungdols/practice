fn main() {
  print_numbers_to(10);

  if is_even(30) {
    println!("It is even!")
  }
}

fn print_numbers_to(number: u32) {
  for n in 1..number {
    if is_even(n) {
      println!("{} is even", n);
    } else {
      println!("{} is odd", n);
    }
  }
}

fn is_even(num: u32) -> bool {
  return num % 2 == 0;
}