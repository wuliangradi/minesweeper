

#include "MinesweeperWnd.h"






/////////////////////////////////////////////////////////////////////////////////////////////////////////
//                               ********主窗口函数 程序开始的地方*********                            //
/////////////////////////////////////////////////////////////////////////////////////////////////////////

int WINAPI WinMain (HINSTANCE hInstance, HINSTANCE hPrevInstance,
                    PSTR szCmdLine, int iCmdShow)
{
	static TCHAR szAppName [] = TEXT ("Bricks1") ;
	HWND         hwnd ;
	MSG          msg ;
	WNDCLASS     wndclass ;
	
	wndclass.style         = CS_HREDRAW | CS_VREDRAW ;
	wndclass.lpfnWndProc   = WndProc ;
	wndclass.cbClsExtra    = 0 ;
	wndclass.cbWndExtra    = 0 ;
	wndclass.hInstance     = hInstance ;
	wndclass.hIcon         = LoadIcon (hInstance,MAKEINTRESOURCE(IDI_ICON)) ;
	wndclass.hCursor       = LoadCursor (NULL, IDC_ARROW) ;
	wndclass.hbrBackground = (HBRUSH) GetStockObject (WHITE_BRUSH) ;
	wndclass.lpszMenuName  = MAKEINTRESOURCE(IDR_MAINMENU);         //这里“注册”菜单
	wndclass.lpszClassName = szAppName ;
	
	if (!RegisterClass (&wndclass))
	{
		MessageBox (NULL, TEXT ("This program requires Windows NT!"),
			szAppName, MB_ICONERROR) ;
		return 0 ;
	}
	
	hwnd = CreateWindow (szAppName, TEXT ("扫雷"), 
		WS_SYSMENU | WS_MINIMIZEBOX | WS_OVERLAPPED, //这里设置窗口样式，如不能最大化
		CW_USEDEFAULT, CW_USEDEFAULT,
		CW_USEDEFAULT, CW_USEDEFAULT,
		NULL, NULL, hInstance, NULL) ;
	
	ShowWindow (hwnd, iCmdShow) ;
	UpdateWindow (hwnd) ;
	
	while (GetMessage (&msg, NULL, 0, 0))
	{
		TranslateMessage (&msg) ;
		DispatchMessage (&msg) ;
	}
	return msg.wParam ;
}






