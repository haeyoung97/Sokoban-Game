#include <stdio.h>
#include <stdlib.h>
#include <termios.h>
#include <unistd.h>
#include <time.h>  // 타이머 함수를 만들기 위한 헤더파일

#define SIZE_MAP_X 30
#define SIZE_MAP_Y 30

int StageNumber=0;
int stage_x[3], stage_y[3], player_x, player_y;
char map[5][SIZE_MAP_Y][SIZE_MAP_X];
char checkO[5][SIZE_MAP_Y][SIZE_MAP_X]={};
int ClearCount[5] = {};
char Undo_SaveMap[5][SIZE_MAP_Y][SIZE_MAP_X] = {};
int UndoCount = 0;
int MoveCount = 0;
char UserName[10];
int dx=0,dy=0; // 값 전달을 위해 전역변수로 바꿈
int SaveUndo=0;   //save 시 Undo 횟수 저장
int SaveMove=0;   //save 시 Move 횟수 저장
char Names[5][5][10];
float Times[5][5];
int TimeCount_Max[5];//타임 랭크 돌릴떄 각 맵별 랭크 갯수.
char ch;


clock_t Map_start, Map_stop, Map_stopEnd, Map_end;  // 현 시간을 저장할 변수
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

void DrawMap(){
  system("clear");
  system("clear");
  printf("Hello %s\n", UserName);
   for(int i= 0; i< SIZE_MAP_X ; i++){
      for(int j = 0; j < SIZE_MAP_Y; j++){
         printf("%c", map[StageNumber][i][j]);
      }
      printf("\n");
   }
}

void Read_command(){
    system("clear");
    system("clear");
    printf("Hello %s\n\n", UserName);
    printf("h(왼쪽), j(아래), k(위), l(오른쪽)\n");
    printf("u(undo)\n");
    printf("r(replay)\n");
    printf("n(new)\n");
    printf("e(exit)\n");
    printf("s(save)\n");
    printf("f(file load)\n");
    printf("d(display help)\n");
    printf("t(top)\n");
    printf("\n\n게임으로 돌아가려면 아무 키나 누르십시오.\n");
    MoveCount-=1;
    if(getch()){
        return;}
    }

void Read_rank(int num){   // 랭킹 출력 함수
    Load_rank();   // 파일에서 순위를 불러와서 배열에 저장하는 함수
    system("clear");
    system("clear");
    if(num==0){
      for(int i=0;i<5;i++){
        printf("map %d\n", i+1);
        for(int j=0;j<TimeCount_Max[i];j++){
          printf("%10s  %.1fsec\n", Names[i][j], Times[i][j]);
        }
      }
    }else{
      printf("map %d\n", num);
      for(int i=0;i<TimeCount_Max[num-1];i++){
        printf("%10s  %.1fsec\n", Names[num-1][i], Times[num-1][i]);
      }
    }
    printf("\n게임으로 돌아가려면 아무 키나 누르십시오.");

    MoveCount-=1;

    if(getch()){
    }
}

void PlayerMove(void){
   int input1;
   int UndoCheck = 0;
   char ch;

   ch = getch();

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
             Undo_LoadMapFunc();
             DrawMap();
             getPlayerXY();
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
      if(ch=='S'||ch=='s'){  // S 는 PlayerMove 함수를 종료해야 한다.
        printf("%c", ch);
        return;
    }
      if(input1== 8){  // 아스키코드 8은 백스페이스
        printf("%c", ch);
        return;
      }
    }
      if(map[StageNumber][player_y+dy][player_x+dx]=='#'){
         DrawMap();

         return;
      }else if(map[StageNumber][player_y+dy][player_x+dx]=='$'){
         if((map[StageNumber][player_y+dy*2][player_x+dx*2]=='#')||(map[StageNumber][player_y+dy*2][player_x+dx*2]=='$')){
           DrawMap();
            return;
         }
         Undo_SaveMapFunc();
         UndoCheck = 1;
         map[StageNumber][player_y+dy*2][player_x+dx*2] = '$';
      }
      if (UndoCheck == 0)
         Undo_SaveMapFunc();
      map[StageNumber][player_y][player_x] = ' ';
      player_x += dx;
      player_y += dy;
      map[StageNumber][player_y][player_x] = '@';
      sameO();
      DrawMap();
      MoveCount++;
}

