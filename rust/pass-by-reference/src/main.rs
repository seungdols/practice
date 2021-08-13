struct Color {
  red: u8,
  green: u8,
  blue: u8
}
fn main() {

  let blue = Color { red: 0, green: 0, blue: 255 };

  // print_color(blue);
  // print_color(blue); 여기서 문제가 된다.

  print_color(&blue);
  print_color(&blue);
}

fn print_color(color: &Color) {
  println!("Color: {}, {}, {}", color.red, color.green, color.blue);
}