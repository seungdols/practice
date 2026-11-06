# How Tomcat Works — 한국어 번역본

## 파일 안내

- 원본: `How Tomcat Works.pdf` (476페이지)
- 구성: 서문(00), 1~20장(01~20) 각각의 Markdown 파일
- 원문 Java/XML/셸 예제 코드는 가능한 한 원문을 유지하고, 설명 문장을 한국어로 옮겼습니다.
- 그림 및 도식 중 Markdown에 삽입되지 않은 것은 원본 PDF의 해당 페이지를 참고해야 합니다.

## 검수 상태

- **페이지 범위 작업:** 원본 PDF 마지막 476페이지까지 번역 자료가 수록되어 있습니다.
- **구조 검수:** Markdown 파일 구성, 코드 펜스의 짝, ZIP 무결성을 확인했습니다.
- **부분 내용 검수:** 1장과 3장의 과거 '부분 번역' 안내가 최종 내용과 불일치하는 것을 확인하고 정정했습니다. 20장의 일부 원문 코드/URL 오탈자 의심 항목을 원본 PDF 추출 텍스트와 대조했습니다.
- **미완료:** 원문 전체 476페이지에 대한 문단별 완전성 검증, 모든 코드의 문자·행 단위 대조, 그림 전수 대조, 전체 번역의 언어 교정.

**주의:** 현재 패키지는 '전수 정밀 교정 완료본'이 아니라 '구조 및 표본 검수 반영본'입니다. 실행용 코드로 사용할 때 원본과 대조하세요.

- **3차 검수:** 1장 원본 PDF 7~23페이지와 번역본의 주요 절, 목록 1.1~1.6, 그림 참조 및 요약의 존재 여부를 대조했습니다. 문장별·코드 문자별 전수 대조는 미완료입니다. 자세한 내용은 `QA_ch01.md`를 참고하세요.

## 장별 파일

- [00_서문](00_서문.md)
- [01_간단한_웹_서버](01_간단한_웹_서버.md)
- [02_간단한_서블릿_컨테이너](02_간단한_서블릿_컨테이너.md)
- [03_커넥터](03_커넥터.md)
- [04_Tomcat_기본_커넥터](04_Tomcat_기본_커넥터.md)
- [05_컨테이너](05_컨테이너.md)
- [06_생명주기](06_생명주기.md)
- [07_로거](07_로거.md)
- [08_로더](08_로더.md)
- [09_세션_관리](09_세션_관리.md)
- [10_보안](10_보안.md)
- [11_StandardWrapper](11_StandardWrapper.md)
- [12_StandardContext](12_StandardContext.md)
- [13_Host_and_Engine](13_Host_and_Engine.md)
- [14_Server_and_Service](14_Server_and_Service.md)
- [15_Digester](15_Digester.md)
- [16_Shutdown_Hook](16_Shutdown_Hook.md)
- [17_Tomcat_Startup](17_Tomcat_Startup.md)
- [18_Deployer](18_Deployer.md)
- [19_Manager_Servlet](19_Manager_Servlet.md)
- [20_JMX_Based_Management](20_JMX_Based_Management.md)


## 세부 검수 추가 기록
- 1장 PDF 7~12페이지를 원문 대조하여 참고 URL 누락 2건을 복원했습니다. 상세 내용은 `QA_ch01_detailed_part1.md`를 참고하세요. 1장 전체 문장별 검수는 미완료입니다.
