// DRAFT FOR PC HOST APPLICATION
// STRIP ALL COMMUNICATION CODE


// Must define it here, or it wont affect 
#define WINVER 0x0500			
#define _WIN32_IE 0x0500		
#define _WIN32_WINNT 0x0500		
#define CURRENTVERSION 0100 // 1.00
#define WIN32_LEAN_AND_MEAN



#define _DEBUG_ // to use track down bug if needed

// change to macro instead of function
#define SetPicture(hButton, resID)   SendMessage(hButton, BM_SETIMAGE, (WPARAM)IMAGE_BITMAP, (LPARAM)LoadBitmap(hInst, MAKEINTRESOURCE(resID)))
#define SetPictureBMP(hButton, hBitmap)   SendMessage(hButton, BM_SETIMAGE, (WPARAM)IMAGE_BITMAP, (LPARAM)hBitmap)


// Need to link with Ws2_32.lib
#include <winsock2.h>
//#pragma comment (lib, "Ws2_32.lib")


#include <CommCtrl.h>
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")

//#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers
// Windows Header Files:
#include "windows.h"
#include "resource.h"

#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <tchar.h>
#include <Windows.h>
#include <tchar.h>
#include <iostream>
#include <Commdlg.h>
#include "ExtendMonitor.h"

#include "bz_udp_helper.h"
#include "ceezMacro.h"
#include "COLORREF16.h"


#define DEFAULT_BUFLEN 512
#define DEFAULT_PORT 4444
#define MSG_LEN 30
#define MAX_CLIENT 10
#define MAX_ROUND 50

#define TEXTSIZE_SCORE 3
#define TEXTSIZE_NAME 0

#define TIMER_ALIVE_BASE 1000
#define STAY_ALIVE_TIME 12000 //ms = 12s

#define uint8_t BYTE // same as UINT8
#define uint16_t USHORT // same as UINT16
#define sprintfByte(d,c,...) sprintf((char*)d,c,...)

// This will load hConfigDialog in shared common data segment
#pragma comment(linker, "/SECTION:.shr,RWS")
#pragma data_seg(".shr")
DWORD dwShareData = NULL;	// share data if there are multiple instances
#pragma data_seg()


struct HwndBtns
{
	HWND hEnable;
	HWND hEdit;
	HWND hName;
	HWND hScore;
	HWND hPlayer1;
	HWND hPlayer2;
	HWND hAddPnt;
	HWND hSubPnt;
	HWND hMultiChoice;
	//HWND hSelect;
};

struct TeamMem
{
	BOOL available; // client is online
	BOOL selected;  // usually TRUE, unless unselected by PC or user or Buzzer
	BOOL enable;
	BOOL marked;
	BYTE mChoice; //A=1, D=4, E=5, No choice = 0
	INT totalScore;
	HwndBtns hWnd;
};

struct TeamInfo
{
	UINT32 IPADDR_32; //fix with union later
	CHAR name[20];
	CHAR player1[20];
	CHAR player2[20];
};

//enum RewardType : BYTE{ NONE = 0, RTSUB10 = 1, RTSUB15 = 2, RTADD10 = 3, RTADD15 = 4 };

enum RewardType : int { NONE = 0, RTSUB = -1, RTADD = 1};


struct RoundPoint
{
	//INT roundNo; // from 0->max 50
	INT roundPoint;
	RewardType teamReward[MAX_CLIENT]; // [teamID]
};
//struct ScoreHistoryRecord
//{	
//	RoundPoint roundRecord[MAX_ROUND]; // [round]
//};


// struct RoundPointControl
// {
// 	INT round;
// 	INT teamPt[MAX_CLIENT]; //{0,0,0,0,0,-10,0,0,0,0} // only team8 get -10
// 	INT opoint[5]; // 0, -15, -10, +10, +15 // option for point for this round, just extra information
// };

struct DataMem
{	
	int onlineCount;
	TeamMem memTeam[MAX_CLIENT];
	TeamInfo infoTeam[MAX_CLIENT];
	RoundPoint roundRecord[MAX_ROUND+10];
	RewardType memRoundRewardType[MAX_CLIENT];
	int memRoundCount;
	int memRoundPoint;
	int memCurrentRound; // from 0 -> max 50
};

// if using _ipicture.h
// LPPICTURE iPicGlobal;
enum hbList {black, white, red ,blue, green, gray, yellow};
HBRUSH hbCollection[7];


//////////////////////////////////////////////////////////////////////////
// VARIABLE
//////////////////////////////////////////////////////////////////////////
HINSTANCE		hInst;	// The current instance
HWND hMain;	// handle to main dialog

HWND hScoreList;
HWND hScoreEditBtn;
HWND hRoundResetBtn;
HWND hNextRoundBtn;
HWND hPrevRoundBtn;
HWND hApplyBtn;
HWND hLoadDataBtn;
HWND hShowTeamNameBtn;
HWND hShowTeamScoreBtn;
HWND hShowPlayerNameBtn;
HWND hClearPanelsBtn;
HWND hUpdatePanelsBtn;
//HWND hHideTeamNameBtn;
//HWND hHideTeamScoreBtn;
//HWND hHidePlayerNameBtn;
HWND hRainDropsBtn;
HWND hSelectAddBtn;
HWND hSelectSubBtn;

HWND hSelectTeam_MC_A_Btn;
HWND hSelectTeam_MC_B_Btn;
HWND hSelectTeam_MC_C_Btn;
HWND hSelectTeam_MC_D_Btn;
//HWND hSelectTeam_ALL_Btn;
//HWND hSelectTeam_NONE_Btn;
HWND hHostIPLabel;

HBITMAP bmpA;
HBITMAP bmpB;
HBITMAP bmpC;
HBITMAP bmpD;
HBITMAP bmpWhite;
HBITMAP bmpSel;
HBITMAP bmpUnsel;
HBITMAP bmpMark;

HWND hLabelRoundNo;
HWND hLabelScoreRoundPoint;
HWND hExitBtn;

DataMem data;

BOOL marking[MAX_CLIENT];

// for sub dialog
INT gLo_TeamEditIdx;

UDP_HELPER udp;

WNDPROC WP_EditProc;

//////////////////////////////////////////////////////////////////////////
// Forward declarations of functions included in this code module:
// Remember: Functions are special variables
BOOL InitInstance(HINSTANCE, int);
BOOL OnInitDialog(HWND);
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow);

LRESULT CALLBACK MainWndProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam);
LRESULT CALLBACK TeamEditWndProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam);
LRESULT CALLBACK ScoreEditWndProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam);
LRESULT CALLBACK EditProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
LRESULT CALLBACK ScoreTableProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

BOOL loadSession();
BOOL loadNewSession();
BOOL saveDataToSession();


int findChar(char* in_string, char charTofind, int start_index, int stop_index);
int trimSpace(char*& in_string, int size);
int updateAddScoreBtn();
int enableScoreButtons();
int updateTeamBtn(int teamNo);
int enableTeam(int teamNo, bool en);
int readTeamName(char* LineBuffer);
int readScoreData(char* LineBuffer);
int clearMC();
int setTeamMC(int teamNo);
HBRUSH hbOutlineButtons(HWND hBtn);
int selectTeam(int idx, BOOL bSelect);
int clearRound();
int saveRound();
int nextRound();
int loadRound();
int prevRound();
int updateScore();
int rewardScore(INT teamID, RewardType rtpt);
int rewardScoreResID(INT resID);
int createScoreTable(MonRect wndRect);
CHAR returnRTchar(RewardType rt);

//////////////////////////////////////////////////////////////////////////
//Start Main program as win32 application


int APIENTRY _tWinMain(HINSTANCE hInstance,
	HINSTANCE hPrevInstance,
	LPTSTR    lpCmdLine,
	int       nCmdShow)
{
	MSG msg;
	HACCEL hAccelTable;

	 //Check if there is another instance has been loaded
	if (dwShareData)
	{
		MessageBoxW(NULL,L"Chương trình đang hoạt động!\n\rBấm OK để tắt thông báo.",L"Notify",MB_OK);
		return FALSE;  // Quit here
	}

	dwShareData = 1;

	// Perform application initialization:
	if (!InitInstance (hInstance, nCmdShow)) return FALSE; // Quit if init fail
	
	hAccelTable = LoadAccelerators(hInstance, (LPCTSTR)IDR_ACCELERATOR1);
	
	// Main message loop:
	while (GetMessage(&msg, NULL, 0, 0))
	{
		if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg)||
			!IsDialogMessage(msg.hwnd,&msg) ) 
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}
	return (int) msg.wParam;
}


BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
	// Store instance handle and create dialog
	hInst = hInstance;
	hMain = CreateDialog(hInstance, MAKEINTRESOURCE(IDD_MAIN), NULL,(DLGPROC)MainWndProc);
	if (!hMain) return FALSE; //fail to create => exit
	
	ShowWindow(hMain,SW_SHOW);
	
	cMonitorInfo cMonInfo;
	ReadMonitorInfo(cMonInfo);	

	//use monitor index 1 - 2nd monitor
	MonRect rc;
	rc.top = cMonInfo.monRC[1].top;
	rc.left = cMonInfo.monRC[1].left;
	rc.height = cMonInfo.monRC[1].height;
	rc.width = cMonInfo.monRC[1].width;

	createScoreTable(rc);
	ShowWindow( hMain, SW_SHOW);

	udp.begin(REMOTE_PORT);

	char hostips[20];
	sprintf(hostips, "%d.%d.%d.%d", udp.local_IPv4.b32.b1, udp.local_IPv4.b32.b2, udp.local_IPv4.b32.b3, udp.local_IPv4.b32.b4);
	SetText(hHostIPLabel, hostips);
	
	// force using my IPs
	udp.local_IPv4.b32.b1 = 10;
	udp.local_IPv4.b32.b2 = 10;
	udp.local_IPv4.b32.b3 = 10;
	udp.local_IPv4.b32.b4 = 10;



	// for _ipicture.h
	//hBrush = CreateSolidBrush(COLOR_BTNFACE);

	//load JPEG as stream data
	//DWORD dwStreamSize;
	//IStream* pstream = CreateStreamOnResource(MAKEINTRESOURCE(IDR_BIN1),_T("BIN"),dwStreamSize);
	//HRESULT hr = ::OleLoadPicture(pstream, dwStreamSize, FALSE, IID_IPicture, (LPVOID *)&iPicGlobal);

//	CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)BZM_Listener_thread, NULL, NULL, NULL);

	return TRUE;
}


LRESULT CALLBACK ScoreTableProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{

	int wmId, wmEvent;//,i=0;
	
	switch (uMsg)
	{
	case WM_INITDIALOG:
		break;

	case WM_COMMAND:		// generated messages, buttons..
		wmId    = LOWORD(wParam);
		wmEvent = HIWORD(wParam);

		//switch (wmId)
		//{
		return 0;

	}
	return 0;
}


HWND hModalTeamEditPlayer1;
HWND hModalTeamEditPlayer2; 
HWND hModalTeamEditNo; 
HWND hModalTeamEditName; 

LRESULT CALLBACK TeamEditWndProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	int wmId, wmEvent;//,i=0;
	
	switch (message)
	{
	case WM_INITDIALOG:
		hModalTeamEditPlayer1 = GetDlgItem(hDlg, IDC_TE_TEAM_PLAYER1_NAME_EDT);
		hModalTeamEditPlayer2 = GetDlgItem(hDlg, IDC_TE_TEAM_PLAYER2_NAME_EDT);
		hModalTeamEditNo = GetDlgItem(hDlg, IDC_TE_TEAM_NO);
		hModalTeamEditName = GetDlgItem(hDlg, IDC_TE_TEAM_NAME_EDT);
		
		edSetLimitText(hModalTeamEditPlayer1, 20);
		edSetLimitText(hModalTeamEditPlayer2, 20);
		edSetLimitText(hModalTeamEditNo, 20);
		edSetLimitText(hModalTeamEditName, 20);
		SetText(hModalTeamEditName, data.infoTeam[gLo_TeamEditIdx].name);
		SetText(hModalTeamEditPlayer1, data.infoTeam[gLo_TeamEditIdx].player1);
		SetText(hModalTeamEditPlayer2, data.infoTeam[gLo_TeamEditIdx].player2);
		{
			char text[10] = {0};
			sprintf_s(text, "#%d", gLo_TeamEditIdx+1);
			SetText(hModalTeamEditNo, text);
		}
		
		PostMessage(hDlg, WM_NEXTDLGCTL, TRUE, FALSE); //similar to press tab - go to next control
		return 0;

	case WM_SYSCOMMAND:		// catching hit [x]
		if ((wParam & 0xFFF0) == SC_CLOSE)
		{
			// asking yes/no maybe
			// and or clean up stuff
			EndDialog(hDlg, 0);
			return 0; //exit success
		}
		break;

		
	case WM_CLOSE:			//closing down
	case WM_DESTROY:
		// clean up stuff
		EndDialog(hDlg, 0);
		return 0; //exit success

		
	case WM_COMMAND:		// generated messages, buttons..
		wmId    = LOWORD(wParam);
		wmEvent = HIWORD(wParam);
		BYTE sztext[MSG_LEN+1] = {0};
		BYTE szmsg[MSG_LEN+1] = {2,1,5,1,3,0};
		BYTE* pData = &szmsg[3];
		BYTE* nzmsg = &szmsg[2];

		switch (wmId)
		{

		case IDOK: 
			//write back data
			{
				GetText(hModalTeamEditName, data.infoTeam[gLo_TeamEditIdx].name);
				GetText(hModalTeamEditPlayer1, data.infoTeam[gLo_TeamEditIdx].player1);
				GetText(hModalTeamEditPlayer2, data.infoTeam[gLo_TeamEditIdx].player2);
				SetText(data.memTeam[gLo_TeamEditIdx].hWnd.hName, data.infoTeam[gLo_TeamEditIdx].name);
				SetText(data.memTeam[gLo_TeamEditIdx].hWnd.hPlayer1, data.infoTeam[gLo_TeamEditIdx].player1);
				SetText(data.memTeam[gLo_TeamEditIdx].hWnd.hPlayer2, data.infoTeam[gLo_TeamEditIdx].player2);
			}

		case IDCANCEL: 
			gLo_TeamEditIdx = 0;
			EndDialog(hDlg, 0);
			break;
		}
	}
	return 0;
}


HWND hHEScoreLB;
HWND hHEScoreIndex;
HWND hHEScorePoint;
HWND hHETeamName[MAX_CLIENT];
HWND hHETeamRTp[MAX_CLIENT];
HWND hHETeamRTs[MAX_CLIENT];
HWND hHEUpdateBtn;
HWND hHEAddRecord;
HWND hHEDeleteRecord;
HWND hHELabelLastRound;

INT  iHEScoreListIndex;

CHAR returnRTchar(RewardType rt)
{
	CHAR ch='0';
	if (rt == RTADD) return '+';
	if (rt == RTSUB) return '-';
	return ch;
}


