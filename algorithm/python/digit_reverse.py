# http://tryhelloworld.co.kr/challenge_codes/117
def digit_reverse(n):
    # 함수를 완성해 주세요
    numbers = map(int, str(n))
    num_list = list(numbers)
    num_list.reverse()
    return num_list

# 아래는 테스트로 출력해 보기 위한 코드입니다.
print("결과 : {}".format(digit_reverse(12345)));
