#include <stdio.h>
#include <stdlib.h>
#include <termios.h>
#include <unistd.h>
#include <time.h>  // 타이머 함수를 만들기 위한 헤더파일
#include <string.h>

#define SIZE_MAP_X 30 //맵 가로 최대 사이즈
#define SIZE_MAP_Y 30 // 맵 세로 최대 사이즈

int StageNumber=0; //맵 번호.
int player_x, player_y;//플레이어 좌표 전역변수.
char map[5][SIZE_MAP_Y][SIZE_MAP_X]; //맵
char checkO[5][SIZE_MAP_Y][SIZE_MAP_X];//원형 맵(게임 내내 변경되지 않음).
int ClearCount[5] = {0};
int boxCounter[5] = {0};
char Undo_SaveMap[5][SIZE_MAP_Y][SIZE_MAP_X]; //되돌리기를 위한 맵 저장용 배열.
int UndoCount = 0;//언두 횟수
int MoveCount = 0;//이동 횟수
char UserName[11]; //플레이어 이름
int dx=0,dy=0; // 값 전달을 위해 전역변수로 바꿈
int SaveUndo=0;   //save 시 Undo 횟수 저장
int SaveMove=0;   //save 시 Move 횟수 저장
char Names[5][5][11]; //순위 저장용 배열
float Times[5][5]; // 순위 저장용 배열
int TimeCount_Max[5];//타임 랭크 돌릴떄 각 맵별 랭크 갯수.
char ch;


clock_t Map_start, Map_stop, Map_stopEnd, Map_end,Map_display;  // 현 시간을 저장할 변수
float gap=0,Fgap=0;

void DrawMap();
int getch();
void PlayerMove();
void EndOneStage();
void getPlayerXY();
void sameO();
void Undo_SaveMapFunc();
void Undo_LoadMapFunc();
void MapA();
void Read_command();
void Read_rank();
void Arrange_rank();
void Save_rank();
void Option(char);
void SaveFile();
void LoadFile();
char TOption(char);
void time_rank();
void Load_rank();
void Print_Command();


int main(){
   system("clear");
   printf("Start....\n");
   printf("Input name : ");
   scanf("%s", &UserName);
   int tmp=0;
    while(UserName[tmp]!='\0'){ //이름이 영문 최대 10자 인것 처리.
      if(tmp>=10){ //10자 이상인지 검사
        printf("영문 최대 10자 까지만 이름으로 사용 가능합니다.");
        return 0;
      }
      if((UserName[tmp]>='a'&&UserName[tmp]<='z')||(UserName[tmp]>='A'&&UserName[tmp]<='Z')){// 영어인지 검사
      }else{
        printf("영문 최대 10자 까지만 이름으로 사용 가능합니다.");
        return 0;
      }
      tmp++;
    }
   getch();
   MapA();// 맵을 map.txt 에서 읽어  배열에 저장하는 함수 호출
   DrawMap();//가장 처음 맵 그리기.
   getPlayerXY();//플레이어의 위치 전역변수에 저장하는 함수.
   Map_start=clock(); // 게임 시작 시 첫 시간 저장
   while(1){
      PlayerMove(); // 플레이어 움직임 무한반복처리. 입력을 받을때마다 반복이 1회씩 됨.
      EndOneStage(); // 플레이어가 움직일떄마다 스테이지 종료조건 검사.
   }
   return 0;
}


int getch(void){
  int ch;
  struct termios buf;
  struct termios save;

   tcgetattr(0, &save);
   buf = save;
   buf.c_lflag &= ~(ICANON|ECHO);
   buf.c_cc[VMIN] = 1;
   buf.c_cc[VTIME] = 0;
   tcsetattr(0, TCSAFLUSH, &buf);
   ch = getchar();
   tcsetattr(0, TCSAFLUSH, &save);
   return ch;
}

