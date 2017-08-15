package com.tistory.seungdols.algorithm;

/**
 * @PROJECT JavaProject
 * @PACKAGE_NAME com.tistory.seungdols.algorithm
 * @NAME seungdols
 * @DATE 16. 6. 1
 * @DISCRIPT http://tryhelloworld.co.kr/challenge_codes/141
 * @HISTORY
 */
public class ProductMatrix {
    public int[][] productMatrix(int[][] A, int[][] B) {
        int len = A.length;
        int[][] answer = new int[len][len];


        for (int i = 0; i < len; i++)
        {

            for(int j = 0 ; j < len; j++)
            {
                int sum = 0;
                for(int k = 0; k < len; k++) {
                    System.out.println(A[i][k] + "*" + B[k][j]);
                    sum += A[i][k] * B[k][j];

                }
                answer[i][j] = sum;
                System.out.println("idx" + i + "," + j);
                System.out.println();
            }

        }
        for(int i = 0; i < len; i++)
        {
            for(int j = 0; j < len; j++)
            {
                System.out.print(answer[i][j]);
            }
            System.out.println();
        }
        return answer;
    }

    public static void main(String[] args) {
        ProductMatrix c = new ProductMatrix();
        int[][] A = {{1, 2}, {3, 4}};
        int[][] B = {{5, 6}, {7, 8}};
        c.productMatrix(A,B);
    }

}
