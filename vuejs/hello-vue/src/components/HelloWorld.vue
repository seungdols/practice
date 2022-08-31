
<template>
  <div id="hello">
    <h1>{{ title }}</h1>

    <ol>
      <TodoItem v-for="todo in todos" v-bind:key="todo.text" v-bind:todo="todo" />
    </ol>
  </div>
</template>

<script>
import TodoItem from './TodoItem.vue'
export default {

  components: {
    TodoItem
  },

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
