<template>
    <button type="button" @click="childFunc" ref="btn">Click</button>
</template>
<script>
    export default {
        data() {
            return {
                message: 'Send Parent'
            }
        },
        methods: {
            childFunc() {
                console.log("부모 컴포넌트에서 직접 발생시킨 이벤트")
            },
            callFromParent() {
                console.log("부모 컴포넌트에서 직접 호출")
            }
        },
        mounted() {
            this.$emit('send-message', this.message)
        }
    }
</script>