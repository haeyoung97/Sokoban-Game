#include <stdio.h>
#include <stdlib.h>
#include <windows.h>
#include <termios.h>
#include <unistd.h>
#define SIZE_MAP_X 30
#define SIZE_MAP_Y 30

int StageNumber=0;
int stage_x[3], stage_y[3], player_x, player_y;
char map[5][SIZE_MAP_X][SIZE_MAP_Y];

void DrawMap();
int getch();
void PlayerMove();
void getPlayerXY();
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


void MapA(){
   int y=0,x=0,z=-1;
   char ch;
   FILE *fp = fopen("map.txt","rt");
   if(fp == NULL){
      puts("파일 열기 오류 발생");
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
      map[z][y][x] = ch;
      x++;
   }
   fclose(fp);
}

int main(){
  MapA();
  DrawMap();
  return 0;
}
