#include <windows.h>
#include "avltree.h"
#include "resource.h"
#include <stack>

using namespace std;

#define WIDTH 1000
#define HEIGHT 500
#define CARIDSIZE 5

BOOL CALLBACK TreeOutputProc(HWND hDlg, UINT iMessage, WPARAM wParam, LPARAM lParam);
BOOL CALLBACK TreeInputProc(HWND hDlg, UINT iMessage, WPARAM wParam, LPARAM lParam);
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
HINSTANCE g_hInst;
LPSTR lpszClass = "Server";

extern void Filesave(char* path);
extern void Fileopen(char *path);

extern AVLTREE car;
int movieNumber; 
list<string> movielist;
HWND tmphWnd;

int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance
	, LPSTR lpszCmdParam, int nCmdShow)
{
	HWND hWnd;
	MSG Message;
	WNDCLASS WndClass;
	g_hInst = hInstance;

	WndClass.cbClsExtra = 0;
	WndClass.cbWndExtra = 0;
	WndClass.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
	WndClass.hCursor = LoadCursor(NULL, IDC_ARROW);
	WndClass.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	WndClass.hInstance = hInstance;
	WndClass.lpfnWndProc = (WNDPROC)WndProc;
	WndClass.lpszClassName = lpszClass;
	WndClass.lpszMenuName = MAKEINTRESOURCE(IDR_MENU1);
	WndClass.style = CS_HREDRAW | CS_VREDRAW;
	RegisterClass(&WndClass);

	hWnd = CreateWindow(lpszClass, lpszClass, WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, CW_USEDEFAULT, WIDTH, HEIGHT,
		NULL, (HMENU)NULL, hInstance, NULL);
	ShowWindow(hWnd, nCmdShow);

	tmphWnd = hWnd;
	while (GetMessage(&Message, 0, 0, 0)) {
		TranslateMessage(&Message);
		DispatchMessage(&Message);
	}
	return Message.wParam;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT iMessage, WPARAM wParam, LPARAM lParam)
{
	char str[100], lpstrFile[100] = "";
	//char filter[] = "Every File(*.*) \0*.*\0Text File\0*.txt;*.doc\0";
	char filter[] = "*.bin\0";
	OPENFILENAME OFN;        // OFN 구조체 선언(파일 열때).
	OPENFILENAME SFN;        // SFN 구조체 선언(저장)               // 하나의 구조체로 해도 상관없지만, 구분되게 보여주기 위해 사용.

	if (iMessage == WM_CREATE)
	{
		
	}
	else if (iMessage == WM_DESTROY)
	{
		PostQuitMessage(0);
		return 0;
	}
	else if (iMessage == WM_COMMAND)
	{
		switch (LOWORD(wParam))
		{
		case ID_FILE_OPEN: //파일열기
			memset(&OFN, 0, sizeof(OPENFILENAME));    // memory 초기화  OFN구조체에 0으로 OPENFILENAME만큼의 크기를 초기화 해준다.
			OFN.lStructSize = sizeof(OPENFILENAME); // OFN 의 구조체크기를 OPENFILENAME의 크기를 넣어준다.
			OFN.hwndOwner = hWnd;              // OFN의 핸들 위치는 Wnd위치를 갖고 있는 핸들러를 갖는다.
			OFN.lpstrFilter = filter;                // 위에서 선언한 filter 파일을 받아 TEXT파일, 워드파일, 모든파일의 확장자를 찾아준다.
			OFN.lpstrFile = lpstrFile;            //열 파일의 이름(위치(?))을 나타내는 듯하다(?)
			OFN.nMaxFile = 100;                // 글자를 쓸수 있는 파일이름의 길이를 나타낸다.
			OFN.lpstrInitialDir = ".";            // 파일의 열기가 실행된 위치

			if (GetOpenFileName(&OFN) != 0)  // OFN(열려고 하는 선택된 파일)을 연다.
			{				
				wsprintf(str, "%s 파일을 열겠습니까?", OFN.lpstrFile);
				MessageBox(hWnd, str, "열기 선택", MB_OK);             // 파일을 열 건지 물어보는 메시지박스 창 생성(OK버튼 하나만 있다.)
				Fileopen(OFN.lpstrFile);
			}
			break;
		case ID_FILE_SAVE: //파일저장
			memset(&SFN, 0, sizeof(OPENFILENAME));         // 초기화를 해주기 때문에, 같은 변수를 사용해도 문제 없을 듯(?) 하다.
			SFN.lStructSize = sizeof(OPENFILENAME);
			SFN.hwndOwner = hWnd;
			SFN.lpstrFilter = filter;
			SFN.lpstrFile = lpstrFile;                      // 저장할 파일 이름(?)
			SFN.nMaxFile = 256;                           // 저장하려는 파일 크기를 256
			SFN.lpstrInitialDir = ".";
			if (GetSaveFileName(&SFN) != 0)          // SFN에 저장할 파일이 있는지 묻는 부분.
			{	
				wsprintf(str, "%s 파일로 저장하겠습니까?", SFN.lpstrFile);
				MessageBox(hWnd, str, "저장하기 선택", MB_OK);
				Filesave(SFN.lpstrFile);				
			}
			break;	
		case ID_DATA_INPUTDATA:
			DialogBox(g_hInst, MAKEINTRESOURCE(IDD_DIALOG_INPUT), hWnd, TreeInputProc);
			break;
		case ID_DATA_PRINTDATA:
			DialogBox(g_hInst, MAKEINTRESOURCE(IDD_DIALOG_OUTPUT), hWnd, TreeOutputProc);
			break;

		}
	
		return 0;
	}
	
	return(DefWindowProc(hWnd, iMessage, wParam, lParam));
}

