#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <ncurses.h>
#include <stdbool.h>
#include <time.h>

#define ARRAY_SIZE 1000
#define PROPAGATION_SIZE 5000

struct point{
	int x;
	int y;
};
struct cell{
	float value;
};
struct person{
	struct point pos;
	bool in;
};

void propagate(int x,int y);
void pmove(struct person *p);
bool onexit(int x,int y);
void pshuf();

struct cell cls[ARRAY_SIZE][ARRAY_SIZE];
struct person ppl[ARRAY_SIZE];
unsigned int ippl=0;

struct point qpropreal[PROPAGATION_SIZE];
struct point *qprop = qpropreal;

struct point nqpropreal[PROPAGATION_SIZE];
struct point *nqprop = nqpropreal;
struct point *temp;
unsigned int cprop=0;
unsigned int ncprop=0;
unsigned int exited=0;
struct point exits[ARRAY_SIZE];
unsigned int cexits;

int main(int argc,char *argv[]){
	if (argc!=2){
		printf("usage: first arg is filename of simulation template\n");
		return 2;
	}
	FILE *f;
	f = fopen(argv[1], "r");
	if (!f) return 1;
	char c;
	int cline=0,crow=0,mrow=0;
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
				break;
			case 'e':
				qprop[cprop].x = crow;
				qprop[cprop].y = cline;
				cprop++;
				cls[crow++][cline].value = 1;
				break;
			default:
				printf("bad file format");
				return 1;
		}
	}
	fclose(f);
	if(cprop<1){
		printf("bad f format");
		return 1;
	}
	printf("fread ok.");
	for (int i=0;i<cprop;i++){
		exits[i]=qprop[i];
	}
	cexits=cprop;
	time_t t;
	srand((unsigned) time(&t));
	for (int i=0;i<ippl;i++){
		ppl[i].in=TRUE;
	}
	printf("initing nc.");
	initscr();
	noecho();
	curs_set(0);
	while(cprop){
		for (int i=0;i<cprop;i++){
			propagate(qprop[i].x,qprop[i].y);
			// for (int iy=0;iy<cline+1;iy++){
			// 	for (int ix=0;ix<mrow;ix++){
			// 		if (cls[ix][iy].value==10000)
			// 			mvprintw(iy*2, ix*5,"#");
			// 		else if (cls[ix][iy].value!=0){
			// 			mvprintw(iy*2, ix*5, "%2.1f",cls[ix][iy].value);
			// 		}
			// 	}
			// }
			// for (int j=0;j<cprop;j++){
			// 	mvprintw(23, 5*j, "%d",qprop[j].x);
			// 	mvprintw(24, 5*j, "%d",qprop[j].y);
			// }
			// mvprintw(getmaxy(stdscr)-1,0,"%d",cprop);
			// refresh();
			// sleep(1);
			// getch();
		}
		
		cprop=ncprop;
		ncprop=0;
		temp=qprop;
		qprop=nqprop;
		nqprop=temp;
	}
	refresh();
	for (int iy=0;iy<cline+1;iy++){
		for (int ix=0;ix<mrow;ix++){
			mvaddch(iy,ix,cls[ix][iy].value==10000 ? '#' : ' ');
			// mvprintw(iy, ix*5, "%2.1f",cls[ix][iy].value==10000 ? 9.9 : cls[ix][iy].value);
			//mvaddch(iy, ix, cls[ix][iy].value==10000 ? '#' : ' ');
		}
	}
	for (int i=0;i<ippl;i++){
		mvaddch(ppl[i].pos.y, ppl[i].pos.x,'p');
	}
	for (int i=0;i<cexits;i++){
		mvaddch(exits[i].y, exits[i].x,'e');
	}
	refresh();
	getch();
	while (true){
		pshuf();
		if (exited==ippl) break;
		for (int i=0;i<ippl;i++){
			if(ppl[i].in){
				mvaddch(ppl[i].pos.y,ppl[i].pos.x,' ');
				pmove(&ppl[i]);
				if (onexit(ppl[i].pos.x,ppl[i].pos.y)){
					exited++;
					ppl[i].in=FALSE;
				}else{
					mvaddch(ppl[i].pos.y,ppl[i].pos.x,'p');
				}
			}
		}
		refresh();
		getch();
		// usleep(250000);
	}
	getch();
	endwin();
}

