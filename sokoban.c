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
char Undo_SaveMap[5][SIZE_MAP_Y][SIZE_MAP_X];
int UndoCount = 0;
int MoveCount = -1;  //초기 enter 값 입력되는 것을 제외 시켜주기 위함
char UserName[10];
int dx=0,dy=0; // 값 전달을 위해 전역변수로 바꿈
int save=0;  // 몇 번 f 옵션을 썼는지 체크하기 위한 변수
int SaveUndo=0;   //save 시 Undo 횟수 저장
int SaveMove=0;   //save 시 Move 횟수 저장
char Names[5][5][10];
float Times[5][5];
int TimeCount_Max[5];//타임 랭크 돌릴떄 각 맵별 랭크 갯수.


time_t Map_start, Map_stop, Map_stopEnd, Map_end;  // 현 시간을 저장할 변수
int gap=0,Fgap=0;

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
   printf("(Command)");
}

void Read_command(void){
    char ch;

    FILE *fp = fopen("command_explain.txt", "r");

    if(fp == NULL){
        printf("파일을 열 수 없음\n");
        return;
    }
    while(fscanf(fp,"%c", &ch) != EOF){
        printf("%c", ch);
    }
    printf("\n게임으로 돌아가려면 아무 키나 누르십시오.");

    MoveCount-=1;

    if(getch()){
    fclose(fp);
    }
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
      SaveFile();
      LoadFile();
      DrawMap();
      return;
    }
    else{
      Option(ch);
      if(ch=='S'||ch=='s'){  // S 는 PlayerMove 함수를 종료해야 한다.
        return;
        }
      }


      if(map[StageNumber][player_y+dy][player_x+dx]=='#'){
         return;
      }else if(map[StageNumber][player_y+dy][player_x+dx]=='$'){
         if((map[StageNumber][player_y+dy*2][player_x+dx*2]=='#')||(map[StageNumber][player_y+dy*2][player_x+dx*2]=='$')){
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
  char input;
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
      time(&Map_stop);  // d 옵션을 시작한 시간
      system("clear");
      Read_command(); //undocount에 입력되는 거 해결해야함 => 해결 됨
      DrawMap();
      time(&Map_stopEnd);  // d 옵션을 종료한 시간
      break;
   case 'n':  // 첫 맵부터 시작
   case 'N':
      StageNumber=0; // 1번 맵으로 다시 시작
      time(&Map_start); // 타이머 초기화
      printf("%d\n",Map_start);
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
      time(&Map_end);
      system("clear");
      time_rank();
      SaveFile();
      DrawMap();  // 이어서 진행
      return;
    case 'f':
    case 'F':
      system("clear");
      LoadFile();
      DrawMap();
      getPlayerXY();
      break;
    case 't':
    case 'T':
      input = getch();
      input = TOption(input);
      switch(input){
        case '0':
          Read_rank(0);
          printf("t");
          break;
        case '1':
          Read_rank(1);
          printf("t1");
          break;
        case '2':
          Read_rank(2);
          printf("t2");
          break;
        case '3':
          Read_rank(3);
          printf("t3");
          break;
        case '4':
          Read_rank(4);
          printf("t4");
          break;
        case '5':
          Read_rank(5);
          printf("t5");
          break;
        default:
          printf("\nt,t1-6 이 아닌것을 입력하였습니다. 다른것을 입력하시오\n");
          break;
      }
      break;
    // case '@':
    //   StageNumber++;
    //   //time(&Map_end);   // <1> 번으로 이동할 것 -test용
    //   //EndOneStage();
    //   printf("%d\n",Map_end);
    //   time_rank();
    //   getPlayerXY();
    //   break;
   default :
    DrawMap();
    return;
  }
}

char TOption(char input){
  switch (input){
    case '\n':
      return '0';
    default :
      return input;
    }
}

void EndOneStage(){
  int PlayerCount = 0;
  for(int j = 0; j<SIZE_MAP_Y; j++){
    for(int k = 0; k < SIZE_MAP_X; k++){
      if (map[StageNumber][j][k] == '$' && checkO[StageNumber][j][k] == 'O')
        PlayerCount++;
    }
  }
  if (ClearCount[StageNumber] == PlayerCount){
    printf("GAME CLEAR!, Press Any Key!");
    StageNumber++;
    getPlayerXY();
    UndoCount = 0;
    // 하나의 맵을 끝낸 시간을 측정 <1>
    time(&Map_end);
    time_rank();
    Load_rank();
    Arrange_rank();
    Save_rank();
    DrawMap();
  }
  if (StageNumber == 6)
    printf("ALL CLEAR!");
}

void time_rank(){

  gap = (Map_end+/*(Map_stopEnd-Map_stop)*/-Map_start+Fgap);///CLOCKS_PER_SEC;  //1sec = 1000, 시작시간과 끝시간의 차
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
        ClearCount[z]++;
      }
      map[z][y][x] = ch;
      checkO[z][y][x] = ch;
      x++;
   }
   fclose(fp);
}

void sameO(){
  int j,k;
  for(j = 0; j<SIZE_MAP_Y; j++){
    for(k = 0; k<SIZE_MAP_X; k++){
      if (map[StageNumber][j][k] == ' ' && checkO[StageNumber][j][k] == 'O')
          map[StageNumber][j][k] = checkO[StageNumber][j][k];
        }
    }
}

void Undo_SaveMapFunc(){
  int i=0,j,k;
  for (i = 4; i > 0; i--){
    for (j = 0; j < SIZE_MAP_X; j++){
      for (k = 0; k < SIZE_MAP_Y; k++){
          Undo_SaveMap[i][j][k] = Undo_SaveMap[i-1][j][k];
      }
    }
  }
  for (j = 0; j < SIZE_MAP_X; j++){
    for (k = 0; k < SIZE_MAP_Y; k++){
        Undo_SaveMap[0][j][k] = map[StageNumber][j][k];
    }
  }
}

