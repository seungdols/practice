enum Direction {
  Up,
  Down,
  Left,
  Right
}


fn main() {
  let player_direction: Direction = Direction::Up;

  match player_direction {
    Direction::Up => println!("We are heading up!"),
    Direction::Down => println!("We are going all the way down..."),
    Direction::Left => println!("Left is right!"),
    Direction::Right => println!("Moving towards the right!"),
  }
}
