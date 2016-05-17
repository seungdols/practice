package com.tistory.seungdols;

/**
 * @PROJECT EucAlgo
 * @PACKAGE com.tistory.seungdols
 * @WRITTER Administrator
 * @DATE 2015-10-30
 * @HISTORY
 * @DISCRIPT
 */
public class EucAlgo {
    private int _leftVal;
    private int _rightVal;
    private int _remainVal;

    public EucAlgo (int leftVal, int rightVal) {
        this._leftVal = leftVal;
        this._rightVal = rightVal;
        _remainVal = 0;
    }

    /**
     * 유클리드 호제법을 이용하여 입력 된 두 수의 최대공약수를 구한다.
     */
    public void euclideanAlgorithm () {

        int left = _leftVal;
        int right = _rightVal;


        while (true) {
            if (left % right == 0) {
                System.out.println ("gcd : " + right);
                break;
            } else {
                _remainVal = left % right;
                left = right;
                right = _remainVal;
            }
        }
    }

    public void expandEuclideanAlgorithm () {
        int q = 0;
        int r = 0;
        int r1 = _leftVal;
        int r2 = _rightVal;
        int s1 = 1, s2 = 0, s = 0;
        int t1 = 0, t2 = 1, t = 0;

        while (r2 > 0) {

            q = r1 / r2;
            System.out.println ("q : \t"+ q + "\tr1 : \t" + r1 + "\t r2 : \t" + r2 + "\tr : \t"+ r+ "\ts1 : \t" + s1
                    + "\ts2 : \t" + s2 +"\ts : \t"+ s+ "\tt1: \t" + t1 + "\tt2 : \t" + t2 + "\tt : \t" + t );
            r = r1 - q * r2;
            r1 = r2;
            r2 = r;
//          gdc end
            s = s1 - q * s2;
            s1 = s2;
            s2 = s;

            t = t1 - q * t2;
            t1 = t2;
            t2 = t;


//          Expand val


        }
        System.out.println ("두 수 : " + _leftVal + " , " +_rightVal);
        System.out.println ("reseult gcd : " + r1);
        System.out.println ("s , t : " + s1 + ", "+ t1);

        if(r1 == 1)
        {
            System.out.println ("am + bn = gcd : " + (s1 *_leftVal)+ "+" + (t1 * _rightVal) + " = " + r1);
            System.out.println ("모듈러 연산의 곱의 역원 : " + t1);
        }
    }

/*    public void eucExe()
    {
        int result = eucRecursion (_leftVal,_rightVal);
        System.out.println ("result: " + result);
    }
    public int eucRecursion(int a, int b){
        if(a % b == 0){
            return b;
        }
        else{
            return eucRecursion (b, a%b);
        }
    }*/
}
