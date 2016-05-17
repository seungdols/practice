/**

https://www.acmicpc.net/problem/11720
**/


import java.math.BigInteger;
import java.util.Scanner;

public class Main {

    public static void main(String[] args) {

        Scanner scanner = new Scanner(System.in);

        int N;
        N = scanner.nextInt();
        scanner.nextLine();

        if(N == 1) {
            String number = scanner.nextLine();
            System.out.println(number);
        }
        else{
            String number = scanner.nextLine();
            BigInteger position = new BigInteger("10");
            BigInteger temp = new BigInteger(number);

            int sum = 0;
            for(int iter = 0; iter < N-1 ; iter++)
            {
                sum += Integer.valueOf((temp.remainder(position).toString()));
                    temp = temp.divide(position);
            }
            System.out.println(sum+Integer.valueOf(temp.toString()));
        }
    }
}