LRESULT CALLBACK ScoreEditWndProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	int wmId, wmEvent;//,i=0;
	//INT wPara = wParam - TIMER_ALIVE_BASE;
	
	switch (message)
	{
	case WM_INITDIALOG:
		//PostMessage(hDlg, WM_NEXTDLGCTL, TRUE, FALSE); //similar to press tab - go to next control
		
		hHEScoreLB = GetDlgItem(hDlg, IDC_HE_SCORE_EDIT_LIST);
		hHEScoreIndex = GetDlgItem(hDlg, IDC_HE_SCORE_INDEX);
		hHEScorePoint = GetDlgItem(hDlg, IDC_HE_POINT_SCORED);


		hHEUpdateBtn = GetDlgItem(hDlg, IDC_HE_UPDATE);
		hHEAddRecord = GetDlgItem(hDlg, IDC_HE_ADD_RECORD);
		hHEDeleteRecord = GetDlgItem(hDlg, IDC_HE_DELETE_RECORD);
		hHELabelLastRound = GetDlgItem(hDlg, IDC_HE_LABEL_LAST_ROUND);

		edSetLimitText(hHEScoreIndex, 4);
		edSetLimitText(hHEScorePoint, 4);
		SetText(hHELabelLastRound, data.memCurrentRound);
		
		// team name
		for (int t = 0; t < MAX_CLIENT; t++)
		{
			hHETeamName[t] = GetDlgItem(hDlg, IDC_HE_TEAM1 + t);
			SetText(hHETeamName[t], data.infoTeam[t].name);
			hHETeamRTp[t] = GetDlgItem(hDlg, IDC_HE_CHECK_ADD_T1 + t);
			hHETeamRTs[t] = GetDlgItem(hDlg, IDC_HE_CHECK_SUB_T1 + t);
		}

		// load data to list
		for (int i = 0; i <= MAX_ROUND; i++)
		{
			//Q. XX,   Pt: 10,   1 2 3 4 5 6 7 8
			CHAR rtchar[20];
			ZeroMemory(rtchar, sizeof(rtchar));
			rtchar[0] =	returnRTchar(data.roundRecord[i].teamReward[0]); 
			rtchar[1] =	returnRTchar(data.roundRecord[i].teamReward[1]),
			rtchar[2] =	returnRTchar(data.roundRecord[i].teamReward[2]),
			rtchar[3] =	returnRTchar(data.roundRecord[i].teamReward[3]),
			rtchar[4] =	returnRTchar(data.roundRecord[i].teamReward[4]),
			rtchar[5] =	returnRTchar(data.roundRecord[i].teamReward[5]),
			rtchar[6] =	returnRTchar(data.roundRecord[i].teamReward[6]),
			rtchar[7] =	returnRTchar(data.roundRecord[i].teamReward[7]),
			rtchar[8] =	returnRTchar(data.roundRecord[i].teamReward[8]),
			rtchar[9] =	returnRTchar(data.roundRecord[i].teamReward[9]);
			
			CHAR ztext[50];			
			CHAR padding[10] = "   ";
			sprintf_s(ztext, "Qu. %d, %1s     Pt. %d, %1s     Ans. %s", i, (i < 10) ? padding:"", data.roundRecord[i].roundPoint, (data.roundRecord[i].roundPoint < 10) ? padding : "", rtchar); // Start at Qu. 1, Qu. 0 for reward before the game
			lbInsertItem(hHEScoreLB, ztext);
		}
		// z-index for each record:   t = (int)index / max_client, i = (int)index % max_client

		{
			iHEScoreListIndex = 0;
			lbSetCurSel(hHEScoreLB, iHEScoreListIndex);
			
			CHAR ztext[6];
			
			sprintf_s(ztext, "%d", iHEScoreListIndex);
			SetText(hHEScoreIndex, ztext);

			sprintf_s(ztext, "%d", data.roundRecord[iHEScoreListIndex].roundPoint);
			SetText(hHEScorePoint, ztext);
			
			for (int i=0;i<MAX_CLIENT;i++)
			{
				if (data.roundRecord[iHEScoreListIndex].teamReward[i] == RTADD) cbChecked(hHETeamRTp[i]);
				if (data.roundRecord[iHEScoreListIndex].teamReward[i] == RTSUB) cbChecked(hHETeamRTs[i]);
			}

		}

		return 0;

		// catching hit [x]
// 	case WM_SYSCOMMAND:
// 		if ((wParam & 0xFFF0) == SC_CLOSE)
// 		{
// 			// asking yes/no maybe
// 			// and or clean up stuff
// 			EndDialog(hDlg, 0);
// 			return 0; //exit success
// 		}
// 		break;

		//closing down
	case WM_CLOSE:
	case WM_DESTROY:
		// clean up stuff

		clearRound();
		loadRound();

		EndDialog(hDlg, 0);
		return 0; //exit success

	case WM_CTLCOLORSTATIC:
		SetTextColor((HDC)wParam, RGB(10, 48, 220));	// blue-ish
		//SetBkColor((HDC)wParam, (INT_PTR)hBrStaticBkgrd);
		return (INT_PTR)hbCollection[white];

	case WM_CTLCOLORDLG:
		return (INT_PTR)(hbCollection[white]);

	case WM_CTLCOLORBTN:
		return (INT_PTR) hbOutlineButtons((HWND)lParam);
		break;

		// generated messages, buttons..
	case WM_COMMAND:
		wmId    = LOWORD(wParam);
		wmEvent = HIWORD(wParam);
		BYTE sztext[MSG_LEN+1] = {0};
		BYTE szmsg[MSG_LEN+1] = {2,1,5,1,3,0};
		BYTE* pData = &szmsg[3];
		BYTE* nzmsg = &szmsg[2];
		
		switch (wmId)
		{

		case IDC_HE_UPDATE:
			{
				CHAR pt[6], qn[6];
				ZeroMemory(pt, sizeof(pt));
				ZeroMemory(qn, sizeof(qn));
				GetText(hHEScoreIndex, qn);
				GetText(hHEScorePoint, pt);
				INT idx = atoi(qn);
				INT ipt = atoi(pt);
				data.roundRecord[idx].roundPoint = ipt;

				for (int i=0;i<MAX_CLIENT;i++)
				{
					data.roundRecord[idx].teamReward[i] = NONE;
					if (cbGetCheck(hHETeamRTs[i])) data.roundRecord[idx].teamReward[i] = RTSUB;
					if (cbGetCheck(hHETeamRTp[i])) data.roundRecord[idx].teamReward[i] = RTADD;
				}

				// update HE list
				lbRemoveItem(hHEScoreLB,idx);
				
				CHAR rtchar[11];
				ZeroMemory(rtchar, sizeof(rtchar));
				rtchar[0] =	returnRTchar(data.roundRecord[idx].teamReward[0]); 
				rtchar[1] =	returnRTchar(data.roundRecord[idx].teamReward[1]),
				rtchar[2] =	returnRTchar(data.roundRecord[idx].teamReward[2]),
				rtchar[3] =	returnRTchar(data.roundRecord[idx].teamReward[3]),
				rtchar[4] =	returnRTchar(data.roundRecord[idx].teamReward[4]),
				rtchar[5] =	returnRTchar(data.roundRecord[idx].teamReward[5]),
				rtchar[6] =	returnRTchar(data.roundRecord[idx].teamReward[6]),
				rtchar[7] =	returnRTchar(data.roundRecord[idx].teamReward[7]),
				rtchar[8] =	returnRTchar(data.roundRecord[idx].teamReward[8]),
				rtchar[9] =	returnRTchar(data.roundRecord[idx].teamReward[9]);

				CHAR ztext[50];			
				sprintf_s(ztext, "Qu. %d,     Pt. %d,      Ans. %s", idx, data.roundRecord[idx].roundPoint, rtchar);
				lbInsertItemZ(hHEScoreLB, idx, ztext);

				clearRound();
				loadRound();
			}			
			break;

		case IDC_HE_CHECK_ADD_T1:
		case IDC_HE_CHECK_ADD_T2:
		case IDC_HE_CHECK_ADD_T3:
		case IDC_HE_CHECK_ADD_T4:
		case IDC_HE_CHECK_ADD_T5:
		case IDC_HE_CHECK_ADD_T6:
		case IDC_HE_CHECK_ADD_T7:
		case IDC_HE_CHECK_ADD_T8:
		case IDC_HE_CHECK_ADD_T9:
		case IDC_HE_CHECK_ADD_T10:
			if (cbGetCheck(hHETeamRTp[wmId - IDC_HE_CHECK_ADD_T1])) 
				cbUnChecked(hHETeamRTs[wmId - IDC_HE_CHECK_ADD_T1]);
			break;

		case IDC_HE_CHECK_SUB_T1:
		case IDC_HE_CHECK_SUB_T2:
		case IDC_HE_CHECK_SUB_T3:
		case IDC_HE_CHECK_SUB_T4:
		case IDC_HE_CHECK_SUB_T5:
		case IDC_HE_CHECK_SUB_T6:
		case IDC_HE_CHECK_SUB_T7:
		case IDC_HE_CHECK_SUB_T8:
		case IDC_HE_CHECK_SUB_T9:
		case IDC_HE_CHECK_SUB_T10:
			if (cbGetCheck(hHETeamRTs[wmId - IDC_HE_CHECK_SUB_T1])) 
				cbUnChecked(hHETeamRTp[wmId - IDC_HE_CHECK_SUB_T1]);
			break;

		case IDC_HE_SCORE_EDIT_LIST:
			if (wmEvent == LBN_SELCHANGE)
			{
				iHEScoreListIndex = lbGetCurSel(hHEScoreLB);
				CHAR ztext[50];

				sprintf_s(ztext, "%d", iHEScoreListIndex);
				SetText(hHEScoreIndex, ztext);

				sprintf_s(ztext, "%d", data.roundRecord[iHEScoreListIndex].roundPoint);
				SetText(hHEScorePoint, ztext);

				for (int i=0;i<MAX_CLIENT;i++)
				{
					switch(data.roundRecord[iHEScoreListIndex].teamReward[i])
					{
					case RTADD:
						cbChecked(hHETeamRTp[i]);
						cbUnChecked(hHETeamRTs[i]);
						break;
					case RTSUB:
						cbChecked(hHETeamRTs[i]);
						cbUnChecked(hHETeamRTp[i]);
						break;
					default:
						cbUnChecked(hHETeamRTs[i]);
						cbUnChecked(hHETeamRTp[i]);
						break;
					}
				}
				
			}

			return 0;

		case IDOK: 
		case IDCANCEL: 
			gLo_TeamEditIdx = 0;
			EndDialog(hDlg, 0);
			break;
		}
	}
	return 0;
}

WNDPROC oldEditProc;

LRESULT CALLBACK subEditProc(HWND wnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg)
	{
	case WM_KEYDOWN:
		switch (wParam)
		{
		case VK_RETURN:
			CHAR rPtztext[8];
			//GetText(hScoreRoundPointEdt, rPtztext);
			data.memRoundPoint= atoi(rPtztext);
			
			sprintf_s(rPtztext, "+%d", data.memRoundPoint);
 			SetText(hSelectAddBtn, rPtztext);
			sprintf_s(rPtztext, "-%d", data.memRoundPoint);
			SetText(hSelectSubBtn, rPtztext);
			updateAddScoreBtn();

			break;  //or return 0; if you don't want to pass it further to def proc
			//If not your key, skip to default:
		}
	default:
		return CallWindowProc(oldEditProc, wnd, msg, wParam, lParam);
	}
	return 0;
}

bool dragWindow;
RECT old_dlg_rect;
POINT old_mouse_point;

