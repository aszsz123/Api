#### Api
Linux C App Api 

# 내용
----------------
* MsgQueue : 더블 링크드 리스트를 사용한 MsgQueue Api
  - 다중 스레드 프로그램 구성시 스레드간 통신을 위해 구현
  - 사용법은 main.c 참조
  
* Sqlite : Sqlite3 라이브러리 사용한 간단한 관리 Api
  - Sqlite를 사용하여 설정, 상태, 로그 값 등을 관리 할수 있도록 구현
  - sqlite3 라이브러리는 제공하지 않습니다.
  - 사용법은 main.c 참조
  
* Menu : Cli 환경 사용자 메뉴 기능 Api
  - Cli 환경에서 사용 가능한 메뉴 기능 구현
  - 사용법은 menu.c 참조
  
* Client : Tcp Client Rx, Tx Api
  - Tcp Client Rx, Tx Thread Module Api
  - Ipv6, Ipv4 Socket 사용
  - MsgQueue, Menu Api 사용
