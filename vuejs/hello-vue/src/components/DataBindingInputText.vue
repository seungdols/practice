<template>
    <div>
        <input type="text" v-model="valueModel" />
        <br />
        <textarea v-model="valueModel"></textarea>
    </div>
</template>
<script>
    export default {
        data() {
            return {
                valueModel: 'South Korea'
            }
        }
    }
</script>