// Message handler
LRESULT CALLBACK MainWndProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	int wmId, wmEvent;//,i=0;
	//INT wPara = wParam - TIMER_ALIVE_BASE;
	INT_PTR success;
	HDC hdc;

	// for sending UDP
	BZ_DATAGRAM dx;
	IPV4_ADDR clientButton_addr;

	switch (message)
	{

	case WM_ERASEBKGND:
	{
		HDC hdc = (HDC)(wParam);
		RECT rc; GetClientRect(hDlg, & rc);
		HBRUSH brush = CreateSolidBrush(RGB(245, 242, 244));//Green
		FillRect(hdc, &rc, brush);
		DeleteObject(brush); // Free the created brush: see note below!
		return TRUE;
	}

	case WM_LBUTTONDOWN:
		dragWindow = true;
		SetCapture(hDlg);
		GetWindowRect(hDlg, &old_dlg_rect);
		old_mouse_point.x = (int)(short)LOWORD(lParam);
		old_mouse_point.y = (int)(short)HIWORD(lParam);
		break;

	case WM_LBUTTONUP:
		ReleaseCapture();
		dragWindow = false;
		break;

	case WM_MOUSEMOVE:
		if (dragWindow == true)
		{
			POINT currentpos;
			GetCursorPos(&currentpos);
			int x = currentpos.x - old_mouse_point.x;
			int y = currentpos.y - old_mouse_point.y;
			MoveWindow(hDlg, x, y, old_dlg_rect.right - old_dlg_rect.left, old_dlg_rect.bottom - old_dlg_rect.top, false);

		}
		break;

	case WM_INITDIALOG:
		//PostMessage(hDlg, WM_NEXTDLGCTL, TRUE, FALSE); //similar to press tab - go to next control to set focus on hESerial

		ZeroMemory(&data, sizeof(data));

		for (int i = 0;i<10;i++)
		{
			data.memTeam[i].hWnd.hEnable = GetDlgItem(hDlg, IDC_ENABLE_1 + i);
			data.memTeam[i].hWnd.hEdit = GetDlgItem(hDlg, IDC_TM_EDT1 + i);
			data.memTeam[i].hWnd.hName = GetDlgItem(hDlg, IDC_TEAM_NAME1 + i);
			data.memTeam[i].hWnd.hScore = GetDlgItem(hDlg, IDC_TEAM_SCORE1 + i);
			data.memTeam[i].hWnd.hPlayer1 = GetDlgItem(hDlg, IDC_PLAYER1_TEAM1 + i); 
			data.memTeam[i].hWnd.hPlayer2 = GetDlgItem(hDlg, IDC_PLAYER2_TEAM1 + i); 
			data.memTeam[i].hWnd.hAddPnt = GetDlgItem(hDlg, IDC_SCORE_ADD_T1 + i);
			data.memTeam[i].hWnd.hSubPnt = GetDlgItem(hDlg, IDC_SCORE_SUB_T1 + i);
			data.memTeam[i].hWnd.hMultiChoice = GetDlgItem(hDlg, IDC_MC_T1 + i);
			//data.memTeam[i].hWnd.hSelect = GetDlgItem(hDlg, IDC_SELECT_T1 + i);
			
			data.memTeam[i].enable = FALSE;
			updateTeamBtn(i);
			enableTeam(i, 0);
			setTeamMC(i);
			selectTeam(i, FALSE);
			// get local IP of this PC and assign to data.team[i].IPADDR_32
		}


		hScoreList = GetDlgItem(hDlg, IDC_SCORE_LIST);
		hScoreEditBtn = GetDlgItem(hDlg, IDC_EDIT_SCORE);
		hRoundResetBtn = GetDlgItem(hDlg, IDC_RESET_ROUND);
		hNextRoundBtn = GetDlgItem(hDlg, IDC_NEXT_ROUND);
		hPrevRoundBtn = GetDlgItem(hDlg, IDC_PREV_ROUND);
		hApplyBtn = GetDlgItem(hDlg, IDC_APPLY_ROUND);
		hLoadDataBtn = GetDlgItem(hDlg, IDC_LOAD_DATA);
		hShowTeamNameBtn = GetDlgItem(hDlg, IDC_SHOW_TEAMNAME);
		hClearPanelsBtn = GetDlgItem(hDlg, IDC_CLEAR_PANELS);
		hShowTeamScoreBtn = GetDlgItem(hDlg, IDC_SHOW_TEAMSCORE);
		hUpdatePanelsBtn = GetDlgItem(hDlg, IDC_UPDATE_PANELS);
		hRainDropsBtn = GetDlgItem(hDlg, IDC_RAINDROPS);
		hShowPlayerNameBtn = GetDlgItem(hDlg, IDC_SHOW_PLAYERNAMES);
		hHostIPLabel = GetDlgItem(hDlg, IDC_STATIC_HOST_IP);

 		hSelectAddBtn = GetDlgItem(hDlg, IDC_SELECTIVE_ADD);
		hSelectSubBtn = GetDlgItem(hDlg, IDC_SELECTIVE_SUB);

		hSelectTeam_MC_A_Btn = GetDlgItem(hDlg, IDC_SELECT_MC_A);
		hSelectTeam_MC_B_Btn = GetDlgItem(hDlg, IDC_SELECT_MC_B);
		hSelectTeam_MC_C_Btn = GetDlgItem(hDlg, IDC_SELECT_MC_C);
		hSelectTeam_MC_D_Btn = GetDlgItem(hDlg, IDC_SELECT_MC_D); 
		//hSelectTeam_ALL_Btn = GetDlgItem(hDlg, IDC_SELECT_ALL);
		//hSelectTeam_NONE_Btn = GetDlgItem(hDlg, IDC_SELECT_NONE);

 		//hScoreRoundPointEdt = GetDlgItem(hDlg, IDC_EDIT_ROUND_POINT);
 		hLabelRoundNo = GetDlgItem(hDlg, IDC_LABEL_ROUND_NO);
		hLabelScoreRoundPoint = GetDlgItem(hDlg, IDC_LABEL_ROUND_POINT);

		hExitBtn = GetDlgItem(hDlg, IDC_EXIT);

 		//edSetLimitText(hScoreRoundPointEdt,4); 

 		//oldEditProc = (WNDPROC)SetWindowLongPtr(hScoreRoundPointEdt, GWLP_WNDPROC, (LONG_PTR)subEditProc);
		
		//interface initial values
 		data.memRoundPoint = 10;
 		//SetText(hScoreRoundPointEdt, "+10"); 

		updateAddScoreBtn();

		hbCollection[black] = CreateSolidBrush(RGB(0, 0, 0));
		hbCollection[white] = CreateSolidBrush(RGB(255, 255, 255));
		hbCollection[red] = CreateSolidBrush(RGB(255, 0, 0));
		hbCollection[blue] = CreateSolidBrush(RGB(0, 212, 255));
		hbCollection[green] = CreateSolidBrush(RGB(0, 255, 42));
		hbCollection[gray] = CreateSolidBrush(RGB(203, 203, 203));
		hbCollection[yellow] = CreateSolidBrush(RGB(243, 239, 0));
		
		clearMC();
		loadSession();
		
		bmpA=LoadBitmap(hInst, MAKEINTRESOURCE(IDB_A));
		bmpB=LoadBitmap(hInst, MAKEINTRESOURCE(IDB_B));
		bmpC=LoadBitmap(hInst, MAKEINTRESOURCE(IDB_C));
		bmpD=LoadBitmap(hInst, MAKEINTRESOURCE(IDB_D));
		bmpWhite=LoadBitmap(hInst, MAKEINTRESOURCE(IDB_WHITE));
		bmpSel=LoadBitmap(hInst, MAKEINTRESOURCE(IDB_SEL));
		bmpUnsel=LoadBitmap(hInst, MAKEINTRESOURCE(IDB_UNSEL));
		bmpMark=LoadBitmap(hInst, MAKEINTRESOURCE(IDB_MARKED));
		
		SetPicture(hExitBtn, IDB_CLOSE);
		SetPicture(hApplyBtn, IDB_APPLYDATA);
		SetPicture(hNextRoundBtn, IDB_NEXTROUND);
		SetPicture(hPrevRoundBtn, IDB_PREVROUND);
		SetPicture(hRoundResetBtn, IDB_RESETROUND);
		SetPicture(hLoadDataBtn, IDB_LOADDATA);
		SetPicture(hRainDropsBtn, IDB_RAINDROPS);
		//SetPicture(hShowTeamNameBtn, IDB_SHOWNAME);
		//SetPicture(hShowTeamScoreBtn, IDB_SHOWSCORE);
		SetPicture(hScoreEditBtn, IDB_EDITSCORE);
		//SetPicture(hHideTeamNameBtn, IDB_HIDENAME);
		//SetPicture(hHideTeamScoreBtn, IDB_HIDESCORE);
		//SetPicture(hHidePlayerNameBtn, IDB_HIDEPLAYER);


		SetPictureBMP(hSelectTeam_MC_A_Btn, bmpA);
		SetPictureBMP(hSelectTeam_MC_B_Btn, bmpB);
		SetPictureBMP(hSelectTeam_MC_C_Btn, bmpC);
		SetPictureBMP(hSelectTeam_MC_D_Btn, bmpD);
		//SetPicture(hSelectTeam_ALL_Btn, IDB_ALL);
		//SetPicture(hSelectTeam_NONE_Btn, IDB_NONE);

		clearRound();
		loadRound();
		updateScore();
		return 0;

		// catching hit [x]
	case WM_SYSCOMMAND:
		if ((wParam & 0xFFF0) == SC_CLOSE)
		{
			// asking yes/no maybe
			// and or clean up stuff
			PostQuitMessage(0);
			return 0; //exit success
		}
		break;

		//closing down
	case WM_CLOSE:
	case WM_DESTROY:
		// clean up stuff
		PostQuitMessage(0);
		return 0; //exit success

		//in case I need timer
	case WM_TIMER:
//		CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)BZM_TimerCall, (LPVOID) wPara, 0, NULL);		
		break;

		//if I want to change text color
	case WM_CTLCOLORSTATIC:
			SetTextColor((HDC)wParam, RGB(10, 48, 220));	// blue-ish
			SetBkColor((HDC)wParam, RGB(245, 242, 244));
			return (INT_PTR)CreateSolidBrush(RGB(245, 242, 244));;

	case WM_CTLCOLORDLG:
		SetBkColor((HDC)wParam, RGB(245, 242, 244));
		return (INT_PTR)(hbCollection[white]);

	case WM_CTLCOLORBTN:
		SetBkColor((HDC)wParam, RGB(245, 242, 244));
		return (INT_PTR)RGB(245, 242, 244);//hbOutlineButtons((HWND)lParam);
		break;

		//case WM_CT

		// generated messages, buttons..
	case WM_COMMAND:
		wmId    = LOWORD(wParam);
		wmEvent = HIWORD(wParam);
		switch (wmId)
		{
		case IDC_EXIT:
			saveDataToSession();
			PostQuitMessage(0);
			return 0;

		case IDC_TM_EDT1:
		case IDC_TM_EDT2:
		case IDC_TM_EDT3:
		case IDC_TM_EDT4:
		case IDC_TM_EDT5:
		case IDC_TM_EDT6:
		case IDC_TM_EDT7:
		case IDC_TM_EDT8:
		case IDC_TM_EDT9:
		case IDC_TM_EDT10:
			if (wmEvent != BN_CLICKED) break;
			gLo_TeamEditIdx = wmId - IDC_TM_EDT1;
			success = DialogBox(hInst,MAKEINTRESOURCE(IDD_TEAMEDIT), hDlg,(DLGPROC)TeamEditWndProc);
			break;

		case IDC_ENABLE_1:
		case IDC_ENABLE_2:
		case IDC_ENABLE_3:
		case IDC_ENABLE_4:
		case IDC_ENABLE_5:
		case IDC_ENABLE_6:
		case IDC_ENABLE_7:
		case IDC_ENABLE_8:
		case IDC_ENABLE_9:
		case IDC_ENABLE_10:
			data.memTeam[wmId -IDC_ENABLE_1].enable = !data.memTeam[wmId -IDC_ENABLE_1].enable;
			enableTeam(wmId -IDC_ENABLE_1, data.memTeam[wmId -IDC_ENABLE_1].enable);
			break;

		//process add point buttons
		case IDC_SCORE_ADD_T1:
		case IDC_SCORE_ADD_T2:
		case IDC_SCORE_ADD_T3:
		case IDC_SCORE_ADD_T4:
		case IDC_SCORE_ADD_T5:
		case IDC_SCORE_ADD_T6:
		case IDC_SCORE_ADD_T7:
		case IDC_SCORE_ADD_T8:
		case IDC_SCORE_ADD_T9:
		case IDC_SCORE_ADD_T10:
			rewardScore(wmId - IDC_SCORE_ADD_T1, RTADD);
			break;

		case IDC_SCORE_SUB_T1:
		case IDC_SCORE_SUB_T2:
		case IDC_SCORE_SUB_T3:
		case IDC_SCORE_SUB_T4:
		case IDC_SCORE_SUB_T5:
		case IDC_SCORE_SUB_T6:
		case IDC_SCORE_SUB_T7:
		case IDC_SCORE_SUB_T8:
		case IDC_SCORE_SUB_T9:
		case IDC_SCORE_SUB_T10:
			rewardScore(wmId - IDC_SCORE_SUB_T1, RTSUB);
			break;

/*
		case IDC_SELECTIVE_ADD:
			for (int i = 0;i<MAX_CLIENT;i++)
				if (data.memTeam[i].selected) rewardScore(i, RTADD);
			
			break;

		case IDC_SELECTIVE_SUB:
			for (int i = 0;i<MAX_CLIENT;i++)
				if (data.memTeam[i].selected) rewardScore(i, RTSUB);
			break;
*/

		case IDC_LOAD_DATA:
			/////
			loadNewSession();
			data.memCurrentRound = 0;
			clearRound();
			loadRound();
			break;

		case IDC_APPLY_ROUND:
			nextRound();
			loadRound();
			prevRound();
			loadRound();
			break;

		case IDC_NEXT_ROUND:
			if (data.memCurrentRound>=MAX_ROUND) return 0;
			nextRound();
			loadRound();
			break;
		case IDC_PREV_ROUND:
			if (data.memCurrentRound==0) return 0;
			prevRound();
			loadRound();
			break;

		case IDC_RESET_ROUND:
			clearRound();
			nextRound();
			loadRound();
			prevRound();
			loadRound();
			break;


		//update name to team panels and display it
		case IDC_SHOW_TEAMNAME:
			
			dx.mmsgid = M_SHOW_TEXT;
			dx.extra.ul32 = 0;

			clientButton_addr.ul32 = udp.local_IPv4.ul32;
			clientButton_addr.b32.b4 = 0;

			for (int i = 0; i< 10; i++)
			{
				clientButton_addr.b32.b4 = 101 + i; //start from ip 101 up to ip 110
				
				dx.mmsgid = M_SET_TEXT;				
				strcpy((char*)dx.data, data.infoTeam[i].name);
				udp.send(dx, clientButton_addr, REMOTE_PORT);
				Sleep(5);
				dx.mmsgid = M_SHOW_TEXT;
				udp.send(dx, clientButton_addr, REMOTE_PORT);
				Sleep(5);
			}			
			break;
			
		case IDC_PLAYER1_TEAM1:
		case IDC_PLAYER1_TEAM2:
		case IDC_PLAYER1_TEAM3:
		case IDC_PLAYER1_TEAM4:
		case IDC_PLAYER1_TEAM5:
		case IDC_PLAYER1_TEAM6:
		case IDC_PLAYER1_TEAM7:
		case IDC_PLAYER1_TEAM8:
		case IDC_PLAYER1_TEAM9:
		case IDC_PLAYER1_TEAM10:
			memzero(dx);
			dx.mmsgid = M_CLEARSCENTER;
			
			clientButton_addr.ul32 = udp.local_IPv4.ul32;
			clientButton_addr.b32.b4 = 101 + wmId - IDC_PLAYER1_TEAM1; //100 is the base ADD, client 1 = 101

			udp.send(dx, clientButton_addr, REMOTE_PORT);
			Sleep(5);
			
			dx.mmsgid = M_SHOW_INSTANT_TEXT;
			dx.extra.cxtxt.cref = CR_ORANGE;
			dx.extra.cxtxt.type = TT_FOOTER1;

			strcpy((char*)dx.data, data.infoTeam[wmId - IDC_PLAYER1_TEAM1].player1);
			udp.send(dx, clientButton_addr, REMOTE_PORT);
			Sleep(5);
			break;


		case IDC_PLAYER2_TEAM1:
		case IDC_PLAYER2_TEAM2:
		case IDC_PLAYER2_TEAM3:
		case IDC_PLAYER2_TEAM4:
		case IDC_PLAYER2_TEAM5:
		case IDC_PLAYER2_TEAM6:
		case IDC_PLAYER2_TEAM7:
		case IDC_PLAYER2_TEAM8:
		case IDC_PLAYER2_TEAM9:
		case IDC_PLAYER2_TEAM10:
			
			memzero(dx);
			dx.mmsgid = M_CLEARSCENTER;

			clientButton_addr.ul32 = udp.local_IPv4.ul32;
			clientButton_addr.b32.b4 = 101 + wmId - IDC_PLAYER2_TEAM1; //100 is the base ADD, client 1 = 101

			udp.send(dx, clientButton_addr, REMOTE_PORT);
			Sleep(5);

			dx.mmsgid = M_SHOW_INSTANT_TEXT;
			dx.extra.cxtxt.cref = CR_ORANGE;
			dx.extra.cxtxt.type = TT_FOOTER1;
			
			strcpy((char*)dx.data, data.infoTeam[wmId - IDC_PLAYER2_TEAM1].player2);
			udp.send(dx, clientButton_addr, REMOTE_PORT);
			Sleep(5);
			
			break;

		case IDC_SELECT_ALL:
			for (int i = 0; i < MAX_CLIENT; i++)
				if (data.memTeam[i].enable && !data.memTeam[i].marked) selectTeam(i, TRUE);
			break;

		case IDC_SELECT_NONE:
			for (int i = 0; i < MAX_CLIENT; i++)
				if (data.memTeam[i].enable && !data.memTeam[i].marked) selectTeam(i, FALSE);
			break;


		case IDC_SHOW_TEAMSCORE:
			memzero(dx);
			dx.mmsgid = M_SHOW_INSTANT_TEXT;
			dx.extra.cxtxt.cref = CR_MAROON;
			dx.extra.cxtxt.size = TEXTSIZE_SCORE;
			dx.extra.cxtxt.type = TT_FOOTER;

			clientButton_addr.ul32 = udp.local_IPv4.ul32;
			clientButton_addr.b32.b4 = 0;

			for (int i = 0; i< 10; i++)
			{
				clientButton_addr.b32.b4 = 101 + i; //start from ip 101 up to ip 110
				sprintf((char*)dx.data, "%d\0", data.memTeam[i].totalScore);
				udp.send(dx, clientButton_addr, REMOTE_PORT);
				Sleep(5);
			}
			break;

		case IDC_CLEAR_PANELS:
			memzero(dx);
			dx.mmsgid = M_CLEARSCREEN;

			clientButton_addr.ul32 = udp.local_IPv4.ul32;
			clientButton_addr.b32.b4 = 0;

			for (int i = 0; i< 10; i++)
			{
				clientButton_addr.b32.b4 = 101 + i; //start from ip 101 up to ip 110
				udp.send(dx, clientButton_addr, REMOTE_PORT);
				Sleep(5);
			}
			break;

		case IDC_CLEAR_CENTER:
			memzero(dx);
			dx.mmsgid = M_CLEARSCENTER;

			clientButton_addr.ul32 = udp.local_IPv4.ul32;
			clientButton_addr.b32.b4 = 0;

			for (int i = 0; i< 10; i++)
			{
				clientButton_addr.b32.b4 = 101 + i; //start from ip 101 up to ip 110
				udp.send(dx, clientButton_addr, REMOTE_PORT);
				Sleep(5);
			}
			break;

		case IDC_SHOW_PLAYERNAMES:
			memzero(dx);
			dx.extra.cxtxt.size = 0;
			
			clientButton_addr.ul32 = udp.local_IPv4.ul32;
			clientButton_addr.b32.b4 = 0;

			for (int i = 0; i< 10; i++)
			{
				clientButton_addr.b32.b4 = 101 + i; //start from ip 101 up to ip 110
				
				dx.mmsgid = M_SHOW_INSTANT_TEXT;
				dx.extra.cxtxt.cref = CR_ORANGE;
				dx.extra.cxtxt.type = TT_FOOTER1;
				strcpy((char*)dx.data, data.infoTeam[i].player1);
				udp.send(dx, clientButton_addr, REMOTE_PORT);
				Sleep(5);

				dx.extra.cxtxt.cref = CR_PURPLE;
				dx.extra.cxtxt.type = TT_FOOTER2;
				strcpy((char*)dx.data, data.infoTeam[i].player2);
				udp.send(dx, clientButton_addr, REMOTE_PORT);
				Sleep(5);
			}			
			break;

		case IDC_UPDATE_PANELS:
			// update team name
			// update player names
			// update host IP
			memzero(dx);
			dx.mmsgid = M_SET_TEXT;
			dx.extra.ul32 = 0;

			clientButton_addr.ul32 = udp.local_IPv4.ul32;
			clientButton_addr.b32.b4 = 0;

			for (int i = 0; i< 10; i++)
			{
				clientButton_addr.b32.b4 = 101 + i; //start from ip 101 up to ip 110
				dx.extra.cxtxt.type = TT_TEAM;
				strcpy((char*)dx.data, data.infoTeam[i].name);
				udp.send(dx, clientButton_addr, REMOTE_PORT);
				Sleep(5);

				dx.extra.cxtxt.type = TT_PLAYER1;
				strcpy((char*)dx.data, data.infoTeam[i].player1);
				udp.send(dx, clientButton_addr, REMOTE_PORT);
				Sleep(5);

				dx.extra.cxtxt.type = TT_PLAYER2;
				strcpy((char*)dx.data, data.infoTeam[i].player2);
				udp.send(dx, clientButton_addr, REMOTE_PORT);
				Sleep(5);

				dx.mmsgid = M_HOST_IP;
				dx.extra.ul32 = udp.local_IPv4.ul32;
				memzero(dx.data);
				udp.send(dx, clientButton_addr, REMOTE_PORT);
				Sleep(5);
			}
			break;
			
		// use as slideshow on another monitor
		case IDC_SCORE_TABLE_BTN:
			break;

		
		case IDC_EDIT_SCORE:
			if (wmEvent != BN_CLICKED) break;
			success = DialogBox(hInst,MAKEINTRESOURCE(IDD_HISTORY), hDlg,(DLGPROC)ScoreEditWndProc);
			break;
		}
	}
	return 0;
}

