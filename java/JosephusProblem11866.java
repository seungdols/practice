/*****
https://www.acmicpc.net/problem/11866

****/


import java.util.Scanner;

/**
 * @PROJECT JavaProject
 * @PACKAGE_NAME com.tistory.seungdols.beakjoonjudge
 * @NAME seungdols
 * @DATE 16. 2. 6
 * @DISCRIPT
 * @HISTORY
 */
public class Main {

    public static void main(String[] args) {
        //조세퍼스 문제 0 - 11866
        Scanner scanner = new Scanner(System.in);

        int pos = 0, check = 0;
        int M, N;
        int idx = 0;

        N = scanner.nextInt();
        M = scanner.nextInt();
        int[] arr = new int[N];
        int[] ans = new int[N];
        int ans_idx = 0;
        for (int i = 0; i < N; i++) {
            arr[i] = i + 1;
        }
        while (true) {
//            System.out.printf("while");
            if (arr[idx] != 0) {
                pos++;
                if (pos == M) {
                    ans[ans_idx++] = arr[idx];
                    arr[idx] = 0;
                    check++;
                    pos = 0;
                }
            }
            if (check == N)
                break;
            idx++;
            if (idx == N)
                idx %= N;
        }
        System.out.printf("<");
        for (int i = 0; i < N-1; i++) {

            System.out.printf("%d, ", ans[i]);

        }
        System.out.printf("%d",ans[N-1]);
        System.out.printf(">");
    }


}
