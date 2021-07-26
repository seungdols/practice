fn main() {
  let mut x = 10;
  let xr = &x;

  // println!("X is {}", xr);
  {
    let dom = &mut x;
    *dom += 1;
    println!("X is {}", dom);
  }
  println!("X is {}", x);
}
