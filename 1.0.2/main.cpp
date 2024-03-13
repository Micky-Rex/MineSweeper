/*
Mine Sweeper
作者：MickyRex
2023.07.29 
*/
#include<ctime>
#include<conio.h>
#include<cstdlib>
#include<iostream>
#include<graphics.h>
using namespace std;
int mine_num=0,open=0,starttime=time(NULL);
MOUSEMSG mouse;	//鼠标指针类型消息 
const int zoom=2;
const int picturex=21,picturey=21;
const int N=100;	//数组最大边长 
int n=10,m=10;	//游玩时地图的宽高 
int window_width=n*(picturex*zoom);	//窗口宽度 
int window_height=m*(picturey*zoom);	//窗口高度 
int g[N][N];	//记录扫雷地图  
int menuh=20;
bool vis[N][N];	//记录是否打开 
bool mapflag[N][N];
bool win_flag=true;
const int dx[]={1,1,1,0,0,-1,-1,-1};
const int dy[]={-1,0,1,-1,1,-1,0,1};
IMAGE p0,p1,p2,p3,p4,p5,p6,p7,p8,pm,pn,pnd,pf; 	//存储图片素材 

void LoadPicture();	//导入图片素材 
void around();	//检查周围雷数 
int dfs(int x,int y);
void Paint();
void mapdfs(int x,int y);

int main(){
	system("title MineSweeper");
	srand(std::time(0));
	LoadPicture();
	around();
	initgraph(window_width,window_height+menuh);
	while(true){
		setbkcolor(WHITE); 
//		双缓冲清屏 
		BeginBatchDraw();
		cleardevice();
		Paint();
		settextcolor(BLACK);
		settextstyle(menuh-2,0,"./STXINWEI.TTF");
		outtextxy(0,0," 雷数：");
		outtextxy(55,0,mine_num/10+'0');
		outtextxy(65,0,mine_num%10+'0');
		if(!win_flag){
			FlushBatchDraw();
			cout<<'\a';
			if(MessageBox(GetForegroundWindow(),"GameOver","MineSweeper",5)!=4)break;
			memset(vis,false,sizeof(vis));
			memset(mapflag,false,sizeof(mapflag));
			win_flag=true;
			open=0;
		}else if(open==n*m-mine_num){
			FlushBatchDraw();
			cout<<'\a';
			MessageBox(GetForegroundWindow(),"Victory!","MineSweeper",1);
			break;
		}
		FlushBatchDraw();
	}
	closegraph();
	return 0;
}

void Paint(){
	bool flag;	//鼠标是否悬停于该方格的标记变量 
	ExMessage m;
	m=getmessage(EM_MOUSE);	//获取鼠标信息 
	for(int i=0;i<window_width;i+=(picturex*zoom)){
		for(int j=menuh;j<window_height+menuh;j+=(picturey*zoom)){
			flag=false;
			switch(m.message){
				case WM_MOUSEMOVE:	//当鼠标移动时 
					if(m.x>i and m.x<i+(picturex*zoom) and m.y>j and m.y<j+(picturey*zoom)) flag=true;
					break;
				case WM_LBUTTONDOWN:	//当鼠标左键按下时 
					if(m.x>i and m.x<i+(picturex*zoom) and m.y>j and m.y<j+(picturey*zoom)){
						if(!mapflag[i/(picturex*zoom)][j/(picturey*zoom)])mapdfs(i/(picturex*zoom),j/(picturey*zoom));	//深搜逐个方格展开 
						if(g[i/(picturex*zoom)][j/(picturey*zoom)]==-1 and vis[i/(picturex*zoom)][j/(picturey*zoom)]) win_flag=false;
					}
					break;
				case WM_RBUTTONDOWN:
					if(m.x>i and m.x<i+(picturex*zoom) and m.y>j and m.y<j+(picturey*zoom)){
						mapflag[i/(picturex*zoom)][j/(picturey*zoom)]=true-mapflag[i/(picturex*zoom)][j/(picturey*zoom)];
//						while(WM_RBUTTONDOWN);
					}
					break;
					
			}
			if(vis[i/(picturex*zoom)][j/(picturey*zoom)]){
				//在 i , j 坐标贴图
				switch(g[i/(picturex*zoom)][j/(picturey*zoom)]){
					case 0:
						putimage(i,j,&p0);
						break;
					case 1:
						putimage(i,j,&p1);
						break;
					case 2:
						putimage(i,j,&p2);
						break;
					case 3:
						putimage(i,j,&p3);
						break;
					case 4:
						putimage(i,j,&p4);
						break;
					case 5:
						putimage(i,j,&p5);
						break;
					case 6:
						putimage(i,j,&p6);
						break;
					case 7:
						putimage(i,j,&p7);
						break;
					case 8:
						putimage(i,j,&p8);
						break;
					case -1:
						putimage(i,j,&pm);
						break;
				}
			}else{
				if(flag) putimage(i,j,&pnd);
				else putimage(i,j,&pn);
				if(mapflag[i/(picturex*zoom)][j/(picturey*zoom)]) putimage(i,j,&pf);
			}
		}
	}
	return;
}