///////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                       ********主窗口的响应函数（回调函数） ，命令执行的地方*********                      //
///////////////////////////////////////////////////////////////////////////////////////////////////////////////
LRESULT CALLBACK WndProc (HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	static HBITMAP hBitmap ;
	static int     cxClient, cyClient, cxSource, cySource ;
	BITMAP         bitmap ;
	HDC            hdc, hdcMem ;
	static HINSTANCE      hInstance ;                //static太重要了！！！！如果这里不加static，下面dialogbox里的instance不能用
    RECT Irect;                                //分别定义信息窗口的矩形区域和雷区的矩形区域
	HPEN hpen;
	HBRUSH hbrush;
	HMENU  hmenu;                                    //程序菜单窗口句柄
	int            x, y ;
	PAINTSTRUCT    ps ;

	switch (message)
	{
	case WM_CREATE:
		
		hInstance = ((LPCREATESTRUCT) lParam)->hInstance ;
		hmenu=((LPCREATESTRUCT)lParam)->hMenu;       //创建菜单
        level = 2;
		initializeinfo(hwnd,level);
		Mrect = getlevelwindowsize(hwnd);
		LayMines(hwnd);



		MoveWindow(hwnd,Mrect.left,Mrect.top,Mrect.right,Mrect.bottom,TRUE);
		return 0 ;
			
	case WM_SIZE:
		cxClient = LOWORD (lParam) ;
		cyClient = HIWORD (lParam) ;
		return 0 ;
		
	case WM_PAINT:
		
		hdc = BeginPaint (hwnd, &ps) ;
		hdcMem = CreateCompatibleDC (hdc) ;
	    GetClientRect(hwnd,&Mrect);
		
		
	 		
		hpen=(HPEN)GetStockObject(NULL_PEN);          //画出的矩形无框
		SelectObject(hdc,hpen);
		
        hbrush=CreateSolidBrush(RGB(200,220,220));
		SelectObject(hdc,hbrush);
        Rectangle(hdc,3,3,Mrect.right+1,Mrect.bottom+1);		

		DeleteObject(hbrush);
        
       
        
		Rectangle(	hdc,
					MAIN_LEFTSTND+(mine_col_num*MINE_BITMAP_WIDTH-FACE_BITMAP_WIDTH)/2-1,
					(MAIN_TOPSTND-FACE_BITMAP_HEIGHT)/2-1,
					MAIN_LEFTSTND+(mine_line_num*MINE_BITMAP_WIDTH-FACE_BITMAP_WIDTH)/2+FACE_BITMAP_WIDTH+2,
 					(MAIN_TOPSTND-FACE_BITMAP_HEIGHT)/2+FACE_BITMAP_HEIGHT+2);

        
    //****表情贴图****//
		hBitmap=LoadBitmap(hInstance,MAKEINTRESOURCE(IDB_FACE));
		SelectObject(hdcMem,hBitmap);
		BitBlt(  hdc,MAIN_LEFTSTND+(mine_col_num*MINE_BITMAP_WIDTH-FACE_BITMAP_WIDTH)/2,
			     (MAIN_TOPSTND-FACE_BITMAP_HEIGHT)/2,
				 FACE_BITMAP_WIDTH,
				 FACE_BITMAP_HEIGHT,
				 hdcMem,0, facestate * FACE_BITMAP_HEIGHT,SRCCOPY);
		DeleteObject(hBitmap);

    //****地雷级数、时间计数****//
		hBitmap=LoadBitmap(hInstance,MAKEINTRESOURCE(IDB_TIME));
		SelectObject(hdcMem,hBitmap);
		BitBlt(  hdc,COUNT_LEFTPOS,COUNT_TOPPOS,NUM_BITMAP_WIDTH,NUM_BITMAP_HEIGHT,
			     hdcMem,0,NUM_BITMAP_HEIGHT*(minecount<0?0:11-(int)floor(minecount/100)),SRCCOPY);
		BitBlt(  hdc,COUNT_LEFTPOS+NUM_BITMAP_WIDTH,COUNT_TOPPOS,NUM_BITMAP_WIDTH,NUM_BITMAP_HEIGHT,
			     hdcMem,0,NUM_BITMAP_HEIGHT*(11-(int)floor((abs(minecount)-(int)floor(abs(minecount)/100)*100)/10)),SRCCOPY);
		BitBlt(  hdc,COUNT_LEFTPOS+NUM_BITMAP_WIDTH*2,COUNT_TOPPOS,NUM_BITMAP_WIDTH,NUM_BITMAP_HEIGHT,
			     hdcMem,0,NUM_BITMAP_HEIGHT*(11-(abs(minecount)-(int)floor(abs(minecount)/10)*10)),SRCCOPY);
			
		BitBlt(  hdc,Mrect.right-TIME_RIGHTSTND-TIME_WIDTH,TIME_TOPPOS,NUM_BITMAP_WIDTH,NUM_BITMAP_HEIGHT,
		         hdcMem,0,NUM_BITMAP_HEIGHT*(11-(timecount<1000?(int)floor(timecount/100):9)),SRCCOPY);
		BitBlt(  hdc,Mrect.right-TIME_RIGHTSTND-TIME_WIDTH+NUM_BITMAP_WIDTH,TIME_TOPPOS,NUM_BITMAP_WIDTH,NUM_BITMAP_HEIGHT,
			     hdcMem,0,NUM_BITMAP_HEIGHT*(11-(timecount<1000?(int)floor((timecount-(int)floor(timecount/100)*100)/10):9)),SRCCOPY);
		BitBlt(  hdc,Mrect.right-TIME_RIGHTSTND-TIME_WIDTH+NUM_BITMAP_WIDTH*2,TIME_TOPPOS,NUM_BITMAP_WIDTH,NUM_BITMAP_HEIGHT,
			     hdcMem,0,NUM_BITMAP_HEIGHT*(11-(timecount<1000?(timecount-(int)floor(timecount/10)*10):9)),SRCCOPY);
		DeleteObject(hBitmap);
		
    //****画边框、渲染****//
        Irect=CreateRect(  MAIN_LEFTSTND-LINE_LIGHTEN_THICK,MAIN_TOPSTND-LINE_LIGHTEN_THICK,
			               Mrect.right-MAIN_RIGHTSTND+LINE_LIGHTEN_THICK,Mrect.bottom-MAIN_BOTTOMSTND+LINE_LIGHTEN_THICK);
		DrawSolidFrame(hdc,Irect,LINE_LIGHTEN_THICK);

		Irect=CreateRect(  FUN_LEFTPOS-LINE_LIGHTEN_THICK,FUN_TOPPOS-LINE_LIGHTEN_THICK,
			               Mrect.right-FUN_RIGHTSTND+LINE_LIGHTEN_THICK,MAIN_TOPSTND-FUN_BOTTOMSTND+LINE_LIGHTEN_THICK);
		DrawSolidFrame(hdc,Irect,LINE_LIGHTEN_NORMAL);
		
		Irect=CreateRect(  COUNT_LEFTPOS-LINE_LIGHTEN_THIN,COUNT_TOPPOS-LINE_LIGHTEN_THIN,
			               COUNT_RIGHTPOS+LINE_LIGHTEN_THIN,COUNT_BOTTOMPOS+LINE_LIGHTEN_THIN);
		DrawSolidFrame(hdc,Irect,LINE_LIGHTEN_THIN);

		Irect=CreateRect(  Mrect.right-TIME_RIGHTSTND-TIME_WIDTH-LINE_LIGHTEN_THIN,TIME_TOPPOS-LINE_LIGHTEN_THIN,
			               Mrect.right-TIME_RIGHTSTND+LINE_LIGHTEN_THIN,TIME_TOPPOS+TIME_HEIGHT+LINE_LIGHTEN_THIN);
		DrawSolidFrame(hdc,Irect,LINE_LIGHTEN_THIN);

	//****雷区贴图****//
		hBitmap=LoadBitmap(hInstance,MAKEINTRESOURCE(IDB_MINE));
     	SelectObject(hdcMem,hBitmap);
		for(x=0;x<mine_col_num;x++)
		{
			for(y=0;y<mine_line_num;y++)
			{

				switch(mState.mouse_button) {
				case 3:
						  if (cellmine[mState.mouse_pos_line][mState.mouse_pos_coloum].special_state >0)
						  {
							  switch(cellmine[mState.mouse_pos_line][mState.mouse_pos_coloum].special_state) {
							  case 1:
								    BitBlt(  hdc,MAIN_LEFTSTND+MINE_BITMAP_WIDTH*x,MAIN_TOPSTND+MINE_BITMAP_HEIGHT*y,
					                         MINE_BITMAP_WIDTH,MINE_BITMAP_HEIGHT,hdcMem,0,MINE_BITMAP_HEIGHT*0,SRCCOPY);

							  	break;
							  case 2:
								    BitBlt(  hdc,MAIN_LEFTSTND+MINE_BITMAP_WIDTH*x,MAIN_TOPSTND+MINE_BITMAP_HEIGHT*y,
					                         MINE_BITMAP_WIDTH,MINE_BITMAP_HEIGHT,hdcMem,0,MINE_BITMAP_HEIGHT*2,SRCCOPY);

							  	break;
							  case 3:
									BitBlt(  hdc,MAIN_LEFTSTND+MINE_BITMAP_WIDTH*x,MAIN_TOPSTND+MINE_BITMAP_HEIGHT*y,
					                MINE_BITMAP_WIDTH,MINE_BITMAP_HEIGHT, hdcMem,0,MINE_BITMAP_HEIGHT*1,SRCCOPY);
							  	break;
							  
							  }
						  }

					break;
				default :
						  switch(gamestate) {
						  case 2:
								if (cellmine[mState.mouse_pos_line][mState.mouse_pos_coloum].special_state == 0)
								{						
									BitBlt(  hdc,MAIN_LEFTSTND+MINE_BITMAP_WIDTH*x,MAIN_TOPSTND+MINE_BITMAP_HEIGHT*y,
					                         MINE_BITMAP_WIDTH,MINE_BITMAP_HEIGHT,
					                         hdcMem,0,MINE_BITMAP_HEIGHT*15,SRCCOPY);
								}
					
								else if(cellmine[mState.mouse_pos_line][mState.mouse_pos_coloum].special_state == -1)
								{	        		        	
					 
						
									BitBlt(  hdc,MAIN_LEFTSTND+MINE_BITMAP_WIDTH*x,MAIN_TOPSTND+MINE_BITMAP_HEIGHT*y,
					                         MINE_BITMAP_WIDTH,MINE_BITMAP_HEIGHT,
					                         hdcMem,0,MINE_BITMAP_HEIGHT*(15-cellmine[y][x].cur_state),SRCCOPY);
								}
					
						  	break;
						  case 1:
									BitBlt(  hdc,MAIN_LEFTSTND+MINE_BITMAP_WIDTH*x,MAIN_TOPSTND+MINE_BITMAP_HEIGHT*y,
					                         MINE_BITMAP_WIDTH,MINE_BITMAP_HEIGHT,
						                     hdcMem,0,MINE_BITMAP_HEIGHT*15,SRCCOPY);
						  	break;
						  case 0:
									BitBlt(  hdc,MAIN_LEFTSTND+MINE_BITMAP_WIDTH*x,MAIN_TOPSTND+MINE_BITMAP_HEIGHT*y,
					                         MINE_BITMAP_WIDTH,MINE_BITMAP_HEIGHT,
 						                     hdcMem,0,MINE_BITMAP_HEIGHT*0,SRCCOPY);
						  	break;
						  }
					break;
				


				case 5:
						  if(cellmine[mState.mouse_pos_line][mState.mouse_pos_coloum].special_state ==1)
						  {					
						
							  BitBlt(  hdc,MAIN_LEFTSTND+MINE_BITMAP_WIDTH*x,MAIN_TOPSTND+MINE_BITMAP_HEIGHT*y,
								  MINE_BITMAP_WIDTH,MINE_BITMAP_HEIGHT,hdcMem,0,MINE_BITMAP_HEIGHT*0,SRCCOPY);
						  }
					break;
				}

								
			}
		}		
		DeleteObject(hBitmap);
		
		
		DeleteDC (hdcMem) ;
		EndPaint (hwnd, &ps) ;
		return 0 ;

	case WM_COMMAND:
         switch(LOWORD(wParam)) 
		 {
			case IDM_START:              	//应该与微笑笑脸是一个意思
                 
				initializeinfo(hwnd,level);
            	Mrect = getlevelwindowsize(hwnd);

				LayMines(hwnd);

				MoveWindow(hwnd,Mrect.left,Mrect.top,Mrect.right,Mrect.bottom,TRUE);
				InvalidateRect(hwnd,NULL,TRUE);
				
                return 0;
			case IDM_RECORD:
                DialogBox(hInstance,MAKEINTRESOURCE(ID_RECORDDLG),hwnd,RecordDlgProc);
				break;

				
			case IDM_PRIMARY:               //初级
				
				level = 1;
                initializeinfo(hwnd,level);
            	Mrect = getlevelwindowsize(hwnd);

				LayMines(hwnd);
				MoveWindow(hwnd,Mrect.left,Mrect.top,Mrect.right,Mrect.bottom,TRUE);
				InvalidateRect(hwnd,NULL,TRUE);
				
				return 0;
			case IDM_INTERMEDIATE:          //中级
				level = 2;
				initializeinfo(hwnd,level);
            	Mrect = getlevelwindowsize(hwnd);
				LayMines(hwnd);
				MoveWindow(hwnd,Mrect.left,Mrect.top,Mrect.right,Mrect.bottom,TRUE);
				InvalidateRect(hwnd,NULL,TRUE);

				return 0;
     
            case IDM_ADVANCED:              //高级
				level = 3;
				initializeinfo(hwnd,level);
            	Mrect = getlevelwindowsize(hwnd);

				LayMines(hwnd);
				MoveWindow(hwnd,Mrect.left,Mrect.top,Mrect.right,Mrect.bottom,TRUE);
				
				InvalidateRect(hwnd,NULL,TRUE);

				return 0;

			case IDM_SOUND:

				return 0;
			case IDM_EXIT:
				SendMessage(hwnd,WM_DESTROY,wParam,lParam);
				break;

            case IDM_STRATEGY:
				
				return 0;
			case IDM_ABOUT:
				DialogBox(hInstance,MAKEINTRESOURCE(ID_ABOUTDLG),hwnd,AboutDlgProc);
				break;

		}
        return 0;
		
	case WM_LBUTTONDOWN:

		
		
        SetCapture(hwnd);	
		mState.mouse_button = 2;
		mState = GetMouseState(hwnd,hInstance,LOWORD(lParam),HIWORD(lParam),mState.mouse_button);
		
		if (wParam&MK_RBUTTON)			//同时右键已经按下
		{
			mState.mouse_button = 4;


			DbbuttondownFun(hwnd);
			
		} 
		else
		{
		
			LbuttondownFun(hwnd);

		}
		

        
		
	
		
		return 0;
	
	case WM_LBUTTONUP:

		if (gamestate == 3)
		{
			return 0;
		}
		ReleaseCapture();
			
		mState.mouse_button = 1;
		mState = GetMouseState(hwnd,hInstance,LOWORD(lParam),HIWORD(lParam),mState.mouse_button);

  
		LbuttonupFun(hwnd);

        
		
		


		
		return 0;
		
	case WM_RBUTTONDOWN:
        
		SetCapture(hwnd);	
		mState.mouse_button = 3;
		mState = GetMouseState(hwnd,hInstance,LOWORD(lParam),HIWORD(lParam),mState.mouse_button);
		

			
		RbuttondownFun(hwnd);


	    ReleaseCapture();
		return 0;
		
	case WM_RBUTTONUP:

		if (wParam&MK_LBUTTON||mState.mouse_button == 1)	
		{
			mState.mouse_button = 5;
			DbbuttonupFun(hwnd);

        }



		return 0;

	case WM_LBUTTONDBLCLK:
		    mState.mouse_button = 8;
	    	mState = GetMouseState(hwnd,hInstance,LOWORD(lParam),HIWORD(lParam),mState.mouse_button);

			RECT Irect1;			
		    Irect1 = CreateRect(mState.mouse_pos_coloum,mState.mouse_pos_line,0,0);	
	        InvalidateRect(hwnd,&Irect1,FALSE);
		return 0;
	case WM_TIMER:
        
		timecount++;		//时间计数
		
		GetClientRect(hwnd,&Mrect);
		Irect=CreateRect(  Mrect.right-TIME_RIGHTSTND-TIME_WIDTH-LINE_LIGHTEN_THIN,TIME_TOPPOS-LINE_LIGHTEN_THIN,
			               Mrect.right-TIME_RIGHTSTND+LINE_LIGHTEN_THIN,TIME_TOPPOS+TIME_HEIGHT+LINE_LIGHTEN_THIN);	
		InvalidateRect(hwnd,&Irect,FALSE);


		return 0;
	case WM_DESTROY:
		DeleteObject (hBitmap) ;
		PostQuitMessage (0) ;
		return 0 ;
	}
	return DefWindowProc (hwnd, message, wParam, lParam) ;
}