int selectTeam(int i, BOOL bSelect)
{
	//data.memTeam[i].selected = bSelect;
	//if (bSelect) SetPictureBMP(data.memTeam[i].hWnd.hSelect, bmpSel);
	//else SetPictureBMP(data.memTeam[i].hWnd.hSelect, bmpUnsel);	
	RedrawWindow(data.memTeam[i].hWnd.hName, NULL, NULL, RDW_INVALIDATE);
	RedrawWindow(data.memTeam[i].hWnd.hEnable, NULL, NULL, RDW_INVALIDATE);
	RedrawWindow(data.memTeam[i].hWnd.hEdit, NULL, NULL, RDW_INVALIDATE);
	RedrawWindow(data.memTeam[i].hWnd.hScore, NULL, NULL, RDW_INVALIDATE);
	RedrawWindow(data.memTeam[i].hWnd.hPlayer1, NULL, NULL, RDW_INVALIDATE);
	RedrawWindow(data.memTeam[i].hWnd.hPlayer2, NULL, NULL, RDW_INVALIDATE);
	RedrawWindow(data.memTeam[i].hWnd.hAddPnt, NULL, NULL, RDW_INVALIDATE);
	RedrawWindow(data.memTeam[i].hWnd.hSubPnt, NULL, NULL, RDW_INVALIDATE);
	//RedrawWindow(data.memTeam[i].hWnd.hSelect, NULL, NULL, RDW_INVALIDATE);
	EnableWindow(data.memTeam[i].hWnd.hAddPnt, bSelect);
	EnableWindow(data.memTeam[i].hWnd.hSubPnt, bSelect);
	SetPictureBMP(data.memTeam[i].hWnd.hMultiChoice, bmpWhite);
	// mark button as RED or GREEN OR BLUE, whatever

	return 0; //team is not selected
}