void Undo_LoadMapFunc(){
  int i=0,j,k;
  if (UndoCount >= 5)
    return;
  if (MoveCount <= UndoCount)
    return;
  for (j = 0; j < SIZE_MAP_X; j++){
    for (k = 0; k < SIZE_MAP_Y; k++){
        map[StageNumber][j][k] = Undo_SaveMap[0][j][k];
    }
  }
  for (i = 0; i < 4 ; i++){
    for (j = 0; j < SIZE_MAP_X; j++){
      for (k = 0; k < SIZE_MAP_Y; k++){
        Undo_SaveMap[i][j][k] = Undo_SaveMap[i+1][j][k];
      }
    }
  }
  UndoCount++;
  DrawMap();
}

void SaveFile(){   //현재 map 상태 파일저장 함수
    save+=1; // 함수 실행 횟수 저장
    char ch;
    FILE *ifp=fopen("sokoban.txt","a");  //연속해서 작성해야 하기 때문에
    if(ifp == NULL){
       printf("파일 오픈 실패");
       exit(1);
    }
    fprintf(ifp,"UC %d MC %d\n", UndoCount, MoveCount);
    fprintf(ifp,"%s\n",UserName);
    fprintf(ifp, "%d\n",StageNumber);
    for(int i= 0; i< SIZE_MAP_X ; i++){     // 현재 map 상태 파일에 저장
       for(int j = 0; j < SIZE_MAP_Y; j++){
          fprintf(ifp,"%c", map[StageNumber][i][j]);
       }
       fprintf(ifp,"\n");
    }

    // fprintf(ifp,"%f\n",gap);
    fclose(ifp);


    //SaveUndo = UndoCount;   // 현재 UndoCount값 저장
    //SaveMove =  MoveCount;   // 현재 MoveCount값 저장


}

void LoadFile(){
    int a = 0 ,x = 0,y = 0;
    char ch;
    FILE *loa;
    loa = fopen("sokoban.txt", "rt");
    fscanf(loa,"UC %d MC %d\n", &UndoCount, &MoveCount);
    fscanf(loa,"%s\n", &UserName);
    fscanf(loa,"%d\n\n", &StageNumber);
    while(fscanf(loa,"%c", &ch) != EOF){
       if(ch == 10){// '\n'아스키값 = 10
         y++;
         x=0;
         continue;
       }
       if(ch == 0){
         continue;
       }
       map[StageNumber][y][x] = ch;
       x++;

    }

  fclose(loa);
  DrawMap();
  getPlayerXY();
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

void Arrange_rank(){
    int i,j;
    for(int hos=0;hos<5;hos++){
      if(TimeCount_Max[hos] == 0){
        for(int x=0;x<10;x++){
          Names[hos][0][x]=UserName[x];
        }
        Times[hos][0]=gap;
        TimeCount_Max[hos]++;
        break;
      }
      if(gap>Times[hos][TimeCount_Max[hos]-1]){
        if(TimeCount_Max[hos] != 5){
          Times[hos][TimeCount_Max[hos]]=gap;
          for(int x=0;x<10;x++){
            Names[hos][TimeCount_Max[hos]][x]=UserName[x];
          }
          TimeCount_Max[hos]++;
        }
      }else{
        for(i=0;i<TimeCount_Max[hos];i++){
          if(Times[hos][i]>gap){
            break;
          }
        }

        if(TimeCount_Max[hos]!=5){
          for(j=TimeCount_Max[hos]-1;j>=i;j--){
            for(int x=0;x<10;x++){
              Names[hos][j+1][x]=Names[hos][j][x];
            }
            Times[hos][j+1]=Times[hos][j];
          }
          for(int x=0;x<10;x++){
            Names[hos][i][x]=UserName[x];
          }
          Times[hos][i]=gap;
          TimeCount_Max[hos]++;
        }else if(TimeCount_Max[hos]==5){
          for(j=3;j>=i;j--){
            for(int x=0;x<10;x++){
              Names[hos][j+1][x]=Names[hos][j][x];
            }
            Times[hos][j+1]=Times[hos][j];
          }
          for(int x=0;x<10;x++){
            Names[hos][i][x]=UserName[x];
          }
          Times[hos][i]=gap;
        }

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

int main(){

   printf("Start....\n");
   printf("Input name : ");
   scanf("%s", &UserName);

   MapA();
   DrawMap();
   getPlayerXY();

   //SaveCall();  // test 용

   time(&Map_start); // 게임 시작 시 첫 시간 저장
   printf("%d\n",Map_start);

   while(1){
      PlayerMove();
      EndOneStage();
      gap = (Map_end+(Map_stopEnd-Map_stop)-Map_start+Fgap);///CLOCKS_PER_SEC;  //1sec = 1000, 시작시간과 끝시간의 차
      printf("\n\n\t%d 초\n", gap);
      printf("\t%d 초\n", Map_start);
      printf("\t%d 초\n", Map_end);
      printf("\t%d 초\n", Map_stop);
      printf("\t%d 초\n", Map_stopEnd);
      gap = (Map_end+(Map_stopEnd-Map_stop)-Map_start+Fgap);///CLOCKS_PER_SEC;  //1sec = 1000, 시작시간과 끝시간의 차
      printf("\n\n\t%d 초\n", gap);
      printf("MoveCount : %d, UndoCount : %d, MoveCount - UndoCount = %d", MoveCount, UndoCount, MoveCount - UndoCount);
   }

   return 0;
}