BOOL CALLBACK TreeOutputProc(HWND hDlg, UINT iMessage, WPARAM wParam, LPARAM lParam)
{
	RECT wrt, crt;
	 

	if (iMessage == WM_INITDIALOG)
	{
		GetWindowRect(GetParent(hDlg), &wrt);
		GetWindowRect(hDlg, &crt);
		SetWindowPos(hDlg, HWND_NOTOPMOST, wrt.left + (wrt.right - wrt.left) / 2 - (crt.right - crt.left) / 2,
			wrt.top + (wrt.bottom - wrt.top) / 2 - (crt.bottom - crt.top) / 2, 0, 0, SWP_NOSIZE);

		HWND hwndList = GetDlgItem(hDlg, IDC_LIST_CARID);

		if (car.getRoot() != NULL)
		{
			int Number = 0;
			stack<NODE*> nodestack;
			nodestack.push(car.getRoot());

			while (nodestack.empty() == false)
			{
				NODE* node = nodestack.top();

				char buf[100] = { 0, };
				sprintf(buf, "%d", node->carData.carnumber);
				int pos = (int)SendMessage(hwndList, LB_ADDSTRING, 0,(LPARAM)buf);
				SendMessage(hwndList, LB_SETITEMDATA, pos, (LPARAM)Number++); //데이터 추가

				nodestack.pop();

				if (node->right)
					nodestack.push(node->right);
				if (node->left)
					nodestack.push(node->left);
			}
		}
		SetFocus(hwndList);
		return TRUE;
	}

	else if (iMessage == WM_COMMAND)
	{
		switch (LOWORD(wParam))
		{
		case IDOK:
		{			
			EndDialog(hDlg, 0);
			return TRUE;
		}
		case IDC_LIST_CARID: //리스트이벤트
			switch (HIWORD(wParam))
			{
			case LBN_SELCHANGE:
				{					
					char selectcar[CARIDSIZE] = { 0, };
					int index;
					HWND hwndList = GetDlgItem(hDlg, IDC_LIST_CARID);

					//인덱스번호 얻어옴
					int lbItem = (int)SendMessage(hwndList, LB_GETCURSEL, 0, 0);
					//데이터 얻어옴					
					SendMessage(hwndList, LB_GETTEXT, (WPARAM)lbItem, (LPARAM)selectcar);					
					sscanf(selectcar, "%d", &index);
					
					NODE* finder = car.Search(car.getRoot(), index);

					SetDlgItemInt(hDlg, IDC_EDIT_sort, finder->carData.carnumber, TRUE); //차종류
					SetDlgItemInt(hDlg, IDC_EDIT_PRICE, finder->carData.totalPrice, TRUE); //총금액
					SetDlgItemText(hDlg, IDC_EDIT_LASTVISIT, finder->carData.lastVisit.c_str()); //마지막방문날짜
					SetDlgItemInt(hDlg, IDC_EDIT_TIME, finder->carData.totalmovieTime, TRUE); //영화시간

					//영화목록
					int ListNumber = 0;
					HWND MovieListhWnd = GetDlgItem(hDlg, IDC_LIST_MOVIE);
					SendMessage(MovieListhWnd, LB_RESETCONTENT, 0, 0);
					list<string>::iterator End = finder->carData.movieName.end();
					for (list<string>::iterator iterPos = finder->carData.movieName.begin();
						iterPos != End; iterPos++)
					{
						int pos = (int)SendMessage(MovieListhWnd, LB_ADDSTRING, 0, (LPARAM)iterPos->c_str());
						SendMessage(MovieListhWnd, LB_SETITEMDATA, pos, (LPARAM)ListNumber++); //데이터 추가
					}			
					break;
				}
			}
			break;
		}		
		return TRUE;
	}
	return FALSE;
}