void Option(char ch){
  char input1;
  char input;
  printf("%c", ch);
  input = getch();
  if(input=='\n'){
    switch(ch){
      case 'e':
      case 'E':
        time_rank();
        SaveFile(); ///현재 맵 상태를 저장
        system("clear");
        system("clear");
        printf("\n\n\nSEE YOU %s....\n\n\n", &UserName);
        exit(0);
      case 'd':
      case 'D':
        Map_stop=clock();  // d 옵션을 시작한 시간
        system("clear");
        system("clear");
        Read_command(); //undocount에 입력되는 거 해결해야함 => 해결 됨
        DrawMap();
        Map_stopEnd=clock();  // d 옵션을 종료한 시간
        break;
      case 'n':  // 첫 맵부터 시작
      case 'N':
        StageNumber=0; // 1번 맵으로 다시 시작
        Map_start=clock(); // 타이머 초기화
        MapA();
        DrawMap();
        getPlayerXY();
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
        Read_rank(0);
        printf("t");
      default :
        return;
      }
    }
    else if((ch=='t'||ch=='T')){
        switch(input){
          case '1':
            printf("%c", input);
            if(getch())
            Read_rank(1);
            break;
          case '2':
            Read_rank(2);
            break;
          case '3':
            Read_rank(3);
            break;
          case '4':
            Read_rank(4);
            break;
          case '5':
            Read_rank(5);
            break;
          default:
            system("clear");
            printf("\nt,t1-5 이 아닌것을 입력하였습니다. 다른것을 입력하시오\n");
            if(getch())
                return;
            DrawMap();
            break;
        }
        if(input == 8||input1==8){  // 아스키코드 8은 백스페이스
          printf("%c", ch);
          return;
        }
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
    printf("GAME CLEAR!, Press Any Key!");
    Map_end=clock();
    time_rank();
    Load_rank();
    Arrange_rank(StageNumber);
    Save_rank();

    StageNumber++; // 다음 스테이지로
    getPlayerXY();
    UndoCount = 0; // 한 스테이지당 5회이므로 Undo 횟수 초기화
    DrawMap();
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

  gap = (Map_end+(Map_stopEnd-Map_stop)-Map_start+Fgap);///CLOCKS_PER_SEC;  //1sec = 1000, 시작시간과 끝시간의 차
//  Map_start = Map_end;  // 새로운 맵 시작 시간 초기화

}

void getPlayerXY(){
   for(int i= 0; i< SIZE_MAP_Y ; i++){
      for(int j = 0; j < SIZE_MAP_X; j++){
         if(map[StageNumber][i][j] == '@'){
            player_x = j;
            player_y = i;
            break;
         }
      }
   }
}

void MapA(){
   int y=0,x=0,z=-1;
   char ch;
   FILE *fp = fopen("map.txt","rt");
   if(fp == NULL){
      printf("파일 오픈 실패");
      exit(1);
   }
   while(fscanf(fp,"%c", &ch) != EOF){
      if(ch=='m'){
         z++;
         y=0;
         x=0;
         continue;
      }else if(ch == 'a' || ch == 'p'){
        continue;
      }else if(ch == 'e'){
        break;
      }
      if(ch=='\n'){
        y++;
        x=0;
        continue;
      }
      if(ch=='O'){
        ClearCount[z]++; // 맵 전체의 O 개수
      }
      map[z][y][x] = ch;
      checkO[z][y][x] = ch; // 초기 맵 상태 저장
      x++;
   }
   fclose(fp);
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


void Load_rank(){
  int i,j;
  int tmp=0;
  int tmp_StageNumber;
  FILE *testF = fopen("testF.txt", "w");//Load가 잘 되나 확인하는 파일.
  FILE *fp =fopen("ranking.txt","rt");
  for(i=0;i<5;i++){
    fscanf(fp,"map %d %d\n", &tmp_StageNumber, &TimeCount_Max[i]);
    fprintf(testF ,"\n map : %d %d\n", tmp_StageNumber, TimeCount_Max[i]);
    for(j=0;j<TimeCount_Max[i];j++){
      fscanf(fp,"%s  %f sec\n", &Names[i][j], &Times[i][j]);
      fprintf(testF ,"Names : %s     Times : %f\n", Names[i][j], Times[i][j]);
    }
  }
  fclose(fp);
  fclose(testF);
  return;
}

void Arrange_rank(int AR_rank){
    int i,j;
      if(TimeCount_Max[AR_rank] == 0){
        for(int x=0;x<10;x++){
          Names[AR_rank][0][x]=UserName[x];
        }
        Times[AR_rank][0]=gap;
        TimeCount_Max[AR_rank]++;
        return;
      }
      if(gap>Times[AR_rank][TimeCount_Max[AR_rank]-1]){
        if(TimeCount_Max[AR_rank] != 5){
          Times[AR_rank][TimeCount_Max[AR_rank]]=gap;
          for(int x=0;x<10;x++){
            Names[AR_rank][TimeCount_Max[AR_rank]][x]=UserName[x];
          }
          TimeCount_Max[AR_rank]++;
        }
      }else{
        for(i=0;i<TimeCount_Max[AR_rank];i++){
          if(Times[AR_rank][i]>gap){
            break;
          }
        }
        if(TimeCount_Max[AR_rank]!=5){
          for(j=TimeCount_Max[AR_rank]-1;j>=i;j--){
            for(int x=0;x<10;x++){
              Names[AR_rank][j+1][x]=Names[AR_rank][j][x];
            }
            Times[AR_rank][j+1]=Times[AR_rank][j];
          }
          for(int x=0;x<10;x++){
            Names[AR_rank][i][x]=UserName[x];
          }
          Times[AR_rank][i]=gap;
          TimeCount_Max[AR_rank]++;
        }
        else if(TimeCount_Max[AR_rank]==5){
          for(j=3;j>=i;j--){
            for(int x=0;x<10;x++){
              Names[AR_rank][j+1][x]=Names[AR_rank][j][x];
            }
            Times[AR_rank][j+1]=Times[AR_rank][j];
          }
          for(int x=0;x<10;x++){
            Names[AR_rank][i][x]=UserName[x];
          }
          Times[AR_rank][i]=gap;
        }
      }
  return;
}

void Save_rank(){
    int i, j;
    FILE *fp = fopen("ranking.txt", "w");
    for(i = 0; i < 5; i++){
            fprintf(fp,"map %d %d\n", i+1, TimeCount_Max[i]);
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

int main(){

   printf("Start....\n");
   printf("Input name : ");
   scanf("%s", &UserName);
   getch();

   MapA();
   DrawMap();
   getPlayerXY();

   Map_start=clock(); // 게임 시작 시 첫 시간 저장
   //printf("%d\n",Map_start);
   printf("\n(Command) ");
   while(1){
      //gap = (Map_end+(Map_stopEnd-Map_stop)-Map_start+Fgap);///CLOCKS_PER_SEC;  //1sec = 1000, 시작시간과 끝시간의 차
      PlayerMove();
      EndOneStage();
      // printf("\t%d 초\n", gap);
      // printf("\t%d 초\n", Fgap);
      // printf("\t%d 초\n", Map_start);
      // printf("\t%d 초\n", Map_end);
      // printf("\t%d 초\n", Map_stop);
      // printf("\t%d 초\n", Map_stopEnd);
      // gap = (Map_end+(Map_stopEnd-Map_stop)-Map_start+Fgap);///CLOCKS_PER_SEC;  //1sec = 1000, 시작시간과 끝시간의 차
      // printf("\n\n\t%d 초\n", gap);
      printf("\n(Command) ");
   }

   return 0;
}