void around(){
	//随机地图
	double maxnum=5;
	for(int i=0;i<n;i++){
		for(int j=0;j<m;j++){
			if(rand()%30<maxnum){
				g[i][j]=-1;
				mine_num++;
				maxnum-=0.5;
				if(i==0 and j==0)g[i][j]=0;
				else if(i==n-1 and j==0)g[i][j]=0;
				else if(i==0 and j==m-1)g[i][j]=0;
				else if(i==n-1 and j==m-1)g[i][j]=0;
			}else g[i][j]=0,maxnum+=0.02;
		}
	}
	//对所有方格位置逐个搜索，标记周围雷数
	for(int i=0;i<n;i++) for(int j=0;j<m;j++) if(g[i][j]!=-1) g[i][j]=dfs(i,j);
	return;
}

void mapdfs(int x,int y){
	//递归出口 1
	if(vis[x][y]==false) open++,vis[x][y]=true;
	else return;
	//递归出口 2
	if(g[x][y]!=0) return;
	//向四周递归 
	if(x>0)mapdfs(x-1,y);
	if(y>0)mapdfs(x,y-1);
	if(x<n-1)mapdfs(x+1,y);
	if(y<m-1)mapdfs(x,y+1);
}

int dfs(int x,int y){
	//遍历周围 8个方格，标记雷数
	int nx,ny;
	int ans=0;
	for(int i=0;i<8;i++){
		nx=x+dx[i];
		ny=y+dy[i];
		if(nx<0 or ny<0 or nx>n-1 or ny>m-1) continue;
		if(g[nx][ny]==-1) ans++;
	}
	return ans;
}


void LoadPicture(){
	memset(vis,false,sizeof(vis));
	memset(mapflag,false,sizeof(mapflag));
	int px=picturex+1,py=picturey+1;
	int w=px*zoom,h=py*zoom;	//        长宽像素数
	//导入图片
	loadimage(&p0,_T("./Picture/0.png"),w,h);
	loadimage(&p1,_T("./Picture/1.png"),w,h);
	loadimage(&p2,_T("./Picture/2.png"),w,h);
	loadimage(&p3,_T("./Picture/3.png"),w,h);
	loadimage(&p4,_T("./Picture/4.png"),w,h);
	loadimage(&p5,_T("./Picture/5.png"),w,h);
	loadimage(&p6,_T("./Picture/6.png"),w,h);
	loadimage(&p7,_T("./Picture/7.png"),w,h);
	loadimage(&p8,_T("./Picture/8.png"),w,h);
	loadimage(&pm,_T("./Picture/mine.png"),w,h);
	loadimage(&pf,_T("./Picture/flag.png"),w,h);
	loadimage(&pn,_T("./Picture/unexplored.png"),w,h);
	loadimage(&pnd,_T("./Picture/unexplored2.png"),w,h);
	return;
}
