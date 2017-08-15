# http://tryhelloworld.co.kr/challenge_codes/23
# seungdols code
def caesar(s, n):
    result = ""
    alp_list = list(s)

    for alpabet in alp_list:
        if ( alpabet == ' ' ):
            result += ' '

        else:
            temp = ord(alpabet)
            if( 65 <= temp and temp <= 90 ):
                    temp = (((temp - 65) + n)%26) + 65
            if( 97 <= temp and temp <= 122):
                    temp = (((temp - 97) + n)%26) + 97

            result += chr(temp)
    return result
    # 주어진 문장을 암호화하여 반환하세요.

# 실행을 위한 테스트코드입니다.
print('s는 "a B z", n은 4인 경우: ' + caesar("bFre JiAyRhfO lhwXkiZrjTy cdEZbUn f Tf EmNUTrVA ", 1))
