// http://tryhelloworld.co.kr/challenge_codes/159
import java.util.ArrayList;
import java.util.Arrays;
import java.util.List;
class LineCombination {
    public int[] setAlign(int n, long k) {
        int[] answer = new int[n];

        List<Integer> arr = new ArrayList<>();

        for(int i = 0; i < n; i++){
            arr.add(i + 1);
//            System.out.println(arr.get(i));
        }



        int num = n;
        long remain;
        int share;
//        System.out.println(fac(5));
        List<Integer> result = new ArrayList<>();
        while(num > 0) {
            --num;
            remain = k % fac(num);
            share = (int)(k / fac(num));
 //           System.out.println("share : " + share);
//            System.out.println("remain : " + remain);
            if (remain == 0) {
//                System.out.println("remain is Zero");

                share--;
                result.add(arr.get(share));
                arr.remove(share);
//                int idx = arr.size() - 1;
////              역순 삽입
//                while (!arr.isEmpty()) {
//                    result.add(arr.get(idx));
//                    arr.remove(idx);
//                    idx--;
//                }
                break;
            }
            result.add(arr.get( share));
            arr.remove( share);

            k = remain;
        }

        for(int i = arr.size()-1; i >= 0; i--){
            result.add(arr.get(i));
            arr.remove(i);
        }

        for(int i = 0; i < result.size(); i++){
//            System.out.print(result.get(i) + ",");
            answer[i] = result.get(i);
        }
        return answer;
    }
    public static int fac(long num){
        int result = 1;

        for(long i = num; i > 0; i--){
            result *= i;
        }

 //       System.out.println("fac : " + result);
        return result;
    }
    // 아래는 테스트로 출력해 보기 위한 코드입i니다.
    public static void main(String[] args) {
        LineCombination lc = new LineCombination();
        System.out.println(Arrays.toString(lc.setAlign(4, 13)));
    }
}