void DrawMap(){ //맵 그리기
  system("clear"); //위의 내용을 터미널에서 지움.
  system("clear"); //2번 호출하면 바로 전 출력물이 남아있지 않고 모두 지워짐.
  printf("    Hello %s\n", UserName);
   for(int i= 0; i< SIZE_MAP_X ; i++){ //해당 맵을 2중 for 문으로 그림.
      for(int j = 0; j < SIZE_MAP_Y; j++){
         printf("%c", map[StageNumber][i][j]);
      }
      printf("\n");
   }
         printf("\n(Command) ");
}

void Read_command(){
    system("clear");
    system("clear");
    printf("    Hello %s\n\n", UserName);
    printf("h(왼쪽), j(아래), k(위), l(오른쪽)\n");
    printf("u(undo)\n");
    printf("r(replay)\n");
    printf("n(new)\n");
    printf("e(exit)\n");
    printf("s(save)\n");
    printf("f(file load)\n");
    printf("d(display help)\n");
    printf("t(top)\n");
    printf("\n\n(Command) d");
    printf("\n\n게임으로 돌아가려면 아무 키나 누르십시오.\n");
    MoveCount-=1;
    if(getch()){
        return;
    }
}

void Read_rank(int num){   // 랭킹 출력 함수
    Load_rank();   // 파일에서 순위를 불러와서 배열에 저장하는 함수
    system("clear");
    system("clear");
    printf("    Hello %s\n\n", UserName);
    if(num==0){
      for(int i=0;i<5;i++){
        if(TimeCount_Max[i]==0) //출력할 순위가 아예 없는 맵의 경우, 출력하지 않음.
          continue;
        printf("map %d\n", i+1);
        for(int j=0;j<TimeCount_Max[i];j++){
          printf("%s  %.1fsec\n", Names[i][j], Times[i][j]);
        }
      }
      printf("\n\n(Command) t");
    }else{
      printf("map %d\n", num);
      for(int i=0;i<TimeCount_Max[num-1];i++){
        printf("%s  %.1fsec\n", Names[num-1][i], Times[num-1][i]);
      }
      printf("\n\n(Command) t%d", num);
    }
    printf("\n게임으로 돌아가려면 아무 키나 누르십시오.");

    MoveCount-=1;

    if(getch()){
      return;
    }
}

