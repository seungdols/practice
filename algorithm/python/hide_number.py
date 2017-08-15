# http://tryhelloworld.co.kr/challenge_codes/133
def hide_numbers(s):
    input_len=len(s)-4
    ast="*"
    return ast*input_len + s[-4:]

print ("결과 : " + hide_numbers('01033339514'));
