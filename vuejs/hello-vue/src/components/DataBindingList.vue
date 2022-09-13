<template>
    <div>
        <table>
            <thead>
                <tr>
                    <th>제품명</th>
                    <th>가격</th>
                    <th>카테고리</th>
                    <th>배송료</th>
                </tr>
            </thead>
            <tbody>
                <tr :key="i" v-for="(product, i) in productList">
                    <td>{{product.product_name}}</td>    
                    <td>{{product.price}}</td>    
                    <td>{{product.category}}</td>    
                    <td>{{product.delivery_price}}</td>    
                </tr>
            </tbody>
        </table>
    </div>
</template>

<script>
export default {
    data() {
        return {
            productList: []
        }
    },
    created() {
        this.getList();
    },
    methods: {
        async getList() {
            const data = await this.$api("https://acb4f899-80e2-4554-8b2a-434a3818e01a.mock.pstmn.io/list", "get")
            this.productList = data
            console.log(this.productList)
            console.log(data.length)
            console.log(this.productList.length)
        }
    }
}

</script>
<style scoped>
    table {
        font-family: arial, sans-serif;
        border-collapse: collapse;
        width: 100%;
    }
    td, th {
        border: 1px solid #dddddd;
        text-align: left;
        padding: 8px;
    }
</style>