BOOL CALLBACK TreeInputProc(HWND hDlg, UINT iMessage, WPARAM wParam, LPARAM lParam)
{
	RECT wrt, crt;
	
	if (iMessage == WM_INITDIALOG)
	{
		GetWindowRect(GetParent(hDlg), &wrt);
		GetWindowRect(hDlg, &crt);
		SetWindowPos(hDlg, HWND_NOTOPMOST, wrt.left + (wrt.right - wrt.left) / 2 - (crt.right - crt.left) / 2,
			wrt.top + (wrt.bottom - wrt.top) / 2 - (crt.bottom - crt.top) / 2, 0, 0, SWP_NOSIZE);

		
		return TRUE;
	}

	else if (iMessage == WM_COMMAND)
	{
		
		switch (LOWORD(wParam))
		{
		case ID_INPUT_OK:
			{
				int numOfcar = 0, numOfsort, totalPrice = 0, totalTime = 0;

				char lastVisit[20] = { 0, };
				numOfcar = GetDlgItemInt(hDlg, IDC_EDIT_INPUT_NUMBER, NULL, FALSE);
				numOfsort = GetDlgItemInt(hDlg, IDC_EDIT_INPUT_SORT, NULL, FALSE);
				totalPrice = GetDlgItemInt(hDlg, IDC_EDIT_INPUT_PRICE, NULL, FALSE);
				totalTime = GetDlgItemInt(hDlg, IDC_EDIT_INPUT_TIME, NULL, FALSE);
				GetDlgItemText(hDlg, IDC_EDIT_LASTVISIT, lastVisit, sizeof(lastVisit));
				if (strlen(lastVisit) == 0)	strcpy(lastVisit, "X");
				carArgument t(numOfcar, numOfsort, totalPrice, totalTime, lastVisit, movielist);
				car.Insert(car.getRootAddress(), numOfcar, t);
				MessageBox(hDlg, "입력을 성공했습니다", "ok", MB_OK);
				break;
			}		
		case ID_INPUT_CANCEL:
			{				
				movieNumber = 0;
				movielist.clear();
				EndDialog(hDlg, 0);
				break;
			}
		case IDC_BUTTON_MOVIE_ADD: //영화입력
			{
				char buf[100]; 
				HWND hwndList = GetDlgItem(hDlg, IDC_LIST_MOVIELIST);
				GetDlgItemText(hDlg, IDC_EDIT_INPUT_MOVIE, buf, sizeof(buf));
				int pos = (int)SendMessage(hwndList, LB_ADDSTRING, 0, (LPARAM)buf);
				SendMessage(hwndList, LB_SETITEMDATA, pos, (LPARAM)movieNumber++); //데이터 추가
				//리스트 추가				
				movielist.push_back(buf);
				break;
			}
		case IDC_BUTTON_MOVIE_DEL:
			{
				char deletemovie[100];
				HWND hwndList = GetDlgItem(hDlg, IDC_LIST_MOVIELIST);
				//인덱스번호 얻어옴
				int lbItem = (int)SendMessage(hwndList, LB_GETCURSEL, 0, 0);
				SendMessage(hwndList, LB_GETTEXT, (WPARAM)lbItem, (LPARAM)deletemovie);
				//데이터삭제
				SendMessage(hwndList, LB_DELETESTRING, lbItem, 0);
				//리스트삭제
				movielist.remove(deletemovie);
				break;
			}
		}
		return TRUE;
	}
	return FALSE;
}