//////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                  ********    对话框回调函数    *********                                 //
//////////////////////////////////////////////////////////////////////////////////////////////////////////////



BOOL CALLBACK AboutDlgProc (HWND hDlg, UINT message, 
                            WPARAM wParam, LPARAM lParam)
{
     switch (message)
     {
     case WM_INITDIALOG :
          return TRUE ;
          
     case WM_COMMAND :
          switch (LOWORD (wParam))
          {
          case IDOK :
          case IDCANCEL :
               EndDialog (hDlg, 0) ;
               return TRUE ;
          }
          break ;
     }
     return FALSE ;
}



BOOL CALLBACK RecordDlgProc (HWND hDlg, UINT message, 
                            WPARAM wParam, LPARAM lParam)
{
     switch (message)
     {
     case WM_INITDIALOG :
          return TRUE ;
          
     case WM_COMMAND :
          switch (LOWORD (wParam))
          {
          case IDOK :
               EndDialog (hDlg, 0) ;
               return TRUE ;
          }
          break ;
     }
     return FALSE ;
}








//////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                  ********     各类功能函数     *********                                 //
//////////////////////////////////////////////////////////////////////////////////////////////////////////////


BOOL initializeinfo(HWND hwnd,int level)
{
	

	if (level == 1)
	{
		mine_col_num = PRIMARY_COL_NUM;
        mine_line_num = PRIMARY_LN_NUM;
		mine_num = PRIMARY_MINE_NUM;
		zerocount = PRIMARY_COL_NUM * PRIMARY_LN_NUM - PRIMARY_MINE_NUM;
	}
        
	else if (level == 2)
	{
		mine_col_num = INTERMEDIATE_COL_NUM;
        mine_line_num = INTERMEDIATE_LN_NUM;
		mine_num = INTERMEDIATE_MINE_NUM;
	    zerocount = PRIMARY_COL_NUM * PRIMARY_LN_NUM - PRIMARY_MINE_NUM;
	}
        
	else if (level == 3)
	{
		mine_col_num = ADVANCED_COL_NUM;
        mine_line_num = ADVANCED_LN_NUM;
		mine_num = ADVANCED_MINE_NUM;
	    zerocount = PRIMARY_COL_NUM * PRIMARY_LN_NUM - PRIMARY_MINE_NUM;
	}
        
	
       minecount = mine_num;
	   gamestate = 0;
	   timecount = 0;
	   timerstate = 1;
       facestate  = 4;
	   gamestate = 0;
	   cellnum = mine_col_num*mine_line_num;
	   
	   mState.mouse_button = -1;

       KillTimer(hwnd,timecount);


	return TRUE; 
}




