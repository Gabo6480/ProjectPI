#define no_init_all deprecated

#include <Windows.h>
#include <Commctrl.h>
#include <string.h>
#include <thread>
#include <cmath>
#include "DeviceEnumerator.h"
#include "opencv2/opencv.hpp"
#include "resource.h"

HINSTANCE hInst;
int appShow;
HWND mainWindow;
HWND camSelectorWindow;

HWND hPicDisplay;


//Video Controls
HWND hPlayBtn;
HWND hProgressSlider;
HWND hTimeText;
HWND hFPSText;

HWND hSourcesCB;

cv::VideoCapture videoCapture;
cv::Mat currFrame;
int cvFPS = 60;
float videoLength = 0;
std::string videoLengthString = "";

bool pauseVideoCapture = true;
bool stopVideoCapture = false;

//Window procedure declaration
LRESULT CALLBACK MainProc(HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam);
LRESULT CALLBACK CamSelectProc(HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam);

void EnableVideoControls(bool enable, int frameCount = 0);

bool GetFile(HWND hwndOwner, int type, char* buff)
{
	LPCSTR filtro = 0;
	switch (type)
	{
	case 0:
		filtro = "Imagenes (.jpg,.bmp)\0*.jpg;*.bmp\0Imagenes JPG (.jpg)\0*.jpg\0Imagenes BMP (.bmp)\0*.bmp\0";
		break;
	case 1:
		filtro = "Todos\0*.*\0Videos Mp4\0*.mp4\0";
		break;
	case 2:
		filtro = "Todos\0*.*\0Videos Avi\0*.avi\0";
	default:
		break;
	}

	char szFile[MAX_PATH];  // buffer for file name
	OPENFILENAME ofn;       // common dialog box structure
	HANDLE hf;              // file handle

							// Initialize OPENFILENAME
	ZeroMemory(&ofn, sizeof(ofn));
	ofn.lStructSize = sizeof(ofn);
	ofn.hwndOwner = hwndOwner;
	ofn.lpstrFile = szFile;
	// Set lpstrFile[0] to '\0' so that GetOpenFileName does not 
	// use the contents of szFile to initialize itself.
	ofn.lpstrFile[0] = '\0';
	ofn.nMaxFile = MAX_PATH;
	ofn.lpstrFilter = filtro;
	ofn.nFilterIndex = 1;
	ofn.lpstrFileTitle = NULL;
	ofn.nMaxFileTitle = 0;
	ofn.lpstrInitialDir = NULL;
	if (type == 0) {
		ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;
	}

	// Display the Open dialog box.
	if (GetOpenFileName(&ofn) == TRUE) {
		strcpy_s(buff, MAX_PATH, ofn.lpstrFile);
		return true;
	}
	else {
		return false;
	}

}
std::string SecToMinAndSecString(float sec) {
	int min = sec / 60;
	int lSec = (int)sec % 60;
	std::string ss = (lSec < 10 ? "0": "") + std::to_string(lSec);
	return std::to_string(min) + ":" + ss;
}


int frameCount = 0;
void replaceCurrentFrame(cv::Mat frame) {

	cv::resize(frame, frame, cv::Size(668, 422), 0, 0, cv::INTER_AREA);

	cv::cvtColor(frame, frame, cv::COLOR_BGR2BGRA);

	HBITMAP hB = CreateBitmap(frame.cols, frame.rows, 1, 32, frame.data);
	SendMessage(hPicDisplay, STM_SETIMAGE, IMAGE_BITMAP, (LPARAM)hB);
	DeleteObject(hB);

	currFrame = frame;
	frameCount++;
}

void videoCaptureRelease() {
	pauseVideoCapture = true;
	EnableVideoControls(false);
	if (videoCapture.isOpened()) {
		videoCapture.release();
	}
}

void videoCaptureThread() {
	stopVideoCapture = false;
	while (!stopVideoCapture)
	{
		using namespace std::chrono_literals;
		if (pauseVideoCapture) {
			std::this_thread::sleep_for(1ms);
		}
		else
		{
			cv::Mat frame;
			if (videoCapture.isOpened())
				if (videoCapture.read(frame)) {
					replaceCurrentFrame(frame);

					if (videoLength > 0) {
						float currFrameNum = abs(videoCapture.get(cv::CAP_PROP_POS_FRAMES));
						SendMessage(hProgressSlider, TBM_SETPOS, true, currFrameNum);
						SetWindowText(hTimeText, (SecToMinAndSecString(currFrameNum / cvFPS) + " / " + videoLengthString).c_str());
					}
				}
		}
	}
	return;
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR cmdLine, int showCmd) {

	hInst = hInstance;
	appShow = showCmd;

	//Create and show main window
	mainWindow = CreateDialog(hInstance, MAKEINTRESOURCE(IDD_MAINDIALOG), 0, MainProc);
	ShowWindow(mainWindow, showCmd);

	camSelectorWindow = CreateDialog(hInstance, MAKEINTRESOURCE(IDD_CAMSOURCE), mainWindow, CamSelectProc);

	MSG msg;
	ZeroMemory(&msg, sizeof(MSG));

	//Amount of threads supported by the implementation
	int x = std::thread::hardware_concurrency();

	std::thread worker(videoCaptureThread);
	worker.detach();

	while (GetMessage(&msg, nullptr, 0, 0))
	{
		if (mainWindow == 0 || !IsDialogMessage(mainWindow, &msg))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}

	stopVideoCapture = true;
	if(worker.joinable())
		worker.join();
	return (int)msg.wParam;
}


