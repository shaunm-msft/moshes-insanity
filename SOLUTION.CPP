/********************************************************************
 *                                                                  *
 *   Source File: Solution.cpp                                      *
 *   Reads the puzzle from a file called "moshe.dat" and solves it  *
 *                                                                  *
 *   I apologize for the extremely messy and poorly documented code *
 *   here, but it was written in competition style, not in "normal" *
 *   circumstances.                                                 *
 *                                                                  *
 *   by Shaun Miller, 1994                                          *
 *                                                                  *
 ********************************************************************/


#include <string.h>
#include <ctype.h>
#include <stdlib.h>
#include <time.h>
#include "CCR.h"
#include "dlist.h"

char cube[8][6];
char corner[8][8][3]; // L,R,T
int bottom[4][3];   // cube,corner,rotation
int top[4][3];
void firstgame (void);
void nextgame (void);


// everything else here has file scope

static char possiblecube[30][7] = {
	"RGBCMY","RGCBMY","RCGBMY","RCBGMY","RBCGMY","RBGCMY",
	"RGBMCY","RGMBCY","RMGBCY","RMBGCY","RBMGCY","RBGMCY",
	"RGCMBY","RGMCBY","RMGCBY","RMCGBY","RCMGBY","RCGMBY",
	"RBCMGY","RBMCGY","RMBCGY","RMCBGY","RCMBGY","RCBMGY",
	"GBCMRY","GBMCRY","GMBCRY","GMCBRY","GCMBRY","GCBMRY"
};

static const char fname[] = "moshe.dat";
static const int linesize = 128;

static int codifiedcorner[8][8];

static char *facecolor[6];

static char position[6][3];

static char line[8][linesize];

static char colors[26];
	static const int R = 1;
	static const int G = 2;
	static const int B = 4;
	static const int C = 8;
	static const int M = 16;
	static const int Y = 32;

static DoubleList<CCR> opposite,*valid[6];

static int cubetaken[8];
static CCR mp[6];


static int process (void);
static void MakeCorners (void);
static int complimentary (int c1, int cor1, int c2, int cor2);
static void MakeOppositeList (int corner);
static void MakeValidList (void);
static int IsValidPosition (int p);
static void SetFaceColors (void);
static int match (void);


void firstgame (void)
{
	for (int i=0; i<6; i++) valid[i] = new DoubleList<CCR>;
	time_t t;
	srand((unsigned)time(&t)+15);
	nextgame();
	return;
}
	
void nextgame (void)
{
	int j;
	int solved=0;
   while (!solved) {
		for (int i=0; i<8; i++) {
			j = (rand())%30;
			memmove(cube[i],possiblecube[j],6);
		}
		solved = process();
	}
	return;
}

int process (void)
{
	int i,j,solved;
	CCR cc, *t;

	memset(colors,Y,26);
		colors['R'] = R;
		colors['G'] = G;
		colors['B'] = B;
		colors['C'] = C;
		colors['M'] = M;
		colors['Y'] = Y;

	MakeCorners();

	bottom[0][0]=0;
	bottom[0][2]=3;
	solved=0;

	for (bottom[0][1]=0; bottom[0][1]<8; bottom[0][1]++) {
		while(opposite.Size()) delete opposite.RemoveCurrent();
		MakeOppositeList(bottom[0][1]);
		for (i=opposite.Size(); i && !solved; i--,opposite++) {
			cc = *(t=opposite.Current());
         delete t;
			top[0][0] = cc.cube;
			top[0][1] = cc.corner;
			for (top[0][2]=1; top[0][2]<=3; top[0][2]++) {
				SetFaceColors();
				for (j=0; j<6; j++) {
					while (valid[j]->Size()) delete valid[j]->RemoveCurrent();
				}
				MakeValidList();
				if (match()) {
					solved = 1;
					break;
				}
			}
		}
		if (solved) break;
	}

	for (i=0; i<3; i++) {
		bottom[i+1][0] = mp[i].cube;
		bottom[i+1][1] = mp[i].corner;
		bottom[i+1][2] = mp[i].rotation;
		top[i+1][0] = mp[i+3].cube;
		top[i+1][1] = mp[i+3].corner;
		top[i+1][2] = mp[i+3].rotation;
	}


	return solved;
}

void MakeCorners (void)
{
	for (int i=0; i<8; i++) {
		corner[i][0][0] = cube[i][1];
		corner[i][0][1] = cube[i][0];
		corner[i][0][2] = cube[i][4];
		codifiedcorner[i][0] =
			colors[cube[i][0]] | colors[cube[i][1]] | colors[cube[i][4]];

		corner[i][1][0] = cube[i][2];
		corner[i][1][1] = cube[i][1];
		corner[i][1][2] = cube[i][4];
		codifiedcorner[i][1] =
			colors[cube[i][1]] | colors[cube[i][2]] | colors[cube[i][4]];

		corner[i][2][0] = cube[i][3];
		corner[i][2][1] = cube[i][2];
		corner[i][2][2] = cube[i][4];
		codifiedcorner[i][2] =
			colors[cube[i][2]] | colors[cube[i][3]] | colors[cube[i][4]];

		corner[i][3][0] = cube[i][0];
		corner[i][3][1] = cube[i][3];
		corner[i][3][2] = cube[i][4];
		codifiedcorner[i][3] =
			colors[cube[i][0]] | colors[cube[i][3]] | colors[cube[i][4]];


		corner[i][4][0] = cube[i][0];
		corner[i][4][1] = cube[i][1];
		corner[i][4][2] = cube[i][5];
		codifiedcorner[i][4] =
			colors[cube[i][0]] | colors[cube[i][1]] | colors[cube[i][5]];

		corner[i][5][0] = cube[i][1];
		corner[i][5][1] = cube[i][2];
		corner[i][5][2] = cube[i][5];
		codifiedcorner[i][5] =
			colors[cube[i][1]] | colors[cube[i][2]] | colors[cube[i][5]];

		corner[i][6][0] = cube[i][2];
		corner[i][6][1] = cube[i][3];
		corner[i][6][2] = cube[i][5];
		codifiedcorner[i][6] =
			colors[cube[i][2]] | colors[cube[i][3]] | colors[cube[i][5]];

		corner[i][7][0] = cube[i][3];
		corner[i][7][1] = cube[i][0];
		corner[i][7][2] = cube[i][5];
		codifiedcorner[i][7] =
			colors[cube[i][0]] | colors[cube[i][3]] | colors[cube[i][5]];

	}
	return;
}