void PlayerMove(void){ //플레이어를 움직이는 함수,
   int input1;
   int UndoCheck = 0;
   char ch;

   Uback:
   ch = getch(); // getch 로 키보드로 ch 에 문자하나를 입력받음.
   //플레이어를 이동하는키인 h, j, k, l 이면 플레이어가 이동해야할 만큼의 변위를 dx, dy 에 저장함.
   // 되돌리기 인 u 또한 플레이어 이동에서 받음.
   if (ch =='h'||ch == 'H'||ch =='l'||ch == 'L'||ch =='k'||ch == 'K'||ch =='j'||ch == 'J'||ch =='u'||ch == 'U'){
      switch (ch) {
         case 'h':
         case 'H':
            dx = -1;
            dy = 0;
            break;
         case 'l':
         case 'L':
            dx = 1;
            dy = 0;
            break;
         case 'k':
         case 'K':
            dx = 0;
            dy = -1;
            break;
         case 'j':
         case 'J':
            dx = 0;
            dy = 1;
            break;
         case 'u':   // Undo 옵션은 이 함수 자체를 끝내야 하기 때문에 이동함수에 추가
         case 'U':
             printf("%c", ch);
             input1 = getch(); //enter 키 날리기
             if(input1=='\n'){
               Undo_LoadMapFunc();
               getPlayerXY();
               return;
             }
             else
              DrawMap();
              getPlayerXY();
              goto Uback;
              return;
         default :
            DrawMap();
            return;
      }
    }
    else if (ch=='f'||ch=='F'){  // 밑에 있는 충돌 체크를 하지 않기 위함
      printf("%c", ch);
      input1 = getch(); // enter 키 날리기
      system("clear");
      LoadFile();
      DrawMap();
      getPlayerXY();
      return;
    }
    else{
      Option(ch);
        return;
    }
      if(map[StageNumber][player_y+dy][player_x+dx]=='#'){ //플레이어가 이동해야할 좌표가 벽(#) 이라면 움직이지 않음.
         DrawMap();
         return;
      }else if(map[StageNumber][player_y+dy][player_x+dx]=='$'){ //플레이어가 이동해야할 좌표에 박스($)가 있을떄
         if((map[StageNumber][player_y+dy*2][player_x+dx*2]=='#')||(map[StageNumber][player_y+dy*2][player_x+dx*2]=='$')){
           //플레이어가 상자를 밀었을때 상자의 위치에 물체(벽 또는 다른 상자)가 있으면 플레이어와 상자 모두 움직이지 않음.
           DrawMap();
            return;
         }
         Undo_SaveMapFunc(); //상자를 움직여야할때, 상자가 움직이기전 Undo_SaveMapFunc() 함수를 호출하여 undo 맵에 현재 맵 상태 저장.
         UndoCheck = 1;
         map[StageNumber][player_y+dy*2][player_x+dx*2] = '$'; // 상자 움직이기.
      }
      if (UndoCheck == 0)//상자가 움직이기 전에 Undo_SaveMapFunc() 으로 undo 맵에 현재 상태를 저장하였다면, 여기서는 Undo_SaveMapFunc()를 호출하지 않음.
         Undo_SaveMapFunc();
      map[StageNumber][player_y][player_x] = ' ';//원래 플레이어 위치를 공백으로 바꿈
      player_x += dx; //플레이어 좌표 이동후 그 자리에 플레이어(@) 를 저장.
      player_y += dy;
      map[StageNumber][player_y][player_x] = '@';
      sameO(); //보관 장소(O) 가 공백으로 되있을때 다시 O 를 그려주는 함수.
      DrawMap(); // 변경 완료 되었으므로 맵을 다시 그림.
      MoveCount++;
}

