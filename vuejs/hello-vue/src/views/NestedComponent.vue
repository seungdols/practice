<template>
    <div>
        <PageTitle :title="title" />
    </div>
</template>
<script>
import PageTitle from '../components/PageTitle'
export default {
    data() {
        return {
            title: "Nested Component!!"
        }
    },
    components: { PageTitle }
}
</script>