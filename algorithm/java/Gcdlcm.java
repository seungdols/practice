import java.util.Arrays;
// http://tryhelloworld.co.kr/challenge_codes/10
class Gcdlcm {
    public int[] gcdlcm(int a, int b) {
        int[] answer = new int[2];
				int left = a;
        int right = b;
				int _remainVal = 0;

        while (true) {
            if (left % right == 0) {
  							answer[0] = right;
                break;
            } else {
                _remainVal = left % right;
                left = right;
                right = _remainVal;
            }
        }
  			answer[1] = a*b/answer[0];
        return answer;
    }

    // 아래는 테스트로 출력해 보기 위한 코드입니다.
    public static void main(String[] args) {
        Gcdlcm c = new Gcdlcm();
        System.out.println(Arrays.toString(c.gcdlcm(3, 12)));
    }
}
