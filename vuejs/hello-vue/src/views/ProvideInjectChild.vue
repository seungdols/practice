<script>
export default {
    inject: ['item'],
    mounted() {
        console.log(this.item)
        console.log(this.item[0])
    }
}
</script>