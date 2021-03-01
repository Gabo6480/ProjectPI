#define no_init_all deprecated

#include <Windows.h>
#include <thread>
#include "DeviceEnumerator.h"
#include "opencv2/opencv.hpp"
#include "resource.h"

HINSTANCE hInst;
int appShow;
HWND mainWindow;

HWND hPicDisplay;

cv::VideoCapture videoCapture;

//Window procedure declaration
LRESULT CALLBACK MainProc(HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam);


bool stopVideoCapture = false;
void videoCaptureThread() {
	stopVideoCapture = false;
	while (!stopVideoCapture)
	{
		cv::Mat frame;
		if (videoCapture.read(frame)) {

			cv::cvtColor(frame, frame, cv::COLOR_BGR2BGRA);

			HBITMAP hB = CreateBitmap(frame.cols, frame.rows, 1, 32, frame.data);
			SendMessage(hPicDisplay, STM_SETIMAGE, IMAGE_BITMAP, (LPARAM)hB);
			DeleteObject(hB);
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

	MSG msg;
	ZeroMemory(&msg, sizeof(MSG));

	//Gets the available video devices
	DeviceEnumerator de;
	std::map<int, Device> devices = de.getVideoDevicesMap();

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


LRESULT CALLBACK MainProc(HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam) {
	
	switch (msg)
	{
	case WM_INITDIALOG:
	{
		hPicDisplay = GetDlgItem(hDlg, IDC_PICDISPLAY);

		videoCapture.open(0);
	}
	break;
	case WM_DESTROY:
	case WM_CLOSE:
		PostQuitMessage(0);
		break;
	default:
		break;
	}
	
	return false;
}