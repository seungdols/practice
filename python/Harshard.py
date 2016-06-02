# http://tryhelloworld.co.kr/challenge_codes/131
def Harshad(n):
    # n은 하샤드 수 인가요?
    numbers = map(int, str(n))
    number_sum = sum(numbers)
    if(n%number_sum==0):
        return True
    else:
        return False

# 아래는 테스트로 출력해 보기 위한 코드입니다.
print(Harshad(42))
