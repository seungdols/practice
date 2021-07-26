fn main() {
  // here
  let x = 10;
  {
    let y = 5;
    // isolated
    println!("x: {}, y: {}", x, y);
  }
}
