// http://tryhelloworld.co.kr/challenge_codes/159

#include<iostream>
#include<vector>
#include<algorithm>
using namespace std;

vector<int> setAlign(int n, long long k)
{
	vector<int> answer;
	int permutations[n];

  for(int i = 1; i <= n; i++)
  {
    permutations[i-1] = i;
  }
  long long cnt = 0;
  do{

    for(int i = 0; i < n; i++){
    	//printf("%d", permutations[i]);
      if(cnt == k-1){
        answer.push_back(permutations[i]);
      }
    }
    //printf("\n");
    cnt++;
  }
    while(next_permutation(permutations, permutations + n));


	return answer;
}
int main()
{
	int testn = 5;
	long long testcnt = 70;
	vector<int> testAnswer = setAlign(testn,testcnt);
	// 아래는 테스트로 출력해 보기 위한 코드입니다.

	for(int i=0; i< testAnswer.size(); i++)
	{
		cout << testAnswer[i] << " ";
	}
}