void EnableVideoControls(bool enable, int frameCount) {
	EnableWindow(hPlayBtn, enable);

	SendMessage(hProgressSlider, TBM_SETRANGE, true, MAKELPARAM(0, frameCount));
	EnableWindow(hProgressSlider, enable);
	
	EnableWindow(hTimeText, enable);

	if (!enable) {
		videoLength = 0;
		videoLengthString = "";
		cvFPS = 60;
		SetWindowText(hTimeText, "0:00 / 0:00");
	}
}

void PauseVideoCapture(bool pause) {
	pauseVideoCapture = pause;
	SetWindowText(hPlayBtn, pauseVideoCapture ? "Play" : "Pause");
}

void CALLBACK Timerproc(HWND hDlg, UINT msg, UINT_PTR timer, DWORD ticks)
{
	std::string fps = "FPS: " + std::to_string(frameCount);
	SetWindowText(hFPSText, fps.c_str());
	frameCount = 0;
}

LRESULT CALLBACK MainProc(HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam) {
	
	switch (msg)
	{
	case WM_INITDIALOG:
	{
		hPicDisplay = GetDlgItem(hDlg, IDC_PICDISPLAY);
		hPlayBtn = GetDlgItem(hDlg, IDC_BTN_PLAY);
		hProgressSlider = GetDlgItem(hDlg, IDC_SLIDER_PROGRESS);
		hTimeText = GetDlgItem(hDlg, IDC_TEXT_TIMER);
		hFPSText = GetDlgItem(hDlg, IDC_TEXT_FPS);

		EnableVideoControls(false);
		SetTimer(hDlg, 6480, 1000, Timerproc);
	}
	break;
	case WM_COMMAND:
	{
		int comm = LOWORD(wParam);
		switch (comm)
		{
		case IDC_BTN_PLAY:
			PauseVideoCapture(!pauseVideoCapture);
			break;
		case ID_FUENTE_WEBCAM:
			ShowWindow(camSelectorWindow, SW_SHOW);
			break;
		case ID_FUENTE_VIDEO:
		{
			char buff[MAX_PATH] = "";
			if (GetFile(hDlg, 1, buff)) {
				videoCaptureRelease();
				videoCapture.open(buff);
				cvFPS = videoCapture.get(cv::CAP_PROP_FPS);
				int frameCount = videoCapture.get(cv::CAP_PROP_FRAME_COUNT);
				videoLength = (float)frameCount / cvFPS;
				videoLengthString = SecToMinAndSecString(videoLength);
				EnableVideoControls(true, frameCount);
				PauseVideoCapture(false);
			}
		}
			break;
		case ID_FUENTE_IMAGEN:
		{
			char buff[MAX_PATH] = "";
			if (GetFile(hDlg, 0, buff)) {
				videoCaptureRelease();
				cv::Mat frame = cv::imread(buff);
				replaceCurrentFrame(frame);
			}
		}
			break;
		default:
			break;
		}
	}
		break;
	case WM_HSCROLL:
	{
		if (LOWORD(wParam) == TB_THUMBTRACK && pauseVideoCapture) {
			int tick = SendMessage(hProgressSlider, TBM_GETPOS, 0, 0);
			videoCapture.set(cv::CAP_PROP_POS_FRAMES, tick);
			cv::Mat frame;
			if(videoCapture.read(frame))
				replaceCurrentFrame(frame);
		}
		else
		{
			PauseVideoCapture(true);
		}
	}
		break;
	case WM_DESTROY:
	case WM_CLOSE:
		KillTimer(hDlg, 6480);
		videoCaptureRelease();
		PostQuitMessage(0);
		break;
	default:
		break;
	}
	
	return false;
}

LRESULT CALLBACK CamSelectProc(HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam) {
	switch (msg)
	{
	case WM_INITDIALOG:
	{
		hSourcesCB = GetDlgItem(hDlg, IDC_CB_SOURCES);
		HWND hOk = GetDlgItem(hDlg, IDOK);

		//Gets the available video devices
		DeviceEnumerator de;
		std::map<int, Device> devices = de.getVideoDevicesMap();

		if (devices.size() <= 0) {
			EnableWindow(hSourcesCB, false);
			EnableWindow(hOk, false);
			SetWindowText(hSourcesCB, (LPCSTR)"No se encontraron fuentes de video");
		}
		else
		{
			SetWindowText(hSourcesCB, (LPCSTR)"Seleccione una fuente de video");
		}

		for (auto const& device : devices) {
			SendMessage(hSourcesCB, CB_ADDSTRING, 0, (LPARAM)device.second.deviceName.c_str());
		}
	}
		break;
	case WM_COMMAND:
	{
		int comm = LOWORD(wParam);
		switch (comm)
		{
		case IDOK:
		{
			int idx = SendMessage(hSourcesCB, CB_GETCURSEL, 0, 0);
			if (idx > -1) {
				videoCaptureRelease();
				if (videoCapture.open(idx)) {
					pauseVideoCapture = false;
					ShowWindow(hDlg, SW_HIDE);
				}
				else
					MessageBox(hDlg, "No se ha podido abrir la fuente de video seleccionada", "Error", MB_ICONEXCLAMATION);
			}
		}
			break;
		case IDCANCEL:
			ShowWindow(hDlg, SW_HIDE);
			break;
		default:
			break;
		}
	}
		break;
	case WM_DESTROY:
	case WM_CLOSE:
		ShowWindow(hDlg, SW_HIDE);
		break;
	default:
		break;
	}

	return false;
}
