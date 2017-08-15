// http://tryhelloworld.co.kr/challenge_codes/23
public class Caesar {
    String caesar(String s, int n) {
        String result = "";
        // 함수를 완성하세요.

        char[] c = s.toCharArray();

        for (char ch : c) {

            if (ch == ' ') {
                result += ' ';
            } else {

                if (ch >= 'A' && ch <= 'Z') {
                    ch -= 'A';

                    ch = (char)(((ch+n)%26) + 'A');

                }
                if (ch >= 'a' && ch <= 'z') {
                    ch -= 'a';

                    ch = (char)(((ch+n)%26) + 'a');

                }
                result += ch;
            }
        }

        return result;
    }

    public static void main(String[] args) {
        Caesar c = new Caesar();
        System.out.println("s는 'a B z', n은 4인 경우: " + c.caesar("WyHZtoJHpcmSu D bvG eQsHbszxjLnX dCnAUxnokgplZaFN", 47));
    }
}
