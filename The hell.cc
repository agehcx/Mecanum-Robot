#include<bits/stdc++.h>
using namespace std;
#define ll 	long long 
#define pii pair<int,int>
#define pb	push_back
#define CMP(i,j)	cout<<'{'<<i<<','<<j<<"}\n"
const int INF = 1e9+7;
const int length = 8;
const int width = 4;



struct Box {
    int N,W,S,E;
    char MID;
    /*
    W WHITE
    H BLACKHOLE
    R RED
    G GREEN
    B BLUE
    Y YELLOW
    1 SLOPE_UP
    2 SLOPE_TOP
    3 SLOPE_DOWN
    */
};

int field[13][25]={};
bool vis[13][25];
bool found = false;
vector<vector<Box>> v(width+1, vector<Box>(length+1));
int curx, cury;

int dfs(int i, int j, char x) {
	curx = i;
	cury = j;
	if(found) {
		return 0;
	}
	if (i < 0 || j < 0 || i >= width*3 || j >= length*3 || vis[i][j] == 1) {
		return 0;
	}
	if (field[i][j] == x) {
		found = true;
	}
	CMP(i,j);
	vis[i][j] = 1;
	field[i][j] = '#';
	dfs(i-1, j, x);
	dfs(i+1, j, x);
	dfs(i, j-1, x);
	dfs(i, j+1, x);
}

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);
    
    memset(field,0,sizeof(field));
    
	v[0][0] = {1,1,0,0,'W'};
	v[0][1] = {1,0,1,0,'W'};
	v[0][2] = {1,0,1,0,'3'};
	v[0][3] = {1,0,1,0,'2'};
	v[0][4] = {1,0,1,0,'1'};
	v[0][5] = {1,0,1,0,'W'};
	v[0][6] = {1,0,0,0,'W'};
	v[0][7] = {1,0,1,1,'Y'};
	
	v[1][0] = {0,1,0,1,'W'};
	v[1][1] = {1,1,1,0,'G'};
	v[1][2] = {1,0,0,1,'W'};
	v[1][3] = {1,1,0,0,'W'};
	v[1][4] = {1,0,1,0,'W'};
	v[1][5] = {1,0,0,0,'W'};
	v[1][6] = {0,0,0,1,'W'};
	v[1][7] = {1,1,0,1,'B'};
	
	v[2][0] = {0,1,1,1,'W'};
	v[2][1] = {1,1,1,1,'H'};
	v[2][2] = {0,1,1,0,'W'};
	v[2][3] = {0,0,1,1,'W'};
	v[2][4] = {1,1,1,1,'W'};
	v[2][5] = {0,1,0,1,'W'};
	v[2][6] = {0,1,0,1,'W'};
	v[2][7] = {0,1,0,1,'W'};
	
	v[3][0] = {1,1,1,0,'R'};
	v[3][1] = {1,0,1,0,'W'};
	v[3][2] = {1,0,1,0,'W'};
	v[3][3] = {1,0,1,0,'W'};
	v[3][4] = {1,0,1,0,'W'};
	v[3][5] = {0,0,1,1,'W'};
	v[3][6] = {0,1,1,0,'W'};
	v[3][7] = {0,0,1,1,'W'};
		
	cout<<"Rendering... \n";
	
	// MAP RENDER //
	for(int i=0; i<width; i++) {
		for(int j=0; j<length; j++) {
			cout<<"TC"<<" "<<i<<" "<<j<<"\n";
			int mi,mj;
			mi = (i)*3;
			mj = (j)*3;
			field[mi+0][mj+0] = true;
			field[mi+2][mj+0] = true;
			field[mi+0][mj+2] = true;
			field[mi+2][mj+2] = true;
			field[mi+0][mj+1] = (v[i][j].N==1?true:false);
			field[mi+1][mj+0] = (v[i][j].W==1?true:false);
			field[mi+2][mj+1] = (v[i][j].S==1?true:false);
			field[mi+1][mj+2] = (v[i][j].E==1?true:false);
			int vl;
			if(v[i][j].MID=='H'  ) vl = true;
			else if(v[i][j].MID=='W') {
				vl = false;
			}
			else {
				vl = v[i][j].MID+0;
			}
			field[mi+1][mj+1] = vl; 	
		}
	}	

	// PRINT //	
	for(int i=0; i<12; i++) {
		for(int j=0; j<24; j++) {
			if(field[i][j]==1) vis[i][j] = 1;
			if(field[i][j]!=0 && field[i][j]!=1 ) cout<<char(field[i][j]);
			else if(field[i][j]) cout<<'X';
			else cout<<'.';
		}
		cout<<'\n';
	}
	// DFS ON GRAPH //
	// START FROM (3,7) //
	dfs(10,1,'Y');
	
	for(int i=0; i<12; i++) {
		for(int j=0; j<24; j++) {
			if(field[i][j]!=1) cout<<char(field[i][j]);
			else cout<<vis[i][j];
		}
		cout<<'\n';
	}
	
}

