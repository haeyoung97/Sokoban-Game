#include <stdio.h>
#include <stdlib.h>
#include <windows.h>
#include <termios.h>
#include <unistd.h>

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

int getch(void)
{
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
   for(int i= 0; i< SIZE_MAP_X ; i++){
      for(int j = 0; j < SIZE_MAP_Y; j++){
         printf("%c", map[StageNumber][i][j]);
      }
      printf("\n");
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
         case '@':
           StageNumber++;
           getPlayerXY();
           break;
          default :
           DrawMap();
           return;
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

void EndOneStage(){
  int PlayerCount = 0;
  for(int j = 0; j<SIZE_MAP_Y; j++){
    for(int k = 0; k < SIZE_MAP_X; k++){
  if (map[StageNumber][j][k] == '$' && checkO[StageNumber][j][k] == 'O')
  PlayerCount++;
}}
  if (ClearCount[StageNumber] == PlayerCount){
  printf("GAME CLEAR!, Press Any Key!");
  StageNumber++;
  getPlayerXY();
  UndoCount = 0;
}
  if (StageNumber == 6)
  printf("ALL CLEAR!");
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
  MapA();
   DrawMap();
   getPlayerXY();
   while(1){
      PlayerMove();
      EndOneStage();
      printf("MoveCount : %d, UndoCount : %d, MoveCount - UndoCount = %d", MoveCount, UndoCount, MoveCount - UndoCount);
   }

   return 0;
}
