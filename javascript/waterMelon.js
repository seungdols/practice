// http://tryhelloworld.co.kr/challenge_codes/107
function waterMelon(n){
  var result = "";
  //함수를 완성하세요
	var str = "수박";
  var num = n;
  while( num > 0 ){
    result += str;
    num-=2;
  }
  if(n%2 != 0)
    result = result.substring(0, result.length-1);
  return result;
}

// 실행을 위한 테스트코드입니다.
console.log("n이 3인 경우: "+ waterMelon(3))
console.log("n이 4인 경우: "+ waterMelon(4))