void Option(char ch){
  char input1;
  char input;
  printf("%c", ch);
  Tback: //t1~t5 를 입력할떄, 백스페이스를 눌러서 숫자를 지웠다면, t 를 사용할것인지 다시 숫자를 입력 받아 t1~t5 를 쓸것인지에대한 분기를 다시 채크해야되므로 goto 문을 사용하여 여기로 되돌아옴.
  input = getch();
  if(input=='\n'){
    switch(ch){
      case 'e':
      case 'E':
        Map_end=clock();
        time_rank();
        SaveFile(); ///현재 맵 상태를 저장
        system("clear");
        system("clear");
        printf("\n\n\nSEE YOU %s....", &UserName);
        printf("\n\n\n(Command)  e");
        exit(0);
      case 'd':
      case 'D':
        Map_stop=clock();  // d 옵션을 시작한 시간
        system("clear");
        system("clear");
        Read_command(); //undocount에 입력되는 거 해결해야함 => 해결 됨
        DrawMap();
        Map_stopEnd=clock();  // d 옵션을 종료한 시간
        Map_display+=Map_stopEnd-Map_stop;
        break;
      case 'n':  // 첫 맵부터 시작
      case 'N':
        StageNumber=0; // 1번 맵으로 다시 시작
        MoveCount=0;  // 초기 상태로 되돌리기 위해서
        UndoCount=0;  // 모든 정보를 초기화 한다.
        Map_display=0;
        MapA();
        DrawMap();
        getPlayerXY();
        // 타이머 초기화 : clock로 시간을 측정하기 때문에 cpu가동시간을
        // 줄이기 위해 n 옵션 마지막으로 이동
        Map_start=clock();
        return;
      case 'r':  // 현재 맵을 처음부터 다시 시작
      case 'R':
        MapA();  // 현재 맵 다시 그리기
        DrawMap();
        getPlayerXY();
        return;
      case 's':  //랭킹 보기
      case 'S':   //잘못된 명령어 수정
        Map_end=clock();
        system("clear");
        time_rank();
        SaveFile();
        DrawMap();  // 이어서 진행
        return;
      case 't':
      case 'T':
        Map_stop=clock();  // t 옵션을 시작한 시간
        Read_rank(0);
        Map_stopEnd=clock();  // t 옵션을 종료한 시간
        Map_display+=Map_stopEnd-Map_stop;
        DrawMap();  // 이어서 진행
        return;
      default : //주어진 명령어가 아닌 값을 입력받으면 예외처리
        DrawMap();
        return;
      }
    }
    else if((ch=='t'||ch=='T')){
      if(input == 127){
        printf("\b \b");
        return;
      }else if (input =='h'||input == 'H'||input =='l'||input == 'L'||input =='k'||input == 'K'||input =='j'||input == 'J'){
        printf("\b \b옵션 입력중 이동키 입력. 다시 입력하세요.");
        return;
      }
      Map_stop=clock();  // t 옵션을 시작한 시간
      printf("%c",input);
      input1=getch();
      if(input1=='\n'){
        if(((input-48)>=1)&&((input-48)<=5)){
          Read_rank(input-48);
         }
         else{
           system("clear");
           printf("\nt,t1-5 이 아닌것을 입력하였습니다. 다른것을 입력하시오\n");
           if(getch()){
              Map_stopEnd=clock();  // t 옵션을 종료한 시간
              Map_display+=Map_stopEnd-Map_stop;
              DrawMap();
              return;
            }
          }
       }else if(input1 == 127){//터미널에서 백스페이스는 127임.
         printf("\b \b");//백 스페이스를 받으면 탈출문자를 써서 뒤로 이동한후 그 자리를 공백으로 지우고 다시 한칸 뒤로 이동.(즉 한 문자를 지우는것을 직접 구현함).
         goto Tback;//이 상황에서, 그냥 t 로 처리할것인지, 다시 숫자를 입력 받을 것인지에대한 분기를 나눠야 하므로 goto 문을 사용하여 위에 Tback: 레이블로 이동.
       }
       Map_stopEnd=clock();  // t 옵션을 종료한 시간
       Map_display+=Map_stopEnd-Map_stop;
     }
     DrawMap();
     return;
}

void EndOneStage(){
  int PlayerCount = 0;
  for(int j = 0; j<SIZE_MAP_Y; j++){
    for(int k = 0; k < SIZE_MAP_X; k++){
      if (map[StageNumber][j][k] == '$' && checkO[StageNumber][j][k] == 'O')
        PlayerCount++; // O가 있어야 할 자리에 $가 있을 경우(조건을 만족한 경우) 카운트++
    }
  }
  if (ClearCount[StageNumber] == PlayerCount){ //맵의 O 개수 = 카운트 일 경우 $가 전부 O에 갔다는 뜻(게임 클리어)
    printf("GAME CLEAR!, Press Any Key!\n");
    Map_end=clock();
    time_rank();
    Load_rank();
    Arrange_rank(StageNumber);
    Save_rank();
    StageNumber++; // 다음 스테이지로
    getPlayerXY();
    UndoCount = 0; // 한 스테이지당 5회이므로 Undo 횟수 초기화
    MoveCount = 0;
    DrawMap();
    Map_display=0;  // display 시간 초기화
    Map_start=clock();
  }
  if (StageNumber == 5){
    system("clear");
    printf("ALL CLEAR!");
    getch();
    exit(0);
  }
}

void time_rank(){
  gap = (Map_end-(Map_display)-Map_start+Fgap)/CLOCKS_PER_SEC;  //1sec = 1000, 시작시간과 끝시간의 차
}

void getPlayerXY(){ // 해당 맵을 2중 for 문으로 돌면서 @, 즉 플레이어의 좌표를 찾는 함수이다.
   for(int i= 0; i< SIZE_MAP_Y ; i++){
      for(int j = 0; j < SIZE_MAP_X; j++){
         if(map[StageNumber][i][j] == '@'){
            player_x = j; //전역변수 player_x, player_y 에 플레이어의 좌표를 저장한다.
            player_y = i;
            break;
         }
      }
   }
}

