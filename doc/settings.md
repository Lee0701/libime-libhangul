## 설정 파일
입력기 설정은 %appdata%\libime-libhangul 아래 settings.ini 파일에 저장됩니다. 상세한 설정 내용은 다음과 같습니다.

### [ime]
#### auto_reorder
* 모아주기(초·중·종성 순서 교정) 활성화
* 기본값: 0
* 현재 작동 불능

#### candidate_font_size
* 후보 창 폰트 크기
* 기본값: 14

#### hangul_keyboard_type
* 한글 키보드 종류
* 기본값: 2 (표준 두벌식)
* keyboards.md 참고

#### hanja_conversion_mode
* 한자 변환 모드
* 기본값: auto
  * auto: 실시간 자동 변환
  * manual: 수동 변환 (한자 키)
