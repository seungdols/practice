import axios from 'axios'

const mixins = {
    methods: {
        async $api(url, method, data) {
            return (await axios({
                method: method,
                url,
                data
            }).catch(e => {
                console.log(e)
            })).data
        }
    }
}

export default mixins