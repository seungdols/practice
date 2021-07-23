fn main() {

    let numbers = 30..51;

    for i in numbers {
        println!("The number is {}", i);
    }

    let animals = vec!["Rabbit", "Dog", "Cat"];

    for a in animals.iter() {
      println!("The animal name is {}", a);
    }

    for (index, a) in animals.iter().enumerate() {
      println!("The index is {} and animal name is {}", index, a);
    }
}
