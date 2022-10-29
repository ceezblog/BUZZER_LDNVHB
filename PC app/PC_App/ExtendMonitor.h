// ExtendMonitor.h - Ceez 2019
// Get Extend monitor info, info of each monitor is store in struct cMonitorInfo
// Warning, read up to 10 monitors, more than that will shoot out error
//
// How to use:
//
//		cMonitorInfo cMonInfo;
//		ReadMonitorInfo(&cMonInfo);
//

#define MAX_MONITORS 10
struct MonRect 
{
	INT left;
	INT top;
	INT width;
	INT height;	
};

struct cMonitorInfo
{
	INT monitor_count;
	MonRect monRC[MAX_MONITORS];
};


//cMonitorInfo *cMonInfo;

// callback function called by EnumDisplayMonitors for each enabled monitor
BOOL CALLBACK EnumDispProc(HMONITOR hMon, HDC dcMon, RECT* pRcMon, LPARAM lParam)
{
	cMonitorInfo* pmi = (cMonitorInfo*)lParam;
	if (pmi->monitor_count>= MAX_MONITORS) return FALSE;

	pmi->monRC[pmi->monitor_count].height = pRcMon->bottom-pRcMon->top;
	pmi->monRC[pmi->monitor_count].width = pRcMon->right - pRcMon->left;
	pmi->monRC[pmi->monitor_count].left = pRcMon->left;
	pmi->monRC[pmi->monitor_count].top = pRcMon->top;
	pmi->monitor_count++;
	return TRUE;
}

void ReadMonitorInfo(cMonitorInfo &cmi)
{
	//cmi;
	ZeroMemory( &cmi, sizeof (cMonitorInfo));
	EnumDisplayMonitors(0, 0, EnumDispProc, (LPARAM)&cmi);
}