RECT getlevelwindowsize(HWND hwnd)
{
	RECT rcFrame,rcWindow,rcClient;
	
	GetWindowRect(hwnd,&rcWindow);
	GetClientRect(hwnd,&rcClient);
	
	rcFrame.left=rcWindow.left;	
	rcFrame.top=rcWindow.top;		
	rcFrame.right=rcClient.left-rcWindow.left+rcWindow.right-rcClient.right+mine_col_num*MINE_BITMAP_WIDTH+MAIN_LEFTSTND+MAIN_RIGHTSTND;
	rcFrame.bottom=rcClient.top-rcWindow.top+rcWindow.bottom-rcClient.bottom+mine_line_num*MINE_BITMAP_HEIGHT+MAIN_TOPSTND+MAIN_BOTTOMSTND;
	
	return rcFrame;

}



RECT CreateRect(int left,int top,int right,int bottom)
{
	RECT rcReturn;
	
	rcReturn.left=(right==0)?MAIN_LEFTSTND+left*MINE_BITMAP_WIDTH:left;
	rcReturn.top=(bottom==0)?MAIN_TOPSTND+top*MINE_BITMAP_HEIGHT:top;
	rcReturn.right=(right==0)?rcReturn.left+MINE_BITMAP_WIDTH:right;
	rcReturn.bottom=(bottom==0)?rcReturn.top+MINE_BITMAP_HEIGHT:bottom;

	return rcReturn;
}

