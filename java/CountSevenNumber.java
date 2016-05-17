/****
Count 1 ~ 10000 range in '7' number
****/
public class Main {
    public static void main (String[] args) {

        int count = 0;
        for (int i = 1; i < 10000; i++) {
            for (int j = i; j > 0; j /= 10) {
                if (j % 10 == 7) count++;
            }
        }
        System.out.println (count);
    }
}
