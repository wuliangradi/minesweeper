#include <windows.h>
#include "resource.h"

#include <MATH.H>
#include <TIME.H>
#include <string.h>
#include <stdio.h>



///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                                      ����                                                             //
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////




//*****************************************************//
//                      ȫ�ֱ���                       //

static int mine_col_num,mine_line_num,mine_num,timecount,minecount,zerocount,facestate,timerstate,gamestate;
    //mine_col_num       �����������
    //mine_line_num      �����������
    //mine_num           ����
    //timecount          ʱ�����
    //minecount          �׼���
    //zerocount          �ѷ����ո���
static int level ;
static   RECT Mrect;
static   int cellnum;


    


//facestate ����״̬

#define smileface_down   0;
#define niubilityface    1;
#define failface         2;
#define clickface        3;
#define smileface_up     4;

//gamestate ����״̬
#define ppregame      0;
#define pregame       1;
#define nowgame       2;
#define endgame       3;





//*****************************************************//
//                 ���沼�ֲ���                        //

#define MINE_BITMAP_WIDTH	16
#define MINE_BITMAP_HEIGHT	16
#define FACE_BITMAP_WIDTH	24
#define FACE_BITMAP_HEIGHT	24
#define NUM_BITMAP_WIDTH	13
#define NUM_BITMAP_HEIGHT	23	
    //������ס����顢ʱ�����ֵĳ���

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
    //����ͼ����м����߼�����������������



 
//*****************************************************//
//     ���������ṹ�岢��������������������Ԫ״̬      //
 struct cellofmine
{
	UINT pos_line;            //��һ��
    UINT pos_coloum;          //��һ��
	UINT cur_state;           //��ǰ״̬
	UINT cell_attrib;         //ÿ����Ԫ����
	UINT old_state;           //��ʷ״̬
	UINT special_state;       //����״̬
	UINT expand_state;        //����״̬
	
};
static cellofmine cellmine[40][40];

#define up              0
#define flag            3
#define question        2
#define normal          1

#define STATE_NORMAL			0		//����
#define STATE_FLAG				1		//��־����
#define STATE_DICEY				2		//δ֪״̬
#define STATE_BLAST				3		//������
#define STATE_ERROR				4		//�������
#define STATE_MINE				5		//����
#define STATE_DICEY_DOWN		6		//��Ҳ�ȷ��

#define STATE_NUM8				7		//��Χ��8�� 
#define STATE_NUM7				8
#define STATE_NUM6				9
#define STATE_NUM5				10
#define STATE_NUM4				11
#define STATE_NUM3				12
#define STATE_NUM2				13
#define STATE_NUM1				14
#define STATE_EMPTY				15		//����

#define ATTRIB_EMPTY			0		//����
#define ATTRIB_MINE				1		//��
//*****************************************************//



//*****************************************************//
//     �������״̬�ṹ�岢�����������������״̬      // 
struct MouseState
{
	UINT mouse_pos_line;       //�������һ��
	UINT mouse_pos_coloum;     //�������һ��
	UINT mouse_area;           //������ĸ�����
	UINT mouse_button;         //��갴�����
};
static MouseState mState; 


#define 	  LBUTTONUP		 1    //�������
#define 	  LBUTTONDOWN	 2	  //�������
#define		  RBUTTONDOWN	 3	  //�Ҽ�����
#define		  DBBUTTONDOWN	 4    //˫������
#define		  DBBUTTONUP	 5    //˫������
#define		  MOUSEMOVING	 6    //����ƶ�

#define       MOUSEINFACE    1    //����ڱ�������
#define       MOUSEINMINE    2    //���������
#define       MOUSEINOTHER   3    //�������������
   





///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                                     ����                                                              //
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

LRESULT CALLBACK WndProc (HWND, UINT, WPARAM, LPARAM) ;             //���ص�����
BOOL CALLBACK AboutDlgProc (HWND, UINT, WPARAM, LPARAM) ;           //�����Ի���ص�����
BOOL CALLBACK RecordDlgProc (HWND, UINT, WPARAM, LPARAM) ;          //��¼�Ի���ص�����



BOOL initializeinfo(HWND hwnd,int level);                           //���ü�����ʼ���ؼ�����

RECT getlevelwindowsize(HWND hwnd);                                 //��ȡ��ͬ�ȼ��Ĵ��ڴ�С��λ��

RECT CreateRect(int left,int top,int right,int bottom);             //���ڽ�����������

BOOL DrawSolidFrame(HDC hDC,RECT rcFrame,int iLighten);             //���߿�

BOOL LayMines(HWND hwnd);                                           //����

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