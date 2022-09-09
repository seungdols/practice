<template>
    <h1>Full Name: {{fullName}}</h1>
</template>
<script>
    export default {
        name: 'ComputedSample',
        data() {
            return {
                firstName: 'seungho',
                lastName: 'choi'
            }
        },
        computed: {
            fullName() {
                return this.firstName + ' ' + this.lastName
            }
        }
    }
</script>