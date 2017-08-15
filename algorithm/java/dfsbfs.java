/*****

Recursion DFS/stack DFS/queue BFS 구현 https://www.acmicpc.net/problem/1260 문제풀이
******/

import java.util.LinkedList;
import java.util.Queue;
import java.util.Scanner;
import java.util.Stack;

/**
 * @author 승호
 * @FileName
 * @project
 * @Edit_History
 * @Discript
 * @since 2016-04-21
 */
public class Solution {

    public static void main(String[] args) {
        Scanner sc = new Scanner(System.in);
        int n = sc.nextInt();
        int m = sc.nextInt();
        int SIZE = 10;
        int[][] matrix = new int[SIZE][SIZE];
        boolean[] check = new boolean[SIZE];
        int u,v;
        for(int i = 0; i < m; i++)
        {
            u = sc.nextInt();
            v = sc.nextInt();
            matrix[u][v] = matrix[v][u] = 1;//무방향 그래프
        }
//        dfs(matrix,check,1, n);
//        stack_dfs(matrix,check,1, n);
        bfs(matrix,check,1,n);
    }

    static public void dfs(int[][] matrix,boolean[] check,int x, int n){
        check[x] = true;
        System.out.println("vertext checked : " + x);
        for(int i = 1; i <= n; i++)
        {
            if(matrix[x][i] == 1 && check[i] == false){
                dfs(matrix, check, i, n);
            }
        }
    }
    static public void stack_dfs(int[][] matrix, boolean[] check, int x, int n){
        Stack stack = new Stack();
        stack.push(x);
        check[x] = true;
        System.out.println("vertex checked : " + x);
        while(!stack.empty()){
            int v = (Integer)stack.peek();

            for(int i = 1; i <= n; i++){
                if(matrix[v][i] == 1 && check[i] == false){
                    System.out.println("vertext checked : " + i);
                    check[i] = true;
                    stack.push(i);
                    break;
                }
                if(i == n) stack.pop();
            }
        }

    }
    static public void bfs(int [][] matrix, boolean[] check, int x, int n)
    {
        Queue q = new LinkedList();
        check[x] = true;
        q.offer(x);
        while (!q.isEmpty()){
            int v = (Integer)q.peek();
            q.poll();
            System.out.println("vertex checked : " + v);
            for(int i = 1; i <= n; i++)
            {
                if(matrix[v][i] == 1 && check[i] == false)
                {
                    check[i] = true;
                    q.offer(i);
                }
            }
        }
    }
}
