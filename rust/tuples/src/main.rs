fn main() {
  // let tup1 = (20, "Rust", 30, 35, false, 3.5, (1,4,7));
  // println!("{}", tup1.5);
  let tup1 = (20, "Rust", 30);
  // let tup1 = (20, "Rust", 30, 45); // 아래 구문에서 에러 발생함. 
  let (a, b, c) = tup1;
  println!("a: {}", a);
  println!("b: {}", b);
  println!("c: {}", c);
}