void MapA(){ //맵을 배열에 저장하는 함수.
   int y=0,x=0,z=-1;//1번 맵을 저장하기전 map 을 만나 z=1 부터 반복이 시작되는것을 막기우해 z=-1 로 시작하여 1번 맵이 맵 배열의 0번부터 저장될수 있게 처리함.
   char ch;
   FILE *fp = fopen("map.txt","rt");//map.txt 파일 개방. 읽기
   if(fp == NULL){//오류 발생시 NULL 반환.
      printf("파일 오픈 실패");
      exit(1);
   }
   while(fscanf(fp,"%c", &ch) != EOF){ //한 글자씩 읽음. 파일의 끝에 도달시 반복 종료.
      if(ch=='m'){//map 중 m 을 만나면 맵 번호를 1 증가 x 좌표 y 좌표 모두 0으로 초기화.
         z++;//map.txt이 맵 위에 map가 있다는 특징 때문에 z 를 -1 로 초기화 했어야했다.
         y=0;
         x=0;
         continue; //밑의 코드 continue 로 이번 반복에서 실행 안함.
      }else if(ch == 'a' || ch == 'p'){//map 중 a, p 를 만날시 아무 동작 안하고 다음 반복으로 이동.
        continue;
      }else if(ch == 'e'){//end 중 e 를 만나면 whie 문을 중료함. 반복의 종료조건.
        break;
      }
      if(ch=='\n'){//개행을 만나면 y좌표를 1 더하고 x 좌표를 0으로 초기화 한다.
        y++;
        x=0;
        continue;
      }
      if(ch=='O'){
        ClearCount[z]++; // 맵 전체의 O(목적지)개수
      }else if(ch == '$'){
        boxCounter[z]++;
      }
      map[z][y][x] = ch; //위의 모든 조건에 해당하지 않는다면, 맵에 해당하므로 map배열에 저장한다.
      checkO[z][y][x] = ch; // 초기 맵 상태 저장
      x++;//x 좌표 1증가
   }
   fclose(fp);
   for(int i=0;i<5;i++){
     if(ClearCount[i]!=boxCounter[i]){
        printf("박스($) 개수와 보관장소(O) 개수가 같지 않습니다. 프로그램을 종료합니다.\n");
        exit(0);
     }
   }
}

void sameO(){ // @나 $가 지나갈 시 O가 사라지는 버그 수정
  int j,k;
  for(j = 0; j<SIZE_MAP_Y; j++){
    for(k = 0; k<SIZE_MAP_X; k++){
      if (map[StageNumber][j][k] == ' ' && checkO[StageNumber][j][k] == 'O')
          map[StageNumber][j][k] = checkO[StageNumber][j][k]; // O가 사라지고 공백이 될 경우 O값을 다시 대입하여 채움
        }
    }
}

void Undo_SaveMapFunc(){
  int i=0,j,k;
  for (i = 4; i > 0; i--){
    for (j = 0; j < SIZE_MAP_X; j++){
      for (k = 0; k < SIZE_MAP_Y; k++){
          Undo_SaveMap[i][j][k] = Undo_SaveMap[i-1][j][k]; //이동 시 1회 전 행동 -> 2회 전 행동이 됨
      }
    }
  }
  for (j = 0; j < SIZE_MAP_X; j++){
    for (k = 0; k < SIZE_MAP_Y; k++){
        Undo_SaveMap[0][j][k] = map[StageNumber][j][k]; // 움직이기 전 상태를 1회 전 행동으로 저장
    }
  }
}

