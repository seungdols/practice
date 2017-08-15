// http://tryhelloworld.co.kr/challenge_codes/130
public class HarshadNumber{
	public boolean isHarshad(int num){

		int sum = 0,number = num;

    while(number > 0)
    {
      sum = sum + number%10;
      number/=10;
    }
    if(num % sum == 0)
			return true;
    else
      return false;
	}

       // 아래는 테스트로 출력해 보기 위한 코드입니다.
	public static void  main(String[] args){
		HarshadNumber sn = new HarshadNumber();
		System.out.println(sn.isHarshad(18));
	}
}
