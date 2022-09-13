<template>
  <ProvideInjectChild />
</template>
<script>
    import ProvideInjectChild from './ProvideInjectChild'
    export default {
        components: {
            ProvideInjectChild
        },
        data() {
            return {
                items: [
                    'A',
                    'B'
                ]
            }
        },
        provide() {
            return {
                item: this.items
            }
        }
    }
</script>
    