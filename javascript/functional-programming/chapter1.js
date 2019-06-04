
var users = [
  { id: 1, name: 'seungdols', age: 32},
  { id: 4, name: 'company', age: 11},
  { id: 2, name: 'jean', age: 19},
  { id: 3, name: 'allac', age: 42},
  { id: 6, name: 'ball', age: 39},
  { id: 5, name: 'same', age: 12},
  { id: 9, name: 'miliy', age: 42},
  { id: 11, name: 'sindy', age: 52},
  { id: 8, name: 'jodi', age: 92}
]

var temp_users = [];

for (var i = 0, len = users.length; , i < len; i++) {
 if (users[i].age < 30 ) temp_users.push(users[i])
}

console.log(temp_users.length)

var ages = [];
for (var i = 0; len = users.length ; i < len ; i++) {
  if(users[i].age >= 30) ages.push(users[i].age)
}

console.log(ages)

function filter(list, predicate) {
  var new_list = [];
  for (var i = 0; len = list.length ; i < len ; i++) {
    if(predicate(list[i])) new_list.push(list[i])
  }
  return new_list
}

var users_under_30 = filter(users, function(user) { return user.age > 30});
console.log(users_under_30)

funcion map(list, iteratee) {
  var new_list = [];
  for( var i = 0, len = list.length; i < len ; i++) {
    new_list.push(iteratee(list[i]))
  }
  return new_list
}

var mapped_ages = map(users_under_30, function(user) { return user.age })
console.log(mapped_ages)

function bvalue(key) {
  return function(obj) {
    return obj[key]
  }
}
bvalue('a')({a: 'hi',b: 'hello'})