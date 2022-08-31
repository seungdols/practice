<template>
  <div id="hello">
    <h1>{{ title }}</h1>

    <ol>
      <li v-for="todo in todos" v-bind:key="todo.text">
        {{ todo.text }}
      </li>
    </ol>
  </div>
</template>

<script>
export default {

  data() {
    return {
      title: 'Hello World',
      todos: [
        { text: 'Learn VueJS' },
        { text: 'Learn Kotlin' },
        { text: 'Learn Go' },
        { text: 'Learn Kafka' },
        { text: 'Learn Rust' },
        { text: 'Learn Python' },
      ]
    }
  }

}
</script>
