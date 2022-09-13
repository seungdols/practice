<template>
  <nav>
    <router-link to="/">Home</router-link> |
    <router-link to="/about">About</router-link> |
    <router-link to="/databinding">Data Binding</router-link> | 
    <router-link to="/databindingInput">Data BindingInput</router-link> | 
    <router-link to="/computed">Computed</router-link> | 
    <router-link to="/watch">Watch</router-link> | 
    <router-link to="/dataBindingList">dataBindingList</router-link> | 
    <router-link to="/nested">Nested</router-link> | 
  </nav>
  <router-view/>
</template>

<style>
#app {
  font-family: Avenir, Helvetica, Arial, sans-serif;
  -webkit-font-smoothing: antialiased;
  -moz-osx-font-smoothing: grayscale;
  text-align: center;
  color: #2c3e50;
}

nav {
  padding: 30px;
}

nav a {
  font-weight: bold;
  color: #2c3e50;
}

nav a.router-link-exact-active {
  color: #42b983;
}
</style>