HBRUSH hbOutlineButtons(HWND hBtn)
{
	if (hExitBtn == hBtn) return hbCollection[red];
	if (hBtn == hSelectAddBtn) return hbCollection[green];
	if (hBtn == hSelectSubBtn) return hbCollection[yellow];

	for (int i=0;i<MAX_CLIENT; i++)
	{
		if (data.memTeam[i].selected)
		{
			if (hBtn == data.memTeam[i].hWnd.hEnable) return hbCollection[red];
			if (hBtn == data.memTeam[i].hWnd.hEdit) return hbCollection[red];
			if (hBtn == data.memTeam[i].hWnd.hName) return hbCollection[red];
			if (hBtn == data.memTeam[i].hWnd.hScore) return hbCollection[red];
			if (hBtn == data.memTeam[i].hWnd.hPlayer1) return hbCollection[red];
			if (hBtn == data.memTeam[i].hWnd.hPlayer2) return hbCollection[red];
			if (hBtn == data.memTeam[i].hWnd.hAddPnt) return hbCollection[red];
			if (hBtn == data.memTeam[i].hWnd.hSubPnt) return hbCollection[red];
			if (hBtn == data.memTeam[i].hWnd.hMultiChoice) return hbCollection[red];
			//if (hBtn == data.memTeam[i].hWnd.hSelect) return hbCollection[red];
		}
		else
		{
			if (hBtn == data.memTeam[i].hWnd.hEnable) return hbCollection[white];
			if (hBtn == data.memTeam[i].hWnd.hEdit) return hbCollection[white];
			if (hBtn == data.memTeam[i].hWnd.hName) return hbCollection[white];
			if (hBtn == data.memTeam[i].hWnd.hScore) return hbCollection[white];
			if (hBtn == data.memTeam[i].hWnd.hPlayer1) return hbCollection[white];
			if (hBtn == data.memTeam[i].hWnd.hPlayer2) return hbCollection[white];
			if (hBtn == data.memTeam[i].hWnd.hAddPnt) return hbCollection[white];
			if (hBtn == data.memTeam[i].hWnd.hSubPnt) return hbCollection[white];
			if (hBtn == data.memTeam[i].hWnd.hMultiChoice) return hbCollection[white];
			//if (hBtn == data.memTeam[i].hWnd.hSelect) return hbCollection[white];
		}
		
	}
	return hbCollection[white];
}

int setTeamMC(int teamNo) // 0 = clear, 1=A .. 4=D
{
	switch(data.memTeam[teamNo].mChoice)
	{
	case 0:
		SetPictureBMP(data.memTeam[teamNo].hWnd.hMultiChoice, bmpWhite);
		break;
	case 1:
		SetPictureBMP(data.memTeam[teamNo].hWnd.hMultiChoice, bmpA);
		break;
	case 2:
		SetPictureBMP(data.memTeam[teamNo].hWnd.hMultiChoice, bmpB);
		break;
	case 3:
		SetPictureBMP(data.memTeam[teamNo].hWnd.hMultiChoice, bmpC);
		break;
	case 4:
		SetPictureBMP(data.memTeam[teamNo].hWnd.hMultiChoice, bmpD);
		break;
	}
	
	return 0;
}


int clearMC()
{
	for (int i = 0; i < MAX_CLIENT; i++)
	{
		data.memTeam[i].mChoice = 0;
		SetPictureBMP(data.memTeam[i].hWnd.hMultiChoice, bmpWhite);
	}
		
	return 0;
}

int enableTeam(int teamNo, bool en)
{
	data.memTeam[teamNo].enable = en;	
	EnableWindow(data.memTeam[teamNo].hWnd.hName, en);
	EnableWindow(data.memTeam[teamNo].hWnd.hPlayer1, en);
	EnableWindow(data.memTeam[teamNo].hWnd.hPlayer2, en);
	EnableWindow(data.memTeam[teamNo].hWnd.hScore, en);
	
	if (data.memTeam[teamNo].marked && en) return 0;
	
	//EnableWindow(data.memTeam[teamNo].hWnd.hSelect, en);
	EnableWindow(data.memTeam[teamNo].hWnd.hAddPnt, en);
	EnableWindow(data.memTeam[teamNo].hWnd.hSubPnt, en);
	EnableWindow(data.memTeam[teamNo].hWnd.hMultiChoice, en);
		
	return 0;
}

