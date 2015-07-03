#include <windows.h>
#include "resource.h"

#include <MATH.H>
#include <TIME.H>
#include <string.h>
#include <stdio.h>



///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                                      数据                                                             //
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////




//*****************************************************//
//                      全局变量                       //

static int mine_col_num,mine_line_num,mine_num,timecount,minecount,zerocount,facestate,timerstate,gamestate;
    //mine_col_num       雷区横向个数
    //mine_line_num      雷区纵向个数
    //mine_num           雷数
    //timecount          时间计数
    //minecount          雷计数
    //zerocount          已翻开空格数
static int level ;
static   RECT Mrect;
static   int cellnum;


    


//facestate 表情状态

#define smileface_down   0;
#define niubilityface    1;
#define failface         2;
#define clickface        3;
#define smileface_up     4;

//gamestate 比赛状态
#define ppregame      0;
#define pregame       1;
#define nowgame       2;
#define endgame       3;





//*****************************************************//
//                 界面布局参数                        //

#define MINE_BITMAP_WIDTH	16
#define MINE_BITMAP_HEIGHT	16
#define FACE_BITMAP_WIDTH	24
#define FACE_BITMAP_HEIGHT	24
#define NUM_BITMAP_WIDTH	13
#define NUM_BITMAP_HEIGHT	23	
    //定义地雷、表情、时间数字的长宽

#define MAIN_LEFTSTND		12
#define MAIN_TOPSTND		55
#define MAIN_RIGHTSTND		8
#define MAIN_BOTTOMSTND		8


#define COUNT_LEFTPOS		17		
#define COUNT_TOPPOS		16
#define COUNT_RIGHTPOS		56
#define COUNT_BOTTOMPOS		39

#define TIME_TOPPOS			16
#define TIME_RIGHTSTND		15
#define TIME_WIDTH			39
#define TIME_HEIGHT			23



#define PRIMARY_COL_NUM  	9
#define PRIMARY_LN_NUM	    9
#define PRIMARY_MINE_NUM	10

#define INTERMEDIATE_COL_NUM  	16
#define INTERMEDIATE_LN_NUM	    16
#define INTERMEDIATE_MINE_NUM	40

#define ADVANCED_COL_NUM  	30
#define ADVANCED_LN_NUM	    16
#define ADVANCED_MINE_NUM	99

#define FUN_LEFTPOS			12		
#define FUN_TOPPOS			12
#define FUN_RIGHTSTND		8
#define FUN_BOTTOMSTND		12

#define LINE_LIGHTEN_THICK		3
#define LINE_LIGHTEN_NORMAL		2
#define LINE_LIGHTEN_THIN		1	
    //定义低级、中级、高级行数、列数、雷数



 
//*****************************************************//
//     定义雷区结构体并声明数据来描述雷区单元状态      //
 struct cellofmine
{
	UINT pos_line;            //哪一行
    UINT pos_coloum;          //哪一列
	UINT cur_state;           //当前状态
	UINT cell_attrib;         //每个单元属性
	UINT old_state;           //历史状态
	UINT special_state;       //特殊状态
	UINT expand_state;        //翻开状态
	
};
static cellofmine cellmine[40][40];

#define up              0
#define flag            3
#define question        2
#define normal          1

#define STATE_NORMAL			0		//正常
#define STATE_FLAG				1		//标志有雷
#define STATE_DICEY				2		//未知状态
#define STATE_BLAST				3		//踩雷了
#define STATE_ERROR				4		//标错雷了
#define STATE_MINE				5		//是雷
#define STATE_DICEY_DOWN		6		//玩家不确定

#define STATE_NUM8				7		//周围有8雷 
#define STATE_NUM7				8
#define STATE_NUM6				9
#define STATE_NUM5				10
#define STATE_NUM4				11
#define STATE_NUM3				12
#define STATE_NUM2				13
#define STATE_NUM1				14
#define STATE_EMPTY				15		//无雷

#define ATTRIB_EMPTY			0		//非雷
#define ATTRIB_MINE				1		//雷
//*****************************************************//



//*****************************************************//
//     定义鼠标状态结构体并声明数据来描述鼠标状态      // 
struct MouseState
{
	UINT mouse_pos_line;       //鼠标在哪一行
	UINT mouse_pos_coloum;     //鼠标在哪一列
	UINT mouse_area;           //鼠标在哪个区域
	UINT mouse_button;         //鼠标按键情况
};
static MouseState mState; 


#define 	  LBUTTONUP		 1    //左键弹起
#define 	  LBUTTONDOWN	 2	  //左键按下
#define		  RBUTTONDOWN	 3	  //右键按下
#define		  DBBUTTONDOWN	 4    //双键按下
#define		  DBBUTTONUP	 5    //双键弹起
#define		  MOUSEMOVING	 6    //鼠标移动

#define       MOUSEINFACE    1    //鼠标在表情区域
#define       MOUSEINMINE    2    //鼠标在雷区
#define       MOUSEINOTHER   3    //鼠标在其他区域
   





///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                                     函数                                                              //
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

LRESULT CALLBACK WndProc (HWND, UINT, WPARAM, LPARAM) ;             //主回调函数
BOOL CALLBACK AboutDlgProc (HWND, UINT, WPARAM, LPARAM) ;           //帮助对话框回调函数
BOOL CALLBACK RecordDlgProc (HWND, UINT, WPARAM, LPARAM) ;          //记录对话框回调函数



BOOL initializeinfo(HWND hwnd,int level);                           //利用级数初始化关键参数

RECT getlevelwindowsize(HWND hwnd);                                 //获取不同等级的窗口大小及位置

RECT CreateRect(int left,int top,int right,int bottom);             //用于建立矩形区域

BOOL DrawSolidFrame(HDC hDC,RECT rcFrame,int iLighten);             //画边框

BOOL LayMines(HWND hwnd);                                           //布雷

MouseState GetMouseState(HWND hwnd,HINSTANCE hInstance,int MouseXPos,int MouseYPos,int MouseSta);

BOOL ChangeFace (HWND hwnd,int mface);

BOOL LbuttondownFun(HWND hwnd);

BOOL LbuttonupFun(HWND hwnd);

BOOL RbuttondownFun(HWND hwnd);

BOOL DbbuttondownFun(HWND hwnd);

BOOL DbbuttonupFun(HWND hwnd);

BOOL MoveFun(HWND hwnd);

BOOL updateminecount(HWND hwnd);



BOOL BlipFun(HWND hwnd,int m,int n);

BOOL ExpandMine (HWND hwnd, int m, int n);

BOOL showdomain(HWND hwnd,int dx,int dy);