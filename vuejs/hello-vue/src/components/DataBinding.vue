<template>
    <h1>Hello, {{ title }}</h1>
    <div v-html="htmlString"></div>
</template>

<script>
export default {
    data() {
        return {
            title: "DataBinding",
            htmlString: '<p style="color:red">This is a red string</p>'
        }
    }
}
</script>