int enableScoreButtons()
{
	BOOL bAnySelected = FALSE;
	for (int i=0;i<MAX_CLIENT;i++)
		if (data.memTeam[i].selected) bAnySelected = TRUE;

	
	for (int i=0;i<MAX_CLIENT;i++)
	{
		EnableWindow(data.memTeam[i].hWnd.hAddPnt, data.memTeam[i].selected);
		EnableWindow(data.memTeam[i].hWnd.hSubPnt, data.memTeam[i].selected);
	}
	EnableWindow(hSelectAddBtn, bAnySelected);
	EnableWindow(hSelectSubBtn, bAnySelected);	
	return 0;
}

int updateAddScoreBtn()
{
	CHAR subtxt[8],addtxt[8];

	sprintf_s(subtxt,"-%d ", data.memRoundPoint);
	sprintf_s(addtxt,"+%d ", data.memRoundPoint);

	for (int i=0; i<MAX_CLIENT; i++)
	{
		SetText(data.memTeam[i].hWnd.hAddPnt, addtxt);
		SetText(data.memTeam[i].hWnd.hSubPnt, subtxt);
	}
	return 0;
}

int updateTeamBtn(int teamNo)
{
	SetText(data.memTeam[teamNo].hWnd.hName, data.infoTeam[teamNo].name);
	SetText(data.memTeam[teamNo].hWnd.hPlayer1, data.infoTeam[teamNo].player1);
	SetText(data.memTeam[teamNo].hWnd.hPlayer2, data.infoTeam[teamNo].player2);
	SetText(data.memTeam[teamNo].hWnd.hScore, data.memTeam[teamNo].totalScore);
	data.memTeam[teamNo].enable = TRUE;
	enableTeam(teamNo, 1);
	return 0;
}

int findChar(char* in_string, char charTofind, int start_index, int stop_index)
{
	for (int i=start_index; i<stop_index;i++)
	{
		if (in_string[i] == charTofind) return i;
	}
	return -1; //found nothing
}

int trimSpace(char*& in_string, int size)
{
	for (int i=0; i<size;i++)
	{
		if (in_string[i] != ' ') // space = 32 or 0x20
		{
			in_string+=i;
			return i;
		}
	}
	return -1;
}

int readTeamName(char* LineBuffer)
{
	// sample : #1, La Vang, Ngoc Nguyen, Thanh Phuong
	LineBuffer++; // --> 1, La Vang, Ngoc Nguyen, Thanh Phuong
	int tno = 0;
	char tnco[5];
	char tn[20];
	char tp1[20];
	char tp2[20];

	ZeroMemory(tnco, sizeof (tno));
	ZeroMemory(tn, sizeof (tn));
	ZeroMemory(tp1, sizeof (tp1));
	ZeroMemory(tp2, sizeof (tp2));

	int k = findChar(LineBuffer, ',', 0, strlen(LineBuffer));
	if (k==-1||k>3) return 0; //fail to read 
	memcpy(tnco, LineBuffer, k); // copy to team no
	tno = atoi(tnco);
	if (tno <= 0 || tno>MAX_CLIENT) return 0;
	tno--; //convert to 0->9 instead of 1->10

	// next to team name
	LineBuffer +=k+1;
	k = trimSpace(LineBuffer, strlen(LineBuffer));
	//LineBuffer+=k;
	k = findChar(LineBuffer, ',', 0, strlen(LineBuffer));
	if (k==-1 || k>=20) return 0; //fail to read 
	memcpy(tn, LineBuffer, k); // copy to teamno

	// next to player1 name
	LineBuffer +=k+1;
	trimSpace(LineBuffer, strlen(LineBuffer));
	k = findChar(LineBuffer, ',', 0, strlen(LineBuffer));
	if (k==-1 || k>=20) return 0; //fail to read 
	memcpy(tp1, LineBuffer, k); // copy to teamno

	// next to player2 name
	LineBuffer += k+1;
	trimSpace(LineBuffer, strlen(LineBuffer));
	k = strlen(LineBuffer);
	if (k==-1 || k>=20) return 0; //fail to read 
	memcpy(tp2, LineBuffer, k); // copy to teamno

	// done reading without error, copy to memory
	ZeroMemory(data.infoTeam[tno].name, sizeof(data.infoTeam[tno].name));
	ZeroMemory(data.infoTeam[tno].player1, sizeof(data.infoTeam[tno].player1));
	ZeroMemory(data.infoTeam[tno].player2, sizeof(data.infoTeam[tno].player2));
	memcpy(data.infoTeam[tno].name, tn, strlen(tn));
	memcpy(data.infoTeam[tno].player1, tp1, strlen(tp1));
	memcpy(data.infoTeam[tno].player2, tp2, strlen(tp2));

	// update GUI
	updateTeamBtn(tno);
	
	return 1;
}

int readScoreData(char* LineBuffer)
{
	// 0, 20, ++++++++++ // round, point, reward team
	// 0, 10, +000000000 // example2
	// 0, 15, 000-000000 // 
	// 0, 10, ++++++++00 // 
	CHAR round[5], point[5]; //4 digits
	CHAR teamPtdata[16]; //10+ digit
	//RewardType tmpRT[MAX_CLIENT];
	
	//INT lastRound = 0;
	INT iRound=-1, iTeamNo=-1, iPoint =-1;

	ZeroMemory(round, sizeof (round));
	ZeroMemory(teamPtdata, sizeof (teamPtdata));
	ZeroMemory(point, sizeof (point));
	//ZeroMemory(tmpPnt, sizeof (tmpPnt));

	int k = findChar(LineBuffer, ',', 0, strlen(LineBuffer));
	if (k==-1||k>4) return 0; //fail to read or number too big
	memcpy(round, LineBuffer, k); // copy round number
	iRound = atoi(round);
	if (iRound < 0 || iRound>=MAX_ROUND) return 0; // wrong round number
	
	// next to point
	LineBuffer += k+1; // move cursor to k+1, since *k = ','
	k = trimSpace(LineBuffer, strlen(LineBuffer));
	k = findChar(LineBuffer, ',', 0, strlen(LineBuffer));
	if (k==-1) return 0; //fail to read
	memcpy(point, LineBuffer, k); // copy to 
	iPoint = atoi(point);
	if (iPoint <= 0 || iPoint > 9999) return 0; // wrong number, should be 0 to 9999
	
	// next to team point data 
	// "-++++++-00" //10 chars, each char for one team
	LineBuffer += k+1; // move cursor to k+1, since *k = ','
	trimSpace(LineBuffer, strlen(LineBuffer));
	int len = strlen(LineBuffer); 
	if (len > MAX_CLIENT+2) return 0; // fail to read the whole data
	memcpy(teamPtdata, LineBuffer, len); // copy to scored point
	
	// got all score, update score list
	data.memRoundCount++; //added a record
	
	
	// teamReward
	for (int i=0;i<MAX_CLIENT;i++)
	{
		switch (teamPtdata[i])
		{
		case '+':
			data.roundRecord[iRound].teamReward[i] = RTADD;
			break;
		case '-':
			data.roundRecord[iRound].teamReward[i] = RTSUB;
			break;
		default:
			data.roundRecord[iRound].teamReward[i] = NONE;
			break;
		}
	}

	// copy data
	data.roundRecord[iRound].roundPoint = iPoint;
	return 1;
}

int updateScore()
{
	CHAR tzscore[8];
	for (INT i=0;i<MAX_CLIENT; i++)
	{
		data.memTeam[i].totalScore = 0;
		if (!data.memTeam[i].enable) continue;
		for (int k=0;k<=MAX_ROUND;k++)
		{
			data.memTeam[i].totalScore += data.roundRecord[k].teamReward[i] * data.roundRecord[k].roundPoint;
		}
		sprintf_s(tzscore, "%d", data.memTeam[i].totalScore);
		SetText(data.memTeam[i].hWnd.hScore, tzscore);
	}
	return 0;
}

// void setTextRound(int roundNo)
// {
// 	CHAR tztext[5]={0,0,0,0,0};
// 	sprintf(tztext, "%d", roundNo);
// 	SetText(hLabelRoundNo, tztext);
// }

int loadRound()
{
	CHAR tztext[5]={0,0,0,0,0};
	sprintf_s(tztext, "%d", data.memCurrentRound);
	SetText(hLabelRoundNo, tztext);
	
	BOOL roundPlayed = FALSE;
	for (int i=0;i<MAX_CLIENT;i++)
		if (data.roundRecord[data.memCurrentRound].teamReward[i] != NONE) {roundPlayed = TRUE; break;}
	
	data.memRoundPoint = data.roundRecord[data.memCurrentRound].roundPoint;
	sprintf_s(tztext,"%d",data.memRoundPoint);
	SetText(hLabelScoreRoundPoint, tztext);

	CHAR rPtztext[8];
	sprintf_s(rPtztext, "+%d", data.memRoundPoint);
	SetText(hSelectAddBtn, rPtztext);
	sprintf_s(rPtztext, "-%d", data.memRoundPoint);
	SetText(hSelectSubBtn, rPtztext);
	updateAddScoreBtn();

	if (roundPlayed)
	{
		for (int i=0;i<MAX_CLIENT;i++)
		{
			if (data.roundRecord[data.memCurrentRound].teamReward[i] != NONE) 
				rewardScore(i, data.roundRecord[data.memCurrentRound].teamReward[i]);
			if (data.memTeam[i].enable == FALSE) enableTeam(i, data.memTeam[i].enable);
		}	
		//sprintf_s(tztext,"",data.mem);
		//SetText(hScoreRoundPointEdt, )
	}

	updateScore();
	return 0;
}