void propagate(int x,int y){
	if(cls[x-1][y].value==0){
		cls[x-1][y].value = cls[x][y].value+1;
		//mvprintw(y, (x-1)*5, "%2.1f",cls[x-1][y].value==10000 ? 9.9 : cls[x-1][y].value);
		nqprop[ncprop].x = x-1;
		nqprop[ncprop++].y = y;
	}
	if(cls[x][y-1].value==0){
		cls[x][y-1].value = cls[x][y].value+1;
		//mvprintw(y-1, x*5, "%2.1f",cls[x][y-1].value==10000 ? 9.9 : cls[x][y-1].value);
		nqprop[ncprop].x = x;
		nqprop[ncprop++].y = y-1;
	}
	if(cls[x+1][y].value==0){
		cls[x+1][y].value = cls[x][y].value+1;
		//mvprintw(y, (x+1)*5, "%2.1f",cls[x+1][y].value==10000 ? 9.9 : cls[x+1][y].value);
		nqprop[ncprop].x = x+1;
		nqprop[ncprop++].y = y;
	}
	if(cls[x][y+1].value==0){
		cls[x][y+1].value = cls[x][y].value+1;
		//mvprintw(y+1, x*5, "%2.1f",cls[x][y+1].value==10000 ? 9.9 : cls[x][y+1].value);
		nqprop[ncprop].x = x;
		nqprop[ncprop++].y = y+1;
	}

	if(cls[x-1][y-1].value==0){
		cls[x-1][y-1].value = cls[x][y].value+1.5;
		//mvprintw(y-1, (x-1)*5, "%2.1f",cls[x-1][y-1].value==10000 ? 9.9 : cls[x-1][y-1].value);
		nqprop[ncprop].x = x-1;
		nqprop[ncprop++].y = y-1;
	}
	if(cls[x+1][y-1].value==0){
		cls[x+1][y-1].value = cls[x][y].value+1.5;
		//mvprintw(y-1, (x+1)*5, "%2.1f",cls[x+1][y-1].value==10000 ? 9.9 : cls[x+1][y-1].value);
		nqprop[ncprop].x = x+1;
		nqprop[ncprop++].y = y-1;
	}
	if(cls[x+1][y+1].value==0){
		cls[x+1][y+1].value = cls[x][y].value+1.5;
		//mvprintw(y+1, (x+1)*5, "%2.1f",cls[x+1][y+1].value==10000 ? 9.9 : cls[x+1][y+1].value);
		nqprop[ncprop].x = x+1;
		nqprop[ncprop++].y = y+1;
	}
	if(cls[x-1][y+1].value==0){
		cls[x-1][y+1].value = cls[x][y].value+1.5;
		//mvprintw(y+1, (x-1)*5, "%2.1f",cls[x-1][y+1].value==10000 ? 9.9 : cls[x-1][y+1].value);
		nqprop[ncprop].x = x-1;
		nqprop[ncprop++].y = y+1;
	}
}

int pon(int x,int y){
	int many=0;
	for (int i=0;i<ippl;i++){
		if(ppl[i].in && ppl[i].pos.x==x && ppl[i].pos.y==y)
			many++;
	}
	return many;
}

void smove(int x,int y,float *low,struct point *c){
	if (cls[x][y].value<*low && !pon(x,y)){
		*low=cls[x][y].value;
		c->x=x;
		c->y=y;
	}
}

void pmove(struct person *p){
	struct point clow = {p->pos.x,p->pos.y};
	float low = cls[clow.x][clow.y].value;

	smove(p->pos.x-1,p->pos.y,&low,&clow);
	smove(p->pos.x,p->pos.y-1,&low,&clow);
	smove(p->pos.x+1,p->pos.y,&low,&clow);
	smove(p->pos.x,p->pos.y+1,&low,&clow);

	smove(p->pos.x-1,p->pos.y-1,&low,&clow);
	smove(p->pos.x+1,p->pos.y-1,&low,&clow);
	smove(p->pos.x+1,p->pos.y+1,&low,&clow);
	smove(p->pos.x-1,p->pos.y+1,&low,&clow);

	p->pos.x=clow.x;
	p->pos.y=clow.y;
}

bool onexit(int x,int y){
	for(int i=0;i<cexits;i++){
		if(x==exits[i].x && y==exits[i].y)
			return TRUE;
	}
	return FALSE;
}

void pshuf(){
	struct person t;
	for (int i=0;i<ippl;i++){
		int j = (rand()%(ippl-i))+i;
		t=ppl[i];
		ppl[i]=ppl[j];
		ppl[j]=t;
	}
}
