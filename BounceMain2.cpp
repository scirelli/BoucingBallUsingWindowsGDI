// Simple Bouncing ball

// INCLUDES ///////////////////////////////////////////////
#define WIN32_LEAN_AND_MEAN  

#include <windows.h>   // include important windows stuff
#include <windowsx.h> 
#include <mmsystem.h> //needed to play sound. Also have to insert winmm.lib C:\Program Files\Microsoft Visual Studio\VC98\Lib
#include <stdlib.h>
#include <stdio.h>
#include <math.h>

// DEFINES ////////////////////////////////////////////////

// defines for windows 
#define WINDOW_CLASS_NAME "WINBOUNCECLASS"  // windows class name
#define WINDOW_WIDTH  640              // size of window
#define WINDOW_HEIGHT 480

// MACROS /////////////////////////////////////////////////

// these read the keyboard asynchronously
#define KEY_DOWN(vk_code) ((GetAsyncKeyState(vk_code) & 0x8000) ? 1 : 0)
#define KEY_UP(vk_code)   ((GetAsyncKeyState(vk_code) & 0x8000) ? 0 : 1)

// PROTOTYPES /////////////////////////////////////////////


// GLOBALS ////////////////////////////////////////////////
HWND main_window_handle = NULL; // save the window handle
HINSTANCE main_instance = NULL; // save the instance
//create pens
HPEN white_pen; //creates a white pen
HPEN black_pen; //creates a black pen

// FUNCTIONS //////////////////////////////////////////////

//\\\\\\\\\\\\\\\\\\\\\\\\\ WinProc ///////////////////////////////
LRESULT CALLBACK WindowProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
	// this is the main message handler of the system
	PAINTSTRUCT	ps;		   // used in WM_PAINT
	HDC			hdc;	   //creates a handle to the device context

	//find out what the message is
	switch(msg)
	{	
	
	case WM_CREATE: //called when window is created
		{// do initialization stuff here
			
			white_pen= CreatePen(PS_SOLID,0,RGB(255,255,255));
			black_pen= CreatePen(PS_SOLID,0,RGB(0,0,0));

			return(0);
		} break;

	case WM_PAINT: //called when window needs repainting
		{//simply validate the window
		    
			//ValidateRect(hwnd, NULL);
			hdc = BeginPaint(hwnd,&ps);
		    EndPaint(hwnd,&ps);
			
			return(0);
		} break;

	case WM_DESTROY: 
		{// kill the application			
			
			//delete pens, if you dont delete all pens you'll get resource leeks
			DeleteObject(white_pen);
			DeleteObject(black_pen);
			//close the program
			PostQuitMessage(0);

			return(0);
		} break;

	default:break;

    } // end main switch

	// process any messages that we didn't take care of 
	return (DefWindowProc(hwnd, msg, wparam, lparam));

} // end WinProc

//\\\\\\\\\\\\\\\\\\\\\\\\ WINMAIN ////////////////////////////////////////////////

int WINAPI WinMain(	HINSTANCE hinstance, HINSTANCE hprevinstance, LPSTR lpcmdline, int ncmdshow)
{

	WNDCLASS winclass;	// this will hold the class created
	HWND	 hwnd;		// generic window handle
	MSG		 msg;		// generic message
	HDC      hdc;       // generic dc
	//PAINTSTRUCT ps;     // generic paintstruct
	
	float gravity= 0.2f;
	float velocity= 0.0f;
	float friction= 0.995f;
	RECT rect;
	int ball_radius= 10;
	int ball_x=WINDOW_WIDTH/2,
		ball_y= ball_radius; //balls radius because the ball is draw around a point and if didn't add the radius
	                         //you would see half the ball
	
	//fill in the window class stucture
	winclass.style			= CS_DBLCLKS | CS_OWNDC | CS_HREDRAW | CS_VREDRAW;
	winclass.lpfnWndProc	= WindowProc;
	winclass.cbClsExtra		= 0;
	winclass.cbWndExtra		= 0;
	winclass.hInstance		= hinstance;
	winclass.hIcon			= LoadIcon(NULL, IDI_APPLICATION);
	winclass.hCursor		= LoadCursor(NULL, IDC_ARROW);
	winclass.hbrBackground	= (HBRUSH) GetStockObject(BLACK_BRUSH);
	winclass.lpszMenuName	= NULL; //MAKEINTRESOURCE(IDR_MENU1);
	winclass.lpszClassName	= WINDOW_CLASS_NAME;

	// register the window class
	if (!RegisterClass(&winclass))
		return(0);

	// create the window
	if (!(hwnd = CreateWindow(WINDOW_CLASS_NAME, /*class*/
							  "WinX Game Console", /*title*/
							  WS_OVERLAPPEDWINDOW | WS_VISIBLE, /*flags*/
						 	  200,200,	     /*x,y*/
							  WINDOW_WIDTH,  /*width*/
						      WINDOW_HEIGHT, /*height*/
							  NULL,	         /* handle to parent */
							  NULL,	         /* handle to menu */
							  hinstance,     /* handle of the instance */
							  NULL)))	     /* creation parms */
	{
		MessageBox(hwnd, "Window Could not be Created", NULL, MB_OK); //NULL is default for Error
		return(0);
	}

	// save the window handle and instance in a global
	main_window_handle = hwnd;
	main_instance      = hinstance;

	// enter main event loop
	while(1)
	{
		if (PeekMessage(&msg,NULL,0,0,PM_REMOVE)) //gets the first messgae one the message queue
		{ 
			// test if this is a quit message
			if (msg.message == WM_QUIT)
				break;
	
			// translate any accelerator keys
			TranslateMessage(&msg);

			// send the message to the window proc
			DispatchMessage(&msg);
		} // end if
		
		if(KEY_DOWN(VK_ESCAPE))
			PostQuitMessage(0);

		//main game processing goes here
			
		hdc= GetDC(hwnd); //get the device context
		
		//fill in rect struct for erasing ball
		rect.left = ball_x - ball_radius;
		rect.right = ball_x + ball_radius;
		rect.top = ball_y - ball_radius;
		rect.bottom = ball_y + ball_radius;
		
		//Draw erasing ball
		SelectObject(hdc, black_pen);
		SelectObject(hdc, GetStockObject(BLACK_BRUSH));
		Ellipse(hdc,rect.left, rect.top, rect.right, rect.bottom);
		
		//Now move the ball
		velocity+= gravity;
		velocity*= friction;
		ball_y+= (int) velocity;

		//Redraw the ball in its new position
		
		//fill in rect struct for drawing ball
		rect.left = ball_x - ball_radius;
		rect.right = ball_x + ball_radius;
		rect.top = ball_y - ball_radius;
		rect.bottom = ball_y + ball_radius;
		
		//Draw the ball in its new spot
		SelectObject(hdc, white_pen);
		SelectObject(hdc, GetStockObject(WHITE_BRUSH));
		Ellipse(hdc,rect.left, rect.top, rect.right, rect.bottom);

		if(ball_y >= 370) //if the ball hits the ground, bounce back.
		{
			velocity= -velocity;
			ball_y= 370; //need to rest the y position because of >=, the y could be > 370
			
			SelectObject(hdc, GetStockObject(BLACK_BRUSH));
			SelectObject(hdc, black_pen);
			Rectangle(hdc, (WINDOW_WIDTH/2)-10, 370, (WINDOW_WIDTH/2)+20, 390);
		}

		//slow things down a bit
		Sleep(5);
		ReleaseDC(hwnd, hdc);
	
	} // end while

	// return to Windows like this
	return(msg.wParam);

} // end WinMain