void Undo_LoadMapFunc(){
  int i=0,j,k;
  if (UndoCount >= 5) // 한 스테이지당 Undo는 5회만 가능
    return;
  if (MoveCount <= UndoCount) // 이동 횟수보다 Undo 횟수가 많을 경우를 방지
    return;
  for (j = 0; j < SIZE_MAP_X; j++){
    for (k = 0; k < SIZE_MAP_Y; k++){
        map[StageNumber][j][k] = Undo_SaveMap[0][j][k];
    } // 1회 전 행동을 불러옴
  }
  for (i = 0; i < 4 ; i++){
    for (j = 0; j < SIZE_MAP_X; j++){
      for (k = 0; k < SIZE_MAP_Y; k++){
        Undo_SaveMap[i][j][k] = Undo_SaveMap[i+1][j][k];
      } // 2회 전 행동 -> 1회 전 행동이 됨
    }
  }
  UndoCount++; //Undo 횟수 추가
  DrawMap();
}


void Load_rank(){ // ranking.txt 에서 순위를 읽어 배열에 저장하는 함수
  int i,j;
  int tmp=0;
  int tmp_StageNumber;
  FILE *fp =fopen("ranking.txt","rt"); //ranking.txt 파일 개방. 읽기
  for(i=0;i<5;i++){
    fscanf(fp,"map %d %d\n", &tmp_StageNumber, &TimeCount_Max[i]); //맵 별로 [맵 번호, 순위 개수] 형식으로 Save_rank() 함수에서 썻으므로 그대로 양식에 맞춰 읽기.
    for(j=0;j<TimeCount_Max[i];j++){ //해당 맵의 순위 개수만큼 순위를 읽음.
      fscanf(fp,"%s  %f sec\n", &Names[i][j], &Times[i][j]);
    }
  }
  fclose(fp);
  return;
}

void Arrange_rank(int AR_rank){// 각 맵별로 순위를 추가,정렬하여 순위 배열에 추가하는 함수.
    int i,j;
      if(TimeCount_Max[AR_rank] == 0){ //원래 순위 배열에 해당 맵에 순위가 아무것도 저장이 안되있다면
        strcpy(Names[AR_rank][0], UserName);
        Times[AR_rank][0]=gap;
        TimeCount_Max[AR_rank]++; //해당 맵의 순위 개수 1 증가 후 함수 종료
        return;
      }
      if(gap>Times[AR_rank][TimeCount_Max[AR_rank]-1]){//만약 저장해야되는 시간이 원래 있던 시간들 보다 더 크다면
        if(TimeCount_Max[AR_rank] != 5){//이 떄 순위의 개수가 5개 미만이라면 가장 뒤에 현재 시간과 이름을 저장한다.
          Times[AR_rank][TimeCount_Max[AR_rank]]=gap;
          strcpy(Names[AR_rank][TimeCount_Max[AR_rank]], UserName);
          TimeCount_Max[AR_rank]++;
        }// 순위 개수가 5개 이상이라면 저장하지 않음.
      }else{ // 저장해야되는 시간이 원래 있던 시간들보다 더 작다면
        for(i=0;i<TimeCount_Max[AR_rank];i++){ //반복문을 돌면서 gap보다 큰 시간을 찾음.
          if(Times[AR_rank][i]>gap){
            break;// 이 떄 break 가 되면서 gap 이 저장되어야할 위차가 i 로 저장됨.
          }
        }
        if(TimeCount_Max[AR_rank]!=5){ //맵의 순위 개수가 0개, 5개가 아닐떄(0개일때는 가장 위에서 처리했으므로 제외됨).
          for(j=TimeCount_Max[AR_rank]-1;j>=i;j--){ //gap 보다 큰 순위들을 모두 한칸씩 뒤로 이동시킴
            strcpy(Names[AR_rank][j+1], Names[AR_rank][j]);
            Times[AR_rank][j+1]=Times[AR_rank][j];
          }
          strcpy(Names[AR_rank][i], UserName);
          Times[AR_rank][i]=gap;
          TimeCount_Max[AR_rank]++; //그리고 현재 맵의 순위 개수를 1 증가 시킴.
        }
        else if(TimeCount_Max[AR_rank]==5){
          for(j=3;j>=i;j--){ //맵의 순위가 5개 일경우, 이동해야할 순위가 0~4 번까지 있다면, 3번부터 한칸씩 밀어야 하므로 j=3 으로 함.
            strcpy(Names[AR_rank][j+1], Names[AR_rank][j]);
            Times[AR_rank][j+1]=Times[AR_rank][j];
          }
          strcpy(Names[AR_rank][i], UserName);
          Times[AR_rank][i]=gap;
        }
      }
  return;
}