int complimentary (int c1, int cor1, int c2, int cor2)
{
	return !(codifiedcorner[c1][cor1] & codifiedcorner[c2][cor2]);
}

void MakeOppositeList (int corner)
{
	CCR cc;
	cc.rotation = 3;
	for (cc.cube=1; cc.cube<8; cc.cube++) {
		for (cc.corner=0; cc.corner<8; cc.corner++) {
			if (complimentary(0,corner,cc.cube,cc.corner)) {
				opposite.Append(cc);
			}
		}
	}
	return;
}

void MakeValidList (void)
{
	int p,x,ok;
	CCR cc;

	for (p=0; p<6; p++) {
		for (cc.cube=1; cc.cube<8; cc.cube++) {
			if (cc.cube==top[0][0]) continue;
			for (cc.corner=0; cc.corner<8; cc.corner++) {
				for (cc.rotation=1,ok=0; cc.rotation<=3 && !ok; cc.rotation++) {
					for (x=0; x<3; x++) {
						position[p][x]=corner[cc.cube][cc.corner][(cc.rotation+x)%3];
					}
					ok = IsValidPosition(p);
					if (ok) break;
				}
				if (ok) valid[p]->Append(cc);
			}
		}
	}
	return;
}

int IsValidPosition (int pos)
{
	switch (pos) {
		case 0:	return  (position[0][2]==*facecolor[5]
							&& position[0][1]==*facecolor[0]
							&& position[0][0]==*facecolor[1]
					);
		case 1:	return  (position[1][2]==*facecolor[5]
							&& position[1][1]==*facecolor[1]
							&& position[1][0]==*facecolor[2]
					);

		case 2:	return  (position[2][2]==*facecolor[5]
							&& position[2][1]==*facecolor[2]
							&& position[2][0]==*facecolor[3]
					);

		case 3:	return  (position[3][2]==*facecolor[4]
							&& position[3][1]==*facecolor[1]
							&& position[3][0]==*facecolor[0]
					);

		case 4:	return  (position[4][2]==*facecolor[4]
							&& position[4][1]==*facecolor[0]
							&& position[4][0]==*facecolor[3]
					);

		case 5:	return  (position[5][2]==*facecolor[4]
							&& position[5][1]==*facecolor[3]
							&& position[5][0]==*facecolor[2]
					);
	}
	return 0;
}

void SetFaceColors (void)
{
	facecolor[0] = &corner[bottom[0][0]][bottom[0][1]][bottom[0][2]%3];
	facecolor[1] = &corner[top[0][0]][top[0][1]][top[0][2]%3];
	facecolor[2] = &corner[top[0][0]][top[0][1]][(1+top[0][2])%3];
	facecolor[3] = &corner[bottom[0][0]][bottom[0][1]][(1+bottom[0][2])%3];
	facecolor[4] = &corner[top[0][0]][top[0][1]][(2+top[0][2])%3];
	facecolor[5] = &corner[bottom[0][0]][bottom[0][1]][(2+bottom[0][2])%3];
};

int match (void)
{
	int p,p1,matched;
   int i,j;
	CCR cc,fcc,cc1,cc2, *t;

	for (p=0; p<6; p++) if (!valid[p]->Size()) return 0;

	memset(cubetaken,0,8*sizeof(int));
	cubetaken[top[0][0]]=cubetaken[bottom[0][0]]=1;
	for (p=0; p<6; p++) mp[p].cube = 8;

	for (p=0; p<6; p++) {
		i = valid[p]->Size();
      do {
			cc = *(t=valid[p]->Current());
			delete t;
         (*valid[p])++;
         --i;
      } while (i && cubetaken[cc.cube]);
		if (!cubetaken[cc.cube]) {
			cubetaken[cc.cube]=1;
			mp[p]=cc;
		}
	}

	matched=1;
	for (p=0; p<6 && matched; p++) {
		if (mp[p].cube<8) continue;
		matched=0;
		i = valid[p]->Size();
      while (i && !matched) {
			cc1 = *(t=valid[p]->Current());
			delete t;
			(*valid[p])++;
			for (p1=0; p1<5 && cc1.cube!=mp[p1].cube; p1++);
			j = valid[p1]->Size();
         while (j && !matched) {
				cc2 = *(t=valid[p1]->Current());
   	      delete t;
				(*valid[p1])++;
				if (!cubetaken[cc2.cube]) {
					mp[p] = cc1;
					mp[p1] = cc2;
					cubetaken[cc2.cube]=1;
					matched=1;
				}
				--j;
			} 
		--i;
		}
	}

	return matched;
}