BOOL DrawSolidFrame(HDC hdc,RECT rcFrame,int iLighten)
{
	HPEN hPen;

	for(int iCount=0;iCount<iLighten;iCount++){	
		
		hPen=CreatePen(PS_SOLID,0,RGB(250,250,250));
		SelectObject(hdc,hPen);
		MoveToEx(hdc,rcFrame.right-iCount-1,rcFrame.top+iCount,NULL);	
		LineTo(hdc,rcFrame.right-iCount-1,rcFrame.bottom-iCount-1);		
		LineTo(hdc,rcFrame.left+iCount,rcFrame.bottom-iCount-1);		
		DeleteObject(hPen);
		
		hPen=CreatePen(PS_SOLID,0,RGB(192,192,192));
		SelectObject(hdc,hPen);
		LineTo(hdc,rcFrame.left+iCount,rcFrame.top+iCount);				
		LineTo(hdc,rcFrame.right-iCount,rcFrame.top+iCount);			
		DeleteObject(hPen);
	}
	return TRUE;
}


BOOL LayMines(HWND hwnd)
{
	
	

    facestate = 4;
//    RECT rectmine;

	srand( (unsigned)time( NULL ) );
	INT i, j;
	INT m, n;
	int aa,bb,cc,ee,dd;
	int ff,gg,hh;
 
	for (m =0;m<mine_line_num;m++)
	{
		for (n = 0;n<mine_col_num;n++)
		{
			
				cellmine[m][n].cur_state = 15;
				cellmine[m][n].cell_attrib = 0;
				
		
		}
	}
	
	for(UINT index = 0; index < mine_num;) 
	{	
		
		i = rand() % mine_line_num;
		j = rand() % mine_col_num;

		if (i == mine_line_num && j == mine_col_num) continue;
		
		if(cellmine[i][j].cell_attrib != ATTRIB_MINE) 
		{
			cellmine[i][j].cell_attrib = ATTRIB_MINE;

			index++;
		}
	}

	


	for (m =0;m<mine_line_num;m++)
	{
		for (n = 0;n<mine_col_num;n++)
		{
			cellmine[m][n].old_state = 1;
			cellmine[m][n].special_state =1;
			cellmine[m][n].expand_state = 1;
			if (cellmine[m][n].cell_attrib == ATTRIB_MINE)
			{
				cellmine[m][n].cur_state = 10;
			}

			else 
			{
				aa =(m-1<0||n-1<0)?0:cellmine[m-1][n-1].cell_attrib;

				bb = (m-1<0)?0:cellmine[m-1][n].cell_attrib;
				
				cc =(m-1<0||n+1 >= mine_col_num)?0:cellmine[m-1][n+1].cell_attrib;

				dd =(n-1<0)?0:cellmine[m][n-1].cell_attrib;

				ee =(n-1<0||m+1 >= mine_line_num)?0:cellmine[m+1][n-1].cell_attrib;

				ff =(m+1 >= mine_line_num)?0:cellmine[m+1][n].cell_attrib;

				gg = (n+1 >= mine_col_num)?0:cellmine[m][n+1].cell_attrib;


				hh = (m+1 >= mine_line_num||n+1 >= mine_col_num)?0:cellmine[m+1][n+1].cell_attrib;

			
			
			    
				cellmine[m][n].cur_state = aa+bb+cc+dd+ee+gg+ff+hh;

				cellmine[m][n].old_state = 1;
			
			}

        
		
	}
	}

//	RECT rcmine1;
//	rcmine1=CreateRect(  MAIN_LEFTSTND,MAIN_TOPSTND,MAIN_LEFTSTND+MINE_BITMAP_WIDTH*mine_col_num,
//			                MAIN_TOPSTND+MINE_BITMAP_HEIGHT*mine_line_num);
//   	InvalidateRect(hwnd,&rcmine1,TRUE);
// 	return TRUE;

	 

       
//		rectmine=CreateRect(  MAIN_LEFTSTND,MAIN_TOPSTND,
//			                  MAIN_LEFTSTND+MINE_BITMAP_WIDTH*mine_col_num,MAIN_TOPSTND+MINE_BITMAP_HEIGHT*mine_line_num);
//     	InvalidateRect(hwnd,&rectmine,TRUE);
	    
        return true;
}



