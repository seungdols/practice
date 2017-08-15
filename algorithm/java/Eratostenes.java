package com.tistory.seungdols;

import java.util.ArrayList;
import java.util.List;

/**
 * @PROJECT EucAlgo
 * @PACKAGE com.tistory.seungdols
 * @WRITTER Administrator
 * @DATE 2015-10-30
 * @HISTORY
 * @DISCRIPT
 */
public class Eratos {
    /**
     * 소수 구하는 에라토스테네스의 체 알고리즘 구현
     */
    private int N;

    private List primeList;

    public Eratos () {
        N = 120;
        primeList = new ArrayList<> ();
    }

    public Eratos (int n) {
        N = n;
        primeList = new ArrayList<> ();

    }

    public void setListNum()
    {

        for(int i = 2; i < N+1; i++)
        {
            primeList.add (i);
        }
    }


    public void eratostenesPrime()
    {
        setListNum ();
        int size = 0;
        int get = 0;
        for(int i = 2; i < 8; i++)
        {

            System.out.println (i + "==========================");

            if ( i == 4) continue;
            if ( i == 6) continue;
            size = primeList.size ();
            for(int idx = 0; idx < size; idx++)
            {
                get = (int) primeList.get (idx);
                if(get % i == 0)
                {

                    if(get != i)
                        primeList.set (idx , 0);

                }
            }

        }

        showList ();
    }
    public void showList()
    {
        int size = primeList.size ();
        for(int i = 0; i < size; i++)
        {
            if((int)primeList.get (i) !=0)
                System.out.println (primeList.get (i));
        }
    }

    public static void main (String[] args) {
        Eratos eratos = new Eratos ();
        eratos.eratostenesPrime ();
    }
}