int nextRound()
{
	saveRound();
	data.memCurrentRound++;
	if (data.memCurrentRound>MAX_ROUND) data.memCurrentRound = MAX_ROUND+1;
	clearRound();
	return 0;
}

int prevRound()
{
	saveRound();
	data.memCurrentRound--;
	if (data.memCurrentRound<0) data.memCurrentRound = 0;
	clearRound();
	return 0;
}

// dump temp data to history
int saveRound()
{
	for (INT i=0;i<MAX_CLIENT; i++)
	{
		//if (!data.memTeam[i].enable) continue;
		data.roundRecord[data.memCurrentRound].teamReward[i] = data.memRoundRewardType[i];
		data.memRoundRewardType[i] = NONE;
	}
	data.roundRecord[data.memCurrentRound].roundPoint = data.memRoundPoint;
	return 0;
}

int clearRound()
{
	//int currentRound = data.currentRound;
	//ZeroMemory(&tempRoundData,sizeof (tempRoundData));

	for (INT i=0;i<MAX_CLIENT; i++)
	{
		data.memRoundRewardType[i] = NONE;
		selectTeam(i, FALSE);
		data.memTeam[i].marked = FALSE;
		//if (!data.memTeam[i].enable) continue;
		EnableWindow(data.memTeam[i].hWnd.hAddPnt, data.memTeam[i].enable);
		EnableWindow(data.memTeam[i].hWnd.hSubPnt, data.memTeam[i].enable);
		marking[i] = FALSE;
	}

 	//EnableWindow(hScoreRoundPointEdt, TRUE);

	lbClearAll(hScoreList);
	
	//setTextRound(data.memCurrentRound);
	return 0;
}

int rewardScoreResID(INT resID)
{
	int id = resID - IDC_SCORE_ADD_T1;
	return 0;
}

int rewardScore(INT teamID, RewardType rtpt)
{
	// get value first
	CHAR pttext[8];
	CHAR lbRecord[50];
	INT rPoint = 0;
	
	GetText(hLabelScoreRoundPoint, pttext);
	data.memRoundPoint = atoi(pttext);

	switch (rtpt)
	{
	case NONE:
		return 0;
	case RTADD:
		rPoint = data.memRoundPoint; //add
		break;
	case RTSUB:
		rPoint = -data.memRoundPoint; //subtract
		break;
	}

	if (rPoint >0)
		sprintf_s(lbRecord, "%s,  +%d",  data.infoTeam[teamID].name, rPoint);
	else 
		sprintf_s(lbRecord, "%s,  %d", data.infoTeam[teamID].name, rPoint);
	lbInsertItem(hScoreList, lbRecord);

	//EnableWindow(hScoreRoundPointEdt, FALSE);
	
	selectTeam(teamID, FALSE);
	data.memTeam[teamID].marked = TRUE;
	
	//SetPictureBMP(data.memTeam[teamID].hWnd.hSelect, bmpMark);
	
	//EnableWindow(data.memTeam[teamID].hWnd.hSelect, FALSE);	
	
	// assign value to 
	data.memRoundRewardType[teamID] = rtpt;	
	return 1;
}


#define FILE_SESSION "session.txt"

BOOL saveDataToSession()
{
	// delete current session file
// 	WCHAR exe_path[256], session_file[256];
// 	HMODULE hModule = GetModuleHandle(NULL);
// 	GetModuleFileNameW(hModule, exe_path, MAX_PATH * sizeof WCHAR);
// 	WCHAR *pWchar = wcsrchr((wchar_t*)&exe_path, L'\\');
// 	pWchar[0] = NULL; // cut string by cheating
// 
// 	session_file[0] = NULL;
// 	formatString(session_file, L"%s\\%s", exe_path, FILE_SESSION);
// 	DeleteFileW(session_file);

	FILE *fileSession = fopen(FILE_SESSION, "w");
	if (!fileSession) return 0; // file script write fail

	CHAR line_buffer[256 + 1];
	int line_no=0, i=0;

	// write team data
	for (int i=0;i<MAX_CLIENT; i++)
	{
		ZeroMemory(line_buffer, sizeof(line_buffer));
		sprintf_s(line_buffer, "\n#%d, %s, %s, %s ", i+1, data.infoTeam[i].name, data.infoTeam[i].player1, data.infoTeam[i].player2);
		fputs(line_buffer, fileSession);
	}
	
	for (int i=0;i<=MAX_ROUND;i++)
	{
		CHAR rttext[MAX_CLIENT+1];
		ZeroMemory(rttext, sizeof(rttext));
		ZeroMemory(line_buffer, sizeof(line_buffer));

		for (int k=0; k<MAX_CLIENT;k++)
		{
			switch (data.roundRecord[i].teamReward[k])
			{
			case RTADD:
				rttext[k] = '+';
				break;
			case RTSUB:
				rttext[k] = '-';
				break;
			default:
				rttext[k] = '0';
				break;
			}
		}
		sprintf_s(line_buffer, "\n%d, %d, %s ", i, data.roundRecord[i].roundPoint, rttext);
		fputs(line_buffer, fileSession);
	}
	
	_fcloseall();
	updateScore();
	return TRUE;
}

BOOL loadSession()
{
	FILE *filex = fopen(FILE_SESSION, "r");
	if (!filex) return 0; // file script read fail

	CHAR line_buffer[256 + 1];
		
	int line_no=0, i=0;
	while (fgets(line_buffer, 256, filex))
	{
		CHAR* line_buffer_trimmed = &line_buffer[0];
		trimSpace(line_buffer_trimmed, strlen(line_buffer));
		if (line_buffer_trimmed[0] == ';') continue; //comment line
		if (strlen(line_buffer_trimmed)<2) continue; //empty line
		
		if (line_buffer_trimmed[0] == '#') readTeamName(line_buffer_trimmed);
		else readScoreData(line_buffer_trimmed);//team name and player name

		// read a word from line
	
	}
	_fcloseall();
	
	data.memCurrentRound=0; //set round as current round, round 0 doesn't count
	updateScore();

	return true;
}

BOOL loadNewSession()
{
	int k = MessageBoxW(NULL, L"Load dữ liệu mới sẽ xóa session hiện tại", L"Thông báo", MB_OKCANCEL);
	if (k == 2) return 0;

	// delete log if there is
	WCHAR exe_path[256], session_file[256];
	HMODULE hModule = GetModuleHandle(NULL);
	GetModuleFileNameW(hModule, exe_path, MAX_PATH * sizeof WCHAR);
	WCHAR *pWchar = wcsrchr((wchar_t*)&exe_path, L'\\');
	pWchar[0] = NULL; // cut string by cheating

	session_file[0] = NULL;
	formatString(session_file, L"%s\\%s", exe_path, FILE_SESSION);
	DeleteFileW(session_file);	

	OPENFILENAME ofn;       // common dialog box structure
	TCHAR szFile[260] = { 0 };       // if using TCHAR macros

	// Initialize OPENFILENAME
	ZeroMemory(&ofn, sizeof(ofn));
	ofn.lStructSize = sizeof(ofn);
	ofn.hwndOwner = hMain;
	ofn.lpstrFile = szFile;
	ofn.nMaxFile = sizeof(szFile);
	ofn.lpstrFilter = _T("All\0*.*\0Text\0*.TXT\0");
	ofn.nFilterIndex = 1;
	ofn.lpstrFileTitle = NULL;
	ofn.nMaxFileTitle = 0;
	ofn.lpstrInitialDir = NULL;
	ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;

	if (GetOpenFileName(&ofn) != TRUE) // file available
		return 0;
		// use ofn.lpstrFile
	
	FILE *filex = _tfopen(ofn.lpstrFile, "r");
	FILE *fileSession = fopen(FILE_SESSION, "w");
	if (!filex) return 0; // file script write fail

	CHAR line_buffer[256 + 1];

	int line_no=0, i=0;
	while (fgets(line_buffer, 256, filex))
	{
		CHAR* line_buffer_trimmed = &line_buffer[0];
		trimSpace(line_buffer_trimmed, strlen(line_buffer));
		if (line_buffer_trimmed[0] == ';') continue; //comment line
		if (strlen(line_buffer_trimmed)<2) continue; //empty line

		if (line_buffer_trimmed[0] == '#') 
			readTeamName(line_buffer_trimmed);
		
		else 
			readScoreData(line_buffer_trimmed);
	}
	_fcloseall();

	data.memCurrentRound=1; // change round to round 1
	saveDataToSession();
	updateScore();

	return TRUE;
}

int createScoreTable(MonRect wndRect)
{
	HWND habd = CreateDialog(hInst, MAKEINTRESOURCE(IDD_TABLE_SCORE), NULL,(DLGPROC)ScoreTableProc);

	MoveWindow(habd,wndRect.left,wndRect.top,wndRect.width,wndRect.height, TRUE);
	ShowWindow(habd, SW_HIDE);
	//UpdateWindow(habd);

	return 0;
}