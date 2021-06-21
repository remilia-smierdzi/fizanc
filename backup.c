#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <ncurses.h>
#include <stdbool.h>
#include <limits.h>

#define ARRAY_SIZE 100
#define PROPAGATION_SIZE 100

struct point{
	int x;
	int y;
};
struct cell{
	float value;
};
struct person{
	struct point pos;
};
struct cell cls[ARRAY_SIZE][ARRAY_SIZE];
struct person ppl[ARRAY_SIZE];

void propagate(int x,int y);
struct point qpropreal[PROPAGATION_SIZE];
struct point (*qprop)[];

struct point nqpropreal[PROPAGATION_SIZE];
struct point (*nqprop)[];
struct point (*temp)[];
int cprop=1;
int ncprop=0;

int main(int argc,char *argv[]){
	if (argc!=2){
		printf("co.\n");
		return 1;
	}
	qprop = &qpropreal;
	nqprop = &nqpropreal;
	FILE *f;
	f = fopen(argv[1], "r");
	if (!f) return 1;
	char c;
	int cline=0,crow=0,mrow=0;
	unsigned int ippl=0;
	while((c=getc(f))!=EOF){
		switch (c) {
			case ' ':
				cls[crow++][cline].value = 0;
				break;
			case '#':
				cls[crow++][cline].value = 10000;
				break;
			case '\n':
				if (crow>mrow) mrow=crow;
				cline++;
				crow=0;
				break;
			case 'p':
				ppl[ippl].pos.x = crow;
				ppl[ippl++].pos.y = cline;
				cls[crow++][cline].value = 0;
			case 'e':
				(*qprop)[0].x = crow;
				(*qprop)[0].y = cline;
				cls[crow++][cline].value = 1;
				break;
			default:
				printf("bad file format");
				return 1;
		}
	}
	fclose(f);
	initscr();
	refresh();
	while(cprop){
		for (int i=0;i<cprop;i++){
			propagate((*qprop)[i].x,(*qprop)[i].y);
			for (int iy=0;iy<cline+1;iy++){
				for (int ix=0;ix<mrow;ix++){
					mvprintw(iy, ix*5, "%2.1f",cls[ix][iy].value==10000 ? 9.9 : cls[ix][iy].value);
					//mvaddch(iy, ix, cls[ix][iy].value==10000 ? '#' : ' ');
				}
			}
			for (int j=0;j<cprop;j++){
				mvprintw(23, 5*j, "%d",(*qprop)[j].x);
				mvprintw(24, 5*j, "%d",(*qprop)[j].y);
			}
			refresh();
			getch();
		}
		cprop=ncprop;
		ncprop=0;
		temp=qprop;
		qprop=nqprop;
		nqprop=temp;
	}
	
	
	for (int iy=0;iy<cline+1;iy++){
		for (int ix=0;ix<mrow;ix++){
			mvprintw(iy, ix*5, "%2.1f",cls[ix][iy].value==10000 ? 9.9 : cls[ix][iy].value);
			//mvaddch(iy, ix, cls[ix][iy].value==10000 ? '#' : ' ');
		}
	}
	refresh();
	getch();
	endwin();
}

void propagate(int x,int y){
	if(cls[x-1][y].value==0){
		cls[x-1][y].value = cls[x][y].value+1;
		//mvprintw(y, (x-1)*5, "%2.1f",cls[x-1][y].value==10000 ? 9.9 : cls[x-1][y].value);
		(*nqprop)[ncprop].x = x-1;
		(*nqprop)[ncprop++].y = y;
	}
	if(cls[x][y-1].value==0){
		cls[x][y-1].value = cls[x][y].value+1;
		//mvprintw(y-1, x*5, "%2.1f",cls[x][y-1].value==10000 ? 9.9 : cls[x][y-1].value);
		(*nqprop)[ncprop].x = x;
		(*nqprop)[ncprop++].y = y-1;
	}
	if(cls[x+1][y].value==0){
		cls[x+1][y].value = cls[x][y].value+1;
		//mvprintw(y, (x+1)*5, "%2.1f",cls[x+1][y].value==10000 ? 9.9 : cls[x+1][y].value);
		(*nqprop)[ncprop].x = x+1;
		(*nqprop)[ncprop++].y = y;
	}
	if(cls[x][y+1].value==0){
		cls[x][y+1].value = cls[x][y].value+1;
		//mvprintw(y+1, x*5, "%2.1f",cls[x][y+1].value==10000 ? 9.9 : cls[x][y+1].value);
		(*nqprop)[ncprop].x = x;
		(*nqprop)[ncprop++].y = y+1;
	}

	if(cls[x-1][y-1].value==0){
		cls[x-1][y-1].value = cls[x][y].value+1.5;
		//mvprintw(y-1, (x-1)*5, "%2.1f",cls[x-1][y-1].value==10000 ? 9.9 : cls[x-1][y-1].value);
		(*nqprop)[ncprop].x = x-1;
		(*nqprop)[ncprop++].y = y-1;
	}
	if(cls[x+1][y-1].value==0){
		cls[x+1][y-1].value = cls[x][y].value+1.5;
		//mvprintw(y-1, (x+1)*5, "%2.1f",cls[x+1][y-1].value==10000 ? 9.9 : cls[x+1][y-1].value);
		(*nqprop)[ncprop].x = x+1;
		(*nqprop)[ncprop++].y = y-1;
	}
	if(cls[x+1][y+1].value==0){
		cls[x+1][y+1].value = cls[x][y].value+1.5;
		//mvprintw(y+1, (x+1)*5, "%2.1f",cls[x+1][y+1].value==10000 ? 9.9 : cls[x+1][y+1].value);
		(*nqprop)[ncprop].x = x+1;
		(*nqprop)[ncprop++].y = y+1;
	}
	if(cls[x-1][y+1].value==0){
		cls[x-1][y+1].value = cls[x][y].value+1.5;
		//mvprintw(y+1, (x-1)*5, "%2.1f",cls[x-1][y+1].value==10000 ? 9.9 : cls[x-1][y+1].value);
		(*nqprop)[ncprop].x = x-1;
		(*nqprop)[ncprop++].y = y+1;
	}
}