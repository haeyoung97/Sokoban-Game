#include <stdio.h>
#include <stdlib.h>
#include <windows.h>
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
int MoveCount = 0;
char UserName;

clock_t Map_start, Map_stop, Map_stopEnd, Map_end;  // 현 시간을 저장할 변수
float gap;

void DrawMap();
void PlayerErase();
void PlayerDraw();
int getch();
void PlayerMove();
void EndOneStage();
void getPlayerXY();
void ClearMap();
void sameO();
void Undo_SaveMapFunc();
void Undo_LoadMapFunc();
void MapA();
void Read_command();
void Read_rank();
void Option(char);

void time_rank();

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
  printf("Hello %s\n", &UserName);
   for(int i= 0; i< SIZE_MAP_X ; i++){
      for(int j = 0; j < SIZE_MAP_Y; j++){
         printf("%c", map[StageNumber][i][j]);
      }
      printf("\n");
   }
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

void Read_rank(void){
    char ch;

    FILE *fp = fopen("ranking.txt", "r");

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

void PlayerMove(void){
   int dx=0,dy=0;
   int UndoCheck = 0;
   char ch;
   ch = getch();
   if(ch == 'e'){
      exit(0);
   }
   else if (ch =='h'||ch == 'H'||ch =='l'||ch == 'L'||ch =='k'||ch == 'K'||ch =='j'||ch == 'J'||ch =='u'||ch == 'U'){
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
         case 'u':
         case 'U':
            Undo_LoadMapFunc();
            DrawMap();
            getPlayerXY();
            return;
      }
    }
      else
        Option(ch);


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
  switch(ch){
    case 'd':
    case 'D':
      Map_stop = clock();  // d 옵션을 시작한 시간
      system("clear");
      Read_command(); //undocount에 입력되는 거 해결해야함
      DrawMap();
      Map_stopEnd = clock();  // d 옵션을 종료한 시간
      break;
   case 'n':  // 첫 맵부터 시작
   case 'N':
      StageNumber=0; // 1번 맵으로 다시 시작
      Map_start = clock(); // 타이머 초기화
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
    case 't':  //랭킹 보기
    case 'T':
      Map_stop = clock();
      system("clear");
      Read_rank();
      DrawMap();
      Map_stopEnd = clock();  // d 옵션을 종료한 시간
      break;
    case '@':
      StageNumber++;
      Map_end = clock();   // <1> 번으로 이동할 것 -test용
      printf("%d\n",Map_end);
      time_rank();
      getPlayerXY();
      break;
   default :
    DrawMap();
    return;
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

  }
  if (StageNumber == 6)
  printf("ALL CLEAR!");
}

void time_rank(){
  FILE *fsaveRank = fopen("ranking.txt", "w");

  gap = (float)(Map_end+(Map_stopEnd-Map_stop)-Map_start)/CLOCKS_PER_SEC;  //1sec = 1000, 시작시간과 끝시간의 차
  fprintf(fsaveRank,"%.3f\n",gap);
  fprintf(fsaveRank,"Ranking : %.3f초\n",gap);

  Map_start = Map_end;  // 새로운 맵 시작 시간 초기화

  fclose(fsaveRank);


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
      puts("파일 오픈 실패");
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

int main(){

   printf("Start....\n");
   printf("Input name : ");
   scanf("%s", &UserName);

   MapA();
   DrawMap();
   getPlayerXY();

   Map_start = clock(); // 게임 시작 시 첫 시간 저장
   printf("%d\n",Map_start);

   while(1){
      PlayerMove();
      EndOneStage();
      printf("\n\n\t%.3f 초\n", gap);
      printf("MoveCount : %d, UndoCount : %d, MoveCount - UndoCount = %d", MoveCount, UndoCount, MoveCount - UndoCount);
   }

   return 0;
}