MouseState GetMouseState(HWND hwnd,HINSTANCE hInstance,int MouseXPos,int MouseYPos,int MouseSta)
{
	
	MouseState state;
	
	if (MouseXPos>MAIN_LEFTSTND&&MouseXPos<(MAIN_LEFTSTND+mine_col_num*MINE_BITMAP_WIDTH)&&MouseYPos>MAIN_TOPSTND&&MouseYPos<(MAIN_TOPSTND+mine_line_num*MINE_BITMAP_HEIGHT))
	{	
		state.mouse_pos_coloum =(int)floor((MouseXPos-MAIN_LEFTSTND)/MINE_BITMAP_WIDTH);
		state.mouse_pos_line =(int)floor((MouseYPos-MAIN_TOPSTND)/MINE_BITMAP_HEIGHT);
		state.mouse_area =MOUSEINMINE;
	} 
	else if (MouseXPos>=(MAIN_LEFTSTND+(mine_col_num*MINE_BITMAP_WIDTH-FACE_BITMAP_WIDTH)/2)
			&&MouseXPos<=(MAIN_LEFTSTND+(mine_col_num*MINE_BITMAP_WIDTH-FACE_BITMAP_WIDTH)/2+FACE_BITMAP_WIDTH)
			&&MouseYPos>=((MAIN_TOPSTND-FACE_BITMAP_HEIGHT)/2)
			&&MouseYPos<=((MAIN_TOPSTND-FACE_BITMAP_HEIGHT)/2+FACE_BITMAP_HEIGHT)) 
	{
		state.mouse_area= MOUSEINFACE;   
	}
	else
	{	
		state.mouse_pos_coloum=MouseXPos>MAIN_LEFTSTND?(int)floor((MouseXPos-MAIN_LEFTSTND)/MINE_BITMAP_WIDTH):-2;
		state.mouse_pos_coloum=MouseXPos<(MAIN_LEFTSTND+mine_col_num*MINE_BITMAP_WIDTH)?state.mouse_pos_coloum:mine_col_num+1;
		state.mouse_pos_line=MouseYPos>MAIN_TOPSTND?(int)floor((MouseXPos-MAIN_TOPSTND)/MINE_BITMAP_HEIGHT):-2;
		state.mouse_pos_line=MouseYPos<(MAIN_TOPSTND+mine_line_num*MINE_BITMAP_HEIGHT)?state.mouse_pos_line:mine_line_num+1;
		state.mouse_area=MOUSEINOTHER;
	}
	state.mouse_button = MouseSta;
	return state;
}


BOOL ChangeFace (HWND hwnd,int mface)
{

	RECT rcface=CreateRect(  MAIN_LEFTSTND+(mine_col_num*MINE_BITMAP_WIDTH-FACE_BITMAP_WIDTH)/2,
			                     (MAIN_TOPSTND-FACE_BITMAP_HEIGHT)/2,
			                     MAIN_LEFTSTND+(mine_col_num*MINE_BITMAP_WIDTH-FACE_BITMAP_WIDTH)/2+FACE_BITMAP_WIDTH,
								 (MAIN_TOPSTND-FACE_BITMAP_HEIGHT)/2+FACE_BITMAP_HEIGHT);
	   
	InvalidateRect(hwnd,&rcface,TRUE);
	
    return true;
}



BOOL LbuttondownFun(HWND hwnd)
{   
	



		if (mState.mouse_area == 1)
		{
			
			facestate =  0;
			ChangeFace (hwnd,facestate);

			initializeinfo(hwnd,level);
            Mrect = getlevelwindowsize(hwnd);

			LayMines(hwnd);

			MoveWindow(hwnd,Mrect.left,Mrect.top,Mrect.right,Mrect.bottom,TRUE);
 			InvalidateRect(hwnd,NULL,TRUE);


		
			
		}
		

		if (mState.mouse_area == 2)
		{
		    if (gamestate == 2&&cellmine[mState.mouse_pos_line][mState.mouse_pos_coloum].cur_state == 0)
			{
				KillTimer(hwnd,timecount);	
						
			    
			}

			facestate =  3;
			ChangeFace (hwnd,facestate);


			if (cellmine[mState.mouse_pos_line][mState.mouse_pos_coloum].old_state == 0)
			{
		
			
				return 0;
			}
			
			if(gamestate == 0)
			{
				
				
				while(cellmine[mState.mouse_pos_line][mState.mouse_pos_coloum].cur_state != 0 && cellmine[mState.mouse_pos_line][mState.mouse_pos_coloum].cur_state != 1)
				{
					LayMines(hwnd);
								
				}
				gamestate = 1;
            }
			cellmine[mState.mouse_pos_line][mState.mouse_pos_coloum].special_state = 0;
			
			

			RECT Irect2;			
	        Irect2 = CreateRect(mState.mouse_pos_coloum,mState.mouse_pos_line,0,0);	
	        InvalidateRect(hwnd,&Irect2,FALSE);

            

		}


	

		if (mState.mouse_area == 3)
		{

							
			facestate =  3;
			ChangeFace (hwnd,facestate);
		}
        
	


    return true;


}


