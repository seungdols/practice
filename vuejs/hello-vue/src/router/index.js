import { createRouter, createWebHistory } from 'vue-router'
import HomeView from '../views/HomeView.vue'

const routes = [
  {
    path: '/',
    name: 'home',
    component: HomeView
  },
  {
    path: '/about',
    name: 'about',
    // route level code-splitting
    // this generates a separate chunk (about.[hash].js) for this route
    // which is lazy-loaded when the route is visited.
    component: () => import(/* webpackChunkName: "about" */ '../views/AboutView.vue')
    
  },
  { 
    path: '/databinding',
    name: 'databinding',
    component: () => import(/* webpackChunkName: "databinding" */ '../components/DataBinding.vue')
  },
  { 
    path: '/databindingInput',
    name: 'databindingInput',
    component: () => import(/* webpackChunkName: "databindingInput" */ '../components/DataBindingInputText.vue')
  },
  { 
    path: '/computed',
    name: 'computed',
    component: () => import(/* webpackChunkName: "computed" */ '../components/ComputedSample.vue')
  },
  { 
    path: '/watch',
    name: 'watch',
    component: () => import(/* webpackChunkName: "watch" */ '../components/WatchSample.vue')
  },
  { 
    path: '/dataBindingList',
    name: 'dataBindingList',
    component: () => import(/* webpackChunkName: "dataBindingList" */ '../components/DataBindingList.vue')
  },
  { 
    path: '/nested',
    name: 'nested',
    component: () => import(/* webpackChunkName: "nested" */ '../views/NestedComponent.vue')
  }
]

const router = createRouter({
  history: createWebHistory(process.env.BASE_URL),
  routes
})

export default router
