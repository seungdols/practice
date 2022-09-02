<template>
    <div>
        <div class="container" v-bind:class="{ 'active': isActive, 'text-red': hasError }">
            Class Binding
        </div>
        <div class="container" v-bind:class="[activeClass, errorClass]">
            Class Binding
        </div>
        <input type="text" v-model="valueModel" />
        <br />
        <textarea v-model="valueModel"></textarea>
        <br />
        <select v-model="travel">
            <option value="01">서울</option>
            <option value="02">제주</option>
            <option value="03">중국</option>
        </select>
        <br />
        <label><input type="checkbox" value="서울" v-model="checked" />서울</label>
        <label><input type="checkbox" value="제주" v-model="checked" />제주</label>
        <label><input type="checkbox" value="중국" v-model="checked" />중국</label>
        <br />
        <span>체크한 지역: {{ checked }}</span>
        <br />
        <label><input type="radio" v-bind:value="radioValue1" v-model="picked" />서울</label>
        <label><input type="radio" v-bind:value="radioValue2" v-model="picked" />제주</label>
        <label><input type="radio" v-bind:value="radioValue3" v-model="picked" />중국</label>
        <br />
        <span>체크한 지역: {{ picked }}</span>

        <br />
        <div>
            <img :src="imgSrc" />
        </div>
        <input type="text" v-model="textValue" />
        <button type="button" :disabled="textValue == ''">Click</button>
        <div :style="styleObject">inline style binding</div>
        <div>
            <h1 v-if="type=='A'">A</h1>
            <h1 v-else-if="type == 'B'">B</h1>
            <h1 v-else>C</h1>
        </div>
        <div>
            <h1 v-show="isShow">v-show</h1>
        </div>
        <div>
            <button type="button" @click="increaseCounter">Add counter</button>
            <p>The counter is: {{counter}}</p>
        </div>
        <button type="button" @click="one(), two()">Click</button>
        <select v-model="selectedValue" @change="changeSelect">
            <option value="서울">서울</option>
            <option value="부산">부산</option>
            <option value="제주">제주</option>
        </select>
        <br />
        <input @keyup.enter="submit" />
        <input @keyup.alt.enter="clear" />
        <input @click.ctrl="ctrlClick" />
    </div>
</template>
<script>
export default {
    data() {
        return {
            valueModel: 'South Korea',
            travel: "02",
            checked: [],
            picked: '',
            radioValue1: '서울',
            radioValue2: '제주',
            radioValue3: '중국',
            imgSrc: 'https://vuejs.org/images/logo.png',
            textValue: '',
            isActive: true,
            hasError: false,
            activeClass: 'active',
            errorClass: 'text-red',
            styleObject: {
                color: 'red',
                fontSize: '13px'
            },
            type: 'B',
            isShow: true,
            counter: 0,
            selectedValue: '서울'
        }
    },
    methods: {
        increaseCounter() {
            this.counter += 1
        },
        one() {
            alert('One')
        },
        two() {
            alert('Two')
        },
        changeSelect() {
            alert(this.selectedValue)
        }
    }
}
</script>
<style scoped>
.container {
    width: 100%;
    height: 100%;
}

.active {
    background-color: yellow;
    font-weight: bold;
}

.text-red {
    color: red;
}
</style>