BOOL LbuttonupFun(HWND hwnd)
{
	int x,y;
	
			
	if (mState.mouse_area == 2)
	{
		facestate =  4;
		ChangeFace (hwnd,facestate);
		
		
		
		if (gamestate == 1)
		{				 
			SetTimer(hwnd,1,1000,NULL);	
		}

		if (cellmine[mState.mouse_pos_line][mState.mouse_pos_coloum].old_state == 0)
		{
		
			
	            return 0;
		}
		
	
		gamestate = 2;

		switch(cellmine[mState.mouse_pos_line][mState.mouse_pos_coloum].cur_state)
		{
		case 0:
		
			
		    ExpandMine(hwnd,mState.mouse_pos_line,mState.mouse_pos_coloum);

         	
			break;
		case 10:
			KillTimer(hwnd,1);
			
			
            cellmine[mState.mouse_pos_line][mState.mouse_pos_coloum].cur_state = 12;
			cellmine[mState.mouse_pos_line][mState.mouse_pos_coloum].special_state = -1;
			
			for (x =0;x<mine_line_num;x++)
			{
		        for (y= 0;y<mine_col_num;y++)
				{
					
                    cellmine[x][y].old_state = 0;
					
					if (cellmine[x][y].cur_state == 10||cellmine[x][y].cur_state ==12)
					{		
						cellmine[x][y].special_state = -1;
						
				       
	                   RECT Irect2;			
		               Irect2 = CreateRect(y,x,0,0);	
	                   InvalidateRect(hwnd,&Irect2,FALSE);
					}

				}

		
			}
			MessageBoxA(NULL,TEXT("踩雷咯"),TEXT("Hung Up"),MB_ICONINFORMATION|MB_OK);
			break;
		default:
			cellmine[mState.mouse_pos_line][mState.mouse_pos_coloum].old_state = 0;
			
			
			cellmine[mState.mouse_pos_line][mState.mouse_pos_coloum].special_state = -1;
			
			RECT Irect1;			
		    Irect1 = CreateRect(mState.mouse_pos_coloum,mState.mouse_pos_line,0,0);	
	        InvalidateRect(hwnd,&Irect1,FALSE);
		}
	

		
		
			
     }
	
	
	
	



		if (mState.mouse_area == 1)
		{
			

			facestate =  4;
			ChangeFace (hwnd,facestate);	
				
		}


		if (mState.mouse_area == 3)
		{
			
			facestate =  4;
			ChangeFace (hwnd,facestate);
			
			}


	    

		return true;
	
}


BOOL RbuttondownFun(HWND hwnd)
{



	if (cellmine[mState.mouse_pos_line][mState.mouse_pos_coloum].special_state == -1)
	{
		return 0;
	}
	else{
	
	
	if (mState.mouse_area == 2 && cellmine[mState.mouse_pos_line][mState.mouse_pos_coloum].special_state >=1 )
	{
		
		//PostQuitMessage (0) ;
		switch(cellmine[mState.mouse_pos_line][mState.mouse_pos_coloum].special_state) 
		{
		case 1 :
			
			cellmine[mState.mouse_pos_line][mState.mouse_pos_coloum].special_state = 3;
			cellmine[mState.mouse_pos_line][mState.mouse_pos_coloum].old_state = 0;
			minecount--;
			updateminecount(hwnd);

		    RECT Irect0;		
		    Irect0 = CreateRect(mState.mouse_pos_coloum,mState.mouse_pos_line,0,0);	
	        InvalidateRect(hwnd,&Irect0,FALSE);
			break;
	
		case 2:
			
			cellmine[mState.mouse_pos_line][mState.mouse_pos_coloum].special_state = 1;
			cellmine[mState.mouse_pos_line][mState.mouse_pos_coloum].old_state = 1;
			
			RECT Irect1;		
		    Irect1 = CreateRect(mState.mouse_pos_coloum,mState.mouse_pos_line,0,0);	
	        InvalidateRect(hwnd,&Irect1,FALSE);
			break;
		case 3:
			cellmine[mState.mouse_pos_line][mState.mouse_pos_coloum].special_state = 2;
			cellmine[mState.mouse_pos_line][mState.mouse_pos_coloum].old_state = 0;
			minecount++;
			updateminecount(hwnd);
		    RECT Irect2;		
		    Irect2 = CreateRect(mState.mouse_pos_coloum,mState.mouse_pos_line,0,0);	
	        InvalidateRect(hwnd,&Irect2,FALSE);
			break;
		}
		
		

    }

		
	}

	return 0;

}


BOOL updateminecount(HWND hwnd)
{
	RECT rectminecount;
    rectminecount = CreateRect(COUNT_LEFTPOS,COUNT_TOPPOS,COUNT_RIGHTPOS,COUNT_BOTTOMPOS);
    InvalidateRect(hwnd,&rectminecount,true);
	return true;
	
}


