#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <ncurses.h>
#include <stdbool.h>
#include <time.h>

#define WALL_VALUE 10000
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

float beta=.5;
/* beta=antisociality factor */

void propagate(int x,int y);
void pmove(struct person *p);
void pmoveesc(struct person *p);
bool onexit(int x,int y);
void pshuf();
bool paround(int x,int y);

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
unsigned int counter=0;
bool skip = false;
bool iniskip = false;
char* filename = NULL;

int main(int argc,char *argv[]){
    if (argc<2){
		printf("Floor field with pressure model\n"
            "Usage\n"
            "%s [options] <file>\n"
            "Options:\n"
            "\t<file>\t\t\tfile containing the simulation template\n"
            "\t-s\t\t\t\tenable skip mode - don't draw\n"
            "\t-b <0.0-1.0>\tbeta factor - antisociality, floating point number from 0 to 1 (default .5)\n\n"
            "Template file format\n"
            "can only contain characters:\n"
            "\t#\t\tfor wall\n"
            "\tp\t\tfor person\n"
            "\te\t\tfor exit\n"
            "\tspace\tfor air\n"
            "The room containing the exits has to be completely closed, with sharp corners, example:\n"
            "\t####\n"
            "\t#p ##\n"
            "\t#p e#\n"
            "\t#####\n",argv[0]);
		return 1;
	}
	
    for (int i=1;i<argc;i++){
        if(argv[i][0]=='-'){
            switch (argv[i][1]){
                case 's':
                    skip=true;
                    iniskip=true;
                    break;
                case 'b':
                    beta = atof(argv[++i]);
                    break;
                default:
                    printf("Usage\n"
                        "%s [options] <file>\n"
                        "Options:\n"
                        "\t<file>\t\t\tfile containing the simulation template\n"
                        "\t-s\t\t\t\tenable skip mode - don't draw\n"
                        "\t-b <0.0-1.0>\tbeta factor - antisociality, floating point number from 0 to 1 (default .5)\n\n"
                        "Template file format\n"
                        "can only contain characters:\n"
                        "\t#\t\tfor wall\n"
                        "\tp\t\tfor person\n"
                        "\te\t\tfor exit\n"
                        "\tspace\tfor air\n"
                        "The room containing the exits has to be completely closed, with sharp corners, example:\n"
                        "\t####\n"
                        "\t#p ##\n"
                        "\t#p e#\n"
                        "\t#####\n",argv[0]);
		                return 1;
            }
        } else {
            filename = argv[i];
        }
    }
	if (filename==NULL) return 2;
	FILE *f;
	f = fopen(filename, "r");
	if (!f) return 1;
	int c;
	int cline=0,crow=0,mrow=0;
	while((c=getc(f))!=EOF){
		switch (c) {
			case ' ':
				cls[crow++][cline].value = 0;
				break;
			case '#':
				cls[crow++][cline].value = WALL_VALUE;
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
				printf("illegal character in file");
				return 3;
		}
	}
	fclose(f);
	if(cprop<1){
		printf("no exits in simulation template");
		return 4;
	}
	for (int i=0;i<cprop;i++){
		exits[i]=qprop[i];
	}
	cexits=cprop;
	time_t t;
	srand((unsigned) time(&t));
	for (int i=0;i<ippl;i++){
		ppl[i].in=true;
	}
    if (!skip){
        initscr();
        start_color();
        init_pair(1, COLOR_GREEN, COLOR_BLACK);
        init_pair(2, COLOR_RED, COLOR_BLACK);
        noecho();
        curs_set(0);
    }
	/* propagate floor field values */
	while(cprop){
		for (int i=0;i<cprop;i++){
			propagate(qprop[i].x,qprop[i].y);
			// for (int iy=0;iy<cline+1;iy++){
			// 	for (int ix=0;ix<mrow;ix++){
			// 		if (cls[ix][iy].value==WALL_VALUE)
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
	/* draw walls and fill empty space with spaces */
	for (int iy=0;iy<cline+1;iy++){
		for (int ix=0;ix<mrow;ix++){
			mvaddch(iy,ix,cls[ix][iy].value==WALL_VALUE ? '#' : ' ');
			// mvprintw(iy, ix*5, "%2.1f",cls[ix][iy].value==WALL_VALUE ? 9.9 : cls[ix][iy].value);
			// mvaddch(iy, ix, cls[ix][iy].value==WALL_VALUE ? '#' : ' ');
		}
	}
	/* draw people */
	for (int i=0;i<ippl;i++){
		attron(COLOR_PAIR(2));
		mvaddch(ppl[i].pos.y, ppl[i].pos.x,'p');
		attroff(COLOR_PAIR(2));
	}
	/* draw exits */
	for (int i=0;i<cexits;i++){
		attron(COLOR_PAIR(1));
		mvaddch(exits[i].y, exits[i].x,'e');
		attroff(COLOR_PAIR(1));
	}

  mvprintw(getmaxy(stdscr)-5,0,"floor field with pressure\n"
  "antisociality beta=%f\n"
  "any key progresses the simulation once, 's' skips to the results",beta);
	/* simulation loop */
	while (true){
        if (!skip){
            refresh();
        }
		if (exited==ippl) break;
        if (!skip && ((c=getch())==EOF || c=='s')){
            //skip the visual part of simulating
            skip=true;
        }
		// usleep(250000);
		pshuf();
        /* draw spaces over people */
        for (int i=0;i<ippl;i++){
			if(ppl[i].in){
				mvaddch(ppl[i].pos.y,ppl[i].pos.x,' ');
            }
        }
		/* move people */
		for (int i=0;i<ippl;i++){
			if(ppl[i].in){
                /* when people around */
                if(paround(ppl[i].pos.x,ppl[i].pos.y)){
                    /* beta-based randomization here */
				    if ((float)rand()/(float)RAND_MAX>beta){
                        pmove(&ppl[i]);
                    }else{
                        pmoveesc(&ppl[i]);
                    }
                /* when no people around */
                } else {
                    pmove(&ppl[i]);
                }
			}
		}
		/* remove people standing on exits */
		for (int i=0;i<ippl;i++){
			if (ppl[i].in && onexit(ppl[i].pos.x,ppl[i].pos.y)){
				exited++;
				ppl[i].in=FALSE;
			}
		}

        counter++;

        if (!skip){
            /* draw people */
            attron(COLOR_PAIR(2));
            for (int i=0;i<ippl;i++){
                if(ppl[i].in){
                    mvaddch(ppl[i].pos.y,ppl[i].pos.x,'p');
                }
            }
            attroff(COLOR_PAIR(2));
            /* redraw all exits */
            for (int i=0;i<cexits;i++){
                attron(COLOR_PAIR(1));
                mvaddch(exits[i].y, exits[i].x,'e');
                attroff(COLOR_PAIR(1));
            }
            mvprintw(getmaxy(stdscr)-2,0,"steps done:%d",counter);
            mvprintw(getmaxy(stdscr)-1, 0, "%d/%d",exited,ippl);
        }
	}
    if(!iniskip){
        if (skip){
            mvprintw(getmaxy(stdscr)-2,0,"steps done:%d",counter);
            mvprintw(getmaxy(stdscr)-1, 0, "%d/%d",exited,ippl);
        }
        mvprintw(getmaxy(stdscr)-3,0,"\n");
        mvprintw(getmaxy(stdscr)-3,0,"press q to exit");
        mvprintw(getmaxy(stdscr)-2,0,"simulation completed in %d",counter);
        refresh();
        while((c=getch())!='q');
    } else{
        printf("%d\n",counter);
    }
	endwin();
}

/* propagate floor values to tiles adjacent to this xy */
void propagate(int x,int y){
	if(cls[x-1][y].value==0){
		cls[x-1][y].value = cls[x][y].value+1;
		//mvprintw(y, (x-1)*5, "%2.1f",cls[x-1][y].value==WALL_VALUE ? 9.9 : cls[x-1][y].value);
		nqprop[ncprop].x = x-1;
		nqprop[ncprop++].y = y;
	}
	if(cls[x][y-1].value==0){
		cls[x][y-1].value = cls[x][y].value+1;
		//mvprintw(y-1, x*5, "%2.1f",cls[x][y-1].value==WALL_VALUE ? 9.9 : cls[x][y-1].value);
		nqprop[ncprop].x = x;
		nqprop[ncprop++].y = y-1;
	}
	if(cls[x+1][y].value==0){
		cls[x+1][y].value = cls[x][y].value+1;
		//mvprintw(y, (x+1)*5, "%2.1f",cls[x+1][y].value==WALL_VALUE ? 9.9 : cls[x+1][y].value);
		nqprop[ncprop].x = x+1;
		nqprop[ncprop++].y = y;
	}
	if(cls[x][y+1].value==0){
		cls[x][y+1].value = cls[x][y].value+1;
		//mvprintw(y+1, x*5, "%2.1f",cls[x][y+1].value==WALL_VALUE ? 9.9 : cls[x][y+1].value);
		nqprop[ncprop].x = x;
		nqprop[ncprop++].y = y+1;
	}

	if(cls[x-1][y-1].value==0){
		cls[x-1][y-1].value = cls[x][y].value+1.5;
		//mvprintw(y-1, (x-1)*5, "%2.1f",cls[x-1][y-1].value==WALL_VALUE ? 9.9 : cls[x-1][y-1].value);
		nqprop[ncprop].x = x-1;
		nqprop[ncprop++].y = y-1;
	}
	if(cls[x+1][y-1].value==0){
		cls[x+1][y-1].value = cls[x][y].value+1.5;
		//mvprintw(y-1, (x+1)*5, "%2.1f",cls[x+1][y-1].value==WALL_VALUE ? 9.9 : cls[x+1][y-1].value);
		nqprop[ncprop].x = x+1;
		nqprop[ncprop++].y = y-1;
	}
	if(cls[x+1][y+1].value==0){
		cls[x+1][y+1].value = cls[x][y].value+1.5;
		//mvprintw(y+1, (x+1)*5, "%2.1f",cls[x+1][y+1].value==WALL_VALUE ? 9.9 : cls[x+1][y+1].value);
		nqprop[ncprop].x = x+1;
		nqprop[ncprop++].y = y+1;
	}
	if(cls[x-1][y+1].value==0){
		cls[x-1][y+1].value = cls[x][y].value+1.5;
		//mvprintw(y+1, (x-1)*5, "%2.1f",cls[x-1][y+1].value==WALL_VALUE ? 9.9 : cls[x-1][y+1].value);
		nqprop[ncprop].x = x-1;
		nqprop[ncprop++].y = y+1;
	}
}

/* is there a person on xy */
bool pon(int x,int y){
	for (int i=0;i<ippl;i++){
		if(ppl[i].in && ppl[i].pos.x==x && ppl[i].pos.y==y)
			return true;
	}
	return false;
}
/* are there people around xy */
bool paround(int x,int y){
  if (pon(x-1,y)) return true;
  if (pon(x,y-1)) return true;
  if (pon(x+1,y)) return true;
  if (pon(x,y+1)) return true;

  if (pon(x-1,y-1)) return true;
  if (pon(x+1,y-1)) return true;
  if (pon(x+1,y+1)) return true;
  if (pon(x-1,y+1)) return true;
  
	return false;
}
/* are there people around xy */
unsigned int paroundc(int x,int y){
  unsigned int many=0;
  if (pon(x-1,y)) many++;
  if (pon(x,y-1)) many++;
  if (pon(x+1,y)) many++;
  if (pon(x,y+1)) many++;

  if (pon(x-1,y-1)) many++;
  if (pon(x+1,y-1)) many++;
  if (pon(x+1,y+1)) many++;
  if (pon(x-1,y+1)) many++;
  
	return many;
}

/* one check of logically moving the person */
void smove(int x,int y,float *low,struct point *c){
	if (cls[x][y].value<*low && !pon(x,y)){
		*low=cls[x][y].value;
		c->x=x;
		c->y=y;
	}
}
/* logically simulation step a person */
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

/* one check of moving a person away from other people */
void smoveesc(int x,int y,unsigned int *low,struct point *c){
	if (cls[x][y].value<WALL_VALUE && !pon(x,y) && paround(x,y)<*low){
		*low=cls[x][y].value;
		c->x=x;
		c->y=y;
	}
}
/* antisocially simulation step a person */
void pmoveesc(struct person *p){
	struct point clow = {p->pos.x,p->pos.y};
	unsigned int low = paroundc(p->pos.x,p->pos.y);

	smoveesc(p->pos.x-1,p->pos.y,&low,&clow);
	smoveesc(p->pos.x,p->pos.y-1,&low,&clow);
	smoveesc(p->pos.x+1,p->pos.y,&low,&clow);
	smoveesc(p->pos.x,p->pos.y+1,&low,&clow);

	smoveesc(p->pos.x-1,p->pos.y-1,&low,&clow);
	smoveesc(p->pos.x+1,p->pos.y-1,&low,&clow);
	smoveesc(p->pos.x+1,p->pos.y+1,&low,&clow);
	smoveesc(p->pos.x-1,p->pos.y+1,&low,&clow);

	p->pos.x=clow.x;
	p->pos.y=clow.y;
}

/* are these xy on an exit */
bool onexit(int x,int y){
	for(int i=0;i<cexits;i++){
		if(x==exits[i].x && y==exits[i].y)
			return TRUE;
	}
	return FALSE;
}

/* shuffle people */
void pshuf(){
	struct person t;
	for (int i=0;i<ippl;i++){
		int j = (rand()%(ippl-i))+i;
		t=ppl[i];
		ppl[i]=ppl[j];
		ppl[j]=t;
	}
}
