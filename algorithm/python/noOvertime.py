# http://tryhelloworld.co.kr/challenge_codes/26
def noOvertime(n, works):
    result = 0
    # 야근 지수를 최소화 하였을 때의 야근 지수는 몇일까

    while(n != 0):
        works.sort(reverse=True)
        works[0] -= 1
        n-=1

    for i in works:
        result += i * i
    return result