BOOL DbbuttondownFun(HWND hwnd)
{
	
//	if ()
//	{
	
	cellmine[mState.mouse_pos_line-1][mState.mouse_pos_coloum-1].special_state = 0;
	cellmine[mState.mouse_pos_line-1][mState.mouse_pos_coloum].special_state = 0;
	cellmine[mState.mouse_pos_line-1][mState.mouse_pos_coloum+1].special_state = 0;
	cellmine[mState.mouse_pos_line][mState.mouse_pos_coloum-1].special_state = 0;
	cellmine[mState.mouse_pos_line][mState.mouse_pos_coloum].special_state = 0;
	cellmine[mState.mouse_pos_line][mState.mouse_pos_coloum+1].special_state = 0;
	cellmine[mState.mouse_pos_line+1][mState.mouse_pos_coloum-1].special_state = 0;
	cellmine[mState.mouse_pos_line+1][mState.mouse_pos_coloum].special_state = 0;
	cellmine[mState.mouse_pos_line+1][mState.mouse_pos_coloum+1].special_state = 0;

	for (int i=-1;i<=1;i++)
		{
			for (int j=-1;j<=1;j++)
			{
				RECT Irect;
				Irect = CreateRect(mState.mouse_pos_coloum+i,mState.mouse_pos_line+j,0,0);	
	            InvalidateRect(hwnd,&Irect,FALSE);
			}
		}
	return TRUE;
    
//	}
}
BOOL DbbuttonupFun(HWND hwnd)
{
	cellmine[mState.mouse_pos_line-1][mState.mouse_pos_coloum-1].special_state = 1;
	cellmine[mState.mouse_pos_line-1][mState.mouse_pos_coloum].special_state = 1;
	cellmine[mState.mouse_pos_line-1][mState.mouse_pos_coloum+1].special_state = 1;
	cellmine[mState.mouse_pos_line][mState.mouse_pos_coloum-1].special_state = 1;
	cellmine[mState.mouse_pos_line][mState.mouse_pos_coloum].special_state = 1;
	cellmine[mState.mouse_pos_line][mState.mouse_pos_coloum+1].special_state = 1;
	cellmine[mState.mouse_pos_line+1][mState.mouse_pos_coloum-1].special_state = 1;
	cellmine[mState.mouse_pos_line+1][mState.mouse_pos_coloum].special_state = 1;
	cellmine[mState.mouse_pos_line+1][mState.mouse_pos_coloum+1].special_state = 1;

	for (int i=-1;i<=1;i++)
		{
			for (int j=-1;j<=1;j++)
			{
				RECT Irect;
				Irect = CreateRect(mState.mouse_pos_coloum+i,mState.mouse_pos_line+j,0,0);	
	            InvalidateRect(hwnd,&Irect,FALSE);
			}
		}
	return TRUE;
	
}


BOOL MoveFun(HWND hwnd)
{
	cellmine[mState.mouse_pos_line-1][mState.mouse_pos_coloum-1].special_state = 0;
	cellmine[mState.mouse_pos_line-1][mState.mouse_pos_coloum].special_state = 0;
	cellmine[mState.mouse_pos_line-1][mState.mouse_pos_coloum+1].special_state = 0;
	cellmine[mState.mouse_pos_line][mState.mouse_pos_coloum-1].special_state = 0;
	cellmine[mState.mouse_pos_line][mState.mouse_pos_coloum].special_state = 0;
	cellmine[mState.mouse_pos_line][mState.mouse_pos_coloum+1].special_state = 0;
	cellmine[mState.mouse_pos_line+1][mState.mouse_pos_coloum-1].special_state = 0;
	cellmine[mState.mouse_pos_line+1][mState.mouse_pos_coloum].special_state = 0;
	cellmine[mState.mouse_pos_line+1][mState.mouse_pos_coloum+1].special_state = 0;

	for (int i=-1;i<=1;i++)
		{
			for (int j=-1;j<=1;j++)
			{
				RECT Irect;
				Irect = CreateRect(mState.mouse_pos_coloum+i,mState.mouse_pos_line+j,0,0);	
	            InvalidateRect(hwnd,&Irect,FALSE);
			}
		}
	return TRUE;
}


	


BOOL ExpandMine (HWND hwnd, int m,int n)
{
	UINT i, j;

	UINT minRow = (m == 0) ? 0 : m - 1;
	UINT maxRow = m + 1;
	UINT minCol = (n == 0) ? 0 : n - 1;
	UINT maxCol = n + 1;
	
	for (i = minRow; i <= maxRow; i++) 
	{
		for (j = minCol; j <= maxCol; j++) 
		{
		
			if (cellmine[i][j].cur_state == 0 && cellmine[i][j].expand_state == 1)
			{				
				cellmine[i][j].expand_state =0;				
												
				showdomain(hwnd,i,j);
	           
				ExpandMine(hwnd,i,j);

			}
			
			

			
		}
 	} 




	return true;
}


BOOL showdomain(HWND hwnd,int dx,int dy)
{
	UINT i,j;
    UINT minRow = (dx == 0) ? 0 : dx - 1;
	UINT maxRow = dx + 1;
	UINT minCol = (dy == 0) ? 0 : dy - 1;
	UINT maxCol = dy + 1;
	
	for (i = minRow; i <= maxRow; i++) 
	{
		for (j = minCol; j <= maxCol; j++) 
		{
				cellmine[i][j].special_state = -1;
				cellmine[i][j].old_state =0;
				zerocount++;
				if (zerocount == minecount)
				{
					MessageBoxA(NULL,TEXT("Bingo"),TEXT("Good boy"),MB_ICONINFORMATION|MB_OK);

				}
				RECT Irect;
				Irect = CreateRect(j,i,0,0);	
	            InvalidateRect(hwnd,&Irect,FALSE);
		}
	}
	return TRUE;

}