void Save_rank(){ //현재 순위 배열을 파일에 저장
    int i, j;
    FILE *fp = fopen("ranking.txt", "w"); //ranking.txt 파일 개방. 쓰기
    for(i = 0; i < 5; i++){ //각 맵의 순위 저장.
            fprintf(fp,"map %d %d\n", i+1, TimeCount_Max[i]);//맵 별로 맨 위에 [맵 번호, 순위 개수] 형식으로 저장.
        for(j = 0; j < TimeCount_Max[i]; j++){
                fprintf(fp,"%s  %fsec\n", Names[i][j], Times[i][j]);
            }
    }
    fclose(fp);
    return;
}

void SaveFile(){   //현재 map 상태 파일저장 함수
    char ch;
    int number;
    FILE *ifp=fopen("sokoban.txt","w");
    if(ifp == NULL){
       printf("파일 오픈 실패");
       exit(1);
    }

    if(MoveCount < 6){
    number = MoveCount;
  }
    else
    number = 5;

    fprintf(ifp,"UC %d MC %d\n", UndoCount, MoveCount); // Undo 횟수 + Move 횟수 저장
    fprintf(ifp,"%s\n",UserName); // 사용자 이름 저장
    fprintf(ifp,"%f\n",gap); // 시간 저장
    fprintf(ifp, "%d\n",StageNumber); // 스테이지 저장
    for(int i = 0; i< SIZE_MAP_X ; i++){     // 현재 map 상태 파일에 저장
       for(int j = 0; j < SIZE_MAP_Y; j++){
          fprintf(ifp,"%c", map[StageNumber][i][j]);
       }
       fprintf(ifp,"\n");

     }
     fprintf(ifp,"U"); // Undo 배열의 시작을 표시
     for(int i = 0; i < number ; i++){ // Undo배열 저장
       for(int j = 0; j < SIZE_MAP_Y ; j++){
         for(int k = 0; k < SIZE_MAP_X; k++){
           fprintf(ifp,"%c", Undo_SaveMap[i][j][k]);
         }
         fprintf(ifp,"\n");
       }
       fprintf(ifp,"N"); // i + 1 번째 행동 기록을 위해 i 추가
     }
    fclose(ifp);
}


void LoadFile(){
    int a = 0 ,x = 0,y = 0;
    char ch;

    FILE *loa;
    loa = fopen("sokoban.txt", "rt");

    fscanf(loa,"UC %d MC %d\n", &UndoCount, &MoveCount);
    fscanf(loa,"%s\n", &UserName);
    fscanf(loa,"%f\n", &Fgap);
    fscanf(loa,"%d\n\n", &StageNumber);
    while(fscanf(loa,"%c", &ch) != EOF){ // map 배열 호출

       if(ch == 10){// '\n'아스키값 = 10
         y++;
         x=0;
         continue;
       }
       if(ch == 0){
         continue;
       }
       if(ch == 'U'){ // Undo 배열값 이전까지만 저장해야 하므로
       x = 0;
       y = -1;
       break;
       }
       map[StageNumber][y][x] = ch;
       x++;
    }
    int z = 0;
     while(fscanf(loa,"%c", &ch) != EOF){
       if(ch == 'N'){
         z++;
         y=0;
         x=0;
         continue;
       }
       if(ch == 10){
         y++;
         x=0;
         continue;
       }
       Undo_SaveMap[z][y][x] = ch;
       x++;
     }

  fclose(loa);
  DrawMap();
  getPlayerXY();
}
