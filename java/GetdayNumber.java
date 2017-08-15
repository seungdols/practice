// http://tryhelloworld.co.kr/challenge_codes/176
class GetdayNumber
{
    public String getDayName(int a, int b)
    {
        String answer = "";
        int[] month = {
//               1  2   3  4  5  6  7  8  9  10  11  12
                31,29,31,30,31,30,31,31,30,31,30,31};
        int[] monthSum = new int[12];
        for(int i = 1; i < 12; i++){
            monthSum[i] = monthSum[i-1] + month[i-1];
        }
        int m = a, d = b;
        int x = (monthSum[m-1] + d)%7;
        switch (x){
            case 0:
                answer = "THU";
                break;
            case 1:
                answer = "FRI";
                break;
            case 2:
                answer = "SAT";
                break;
            case 3:
                answer = "SUN";
                break;
            case 4:
                answer = "MON";
                break;
            case 5:
                answer = "TUE";
                break;
            case 6:
                answer = "WED";
                break;
        }

        return answer;
    }
    public static void main(String[] args)
    {
        TryHelloWorld test = new TryHelloWorld();
        int a=1, b=1;
        System.out.println(test.getDayName(a,b));
    }
}
