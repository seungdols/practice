<template>
   <child-component @send-message="sendMessage" ref="child_component" />
</template>

<script>
    import ChildComponent from './ChildComponent'
    export default {
        components: { ChildComponent },
        mounted() {
            this.$refs.child_component.$refs.btn.click()
            this.$refs.child_component.callFromParent()
        },
        methods: {
            sendMessage(data) {
                console.log(data)
            }
        }
    }
</script>