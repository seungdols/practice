/***
https://www.acmicpc.net/problem/11441

***/

import java.util.Scanner;
public class Main {

    public static void main(String[] args) {

        Scanner scanner = new Scanner(System.in);
        int n;
        n = scanner.nextInt();
        int[] sum = new int[n];
        for(int i = 0; i < n; i++)
        {
            if(i == 0 ){
                sum[i] = scanner.nextInt();
            }
            else{
                sum[i] = sum[i-1] + scanner.nextInt();
            }
        }
        int m = scanner.nextInt();
        int a , b;
        for(int i = 0; i < m; i++)
        {
            //TODO 3가지 케이스 분기
            a = scanner.nextInt()-1;
            b = scanner.nextInt()-1;
            if(a == 0){
                System.out.println(sum[b]);
            }
            else if(a >= 1)
            {
                System.out.println(sum[b] - sum[a-1]);
            }
            else if(a==b){
                System.out.println(sum[b]);
            }

        }
    }


}
