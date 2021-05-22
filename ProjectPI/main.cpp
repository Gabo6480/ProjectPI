#define no_init_all deprecated

#include <Windows.h>
#include <Commctrl.h>
#include <string.h>
#include <thread>
#include <cmath>
#include <vector>
#include "DeviceEnumerator.h"
#include "opencv2/opencv.hpp"
#include "resource.h"


#include "FilterAverage.h"
#include "FilterAverageSubstraction.h"
#include "FilterDirectionalEast.h"
#include "FilterDirectionalNorth.h"
#include "FilterGaussian.h"
#include "FilterLaplacian.h"
#include "FilterMedian.h"
#include "FilterMinusLaplacian.h"
#include "FilterSobel.h"
#include "FilterWeightedAverage.h"
#include "FilterGrayscale.h"
#include "FilterThreshold.h"
#include "FilterSepia.h"
#include "FilterSimpleEqualization.h"
#include "FilterUniformEqualization.h"
#include "FilterExponentialEqualization.h"
#include "FilterPowerEqualization.h"
#include "FilterNegative.h"
#include "FilterVerticalMirror.h"
#include "FilterFourths.h"

HINSTANCE hInst;
int appShow;
HWND mainWindow;
HWND camSelectorWindow;
HWND setValueWindow;
HWND setTwoValuesWindow;

HWND hPicDisplay;


//Video Controls
HWND hPlayBtn;
HWND hRecordBtn;
HWND hCancelBtn;
HWND hProgressSlider;
HWND hTimeText;
HWND hFPSText;

HWND hAddBtn;
HWND hRemoveBtn;
HWND hFiltersList;
HWND hActiveList;

//camSelectorWindow
HWND hSourcesCB;

//setValueWindow
HWND hValueEdit;

//setTwoValuesWindow
HWND hValueOneEdit;
HWND hValueTwoEdit;

cv::VideoCapture videoCapture;
cv::Mat currFrame;
cv::Mat prosFrame;
cv::Mat auxFrame;
int cvFPS = 60;
float videoLength = 0;
std::string videoLengthString = "";

bool newFrameReady = false;
bool pauseVideoCapture = true;
bool isRecording = false;
bool stopVideoCapture = false;

std::vector<IFilter*> appliedFilters;
ISetValue* filterToEditValue = 0;
ISetTwoValues* filterToEditTwoValues = 0;

std::vector<cv::Mat> storedFrames;

std::string filterNames[] = {"Average", "Weighted Average", "Median", "Gaussian", "Sobel", 
							"Average Substaction", "Laplacian", "Minus Laplacian", "Directional North",
							"Directional East", "Grayscale", "Threshold", "Sepia", "Simple Equalization", 
							"Uniform Equalization", "Exponential Equalization", "Power Equalization",
							"Negative", "Vertical Mirror", "Fourths"};

//Window procedure declaration
LRESULT CALLBACK MainProc(HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam);
LRESULT CALLBACK CamSelectProc(HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam);
LRESULT CALLBACK SetValueProc(HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam);
LRESULT CALLBACK SetTwoValuesProc(HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam);

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

	char szFile[MAX_PATH] = { NULL };  // buffer for file name
	OPENFILENAME ofn;       // common dialog box structure
	HANDLE hf = 0;              // file handle

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
void replaceProssesedFrame() {

	if (currFrame.empty())
		return;

	frameCount++;
	cv::Mat aux;
	currFrame.copyTo(aux);

	//Apply all filters
	for (auto& filter : appliedFilters) {
		filter->Filter(aux, aux);
	}

	aux.copyTo(prosFrame);
	newFrameReady = false;

	cv::resize(aux, aux, cv::Size(668, 422), 0, 0, cv::INTER_AREA);

	//Convert color format so it is compatible with bitmaps
	cv::cvtColor(aux, aux, cv::COLOR_BGR2BGRA);

	HBITMAP hB = CreateBitmap(aux.cols, aux.rows, 1, 32, aux.data);
	SendMessage(hPicDisplay, STM_SETIMAGE, IMAGE_BITMAP, (LPARAM)hB);
	DeleteObject(hB);
}

void storeFrame() {
	storedFrames.push_back(currFrame.clone());
}

//Clears the video capturer
void videoCaptureRelease() {
	pauseVideoCapture = true;
	if (isRecording)
		storedFrames.clear();
	isRecording = false;
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
			std::this_thread::sleep_for(1ns);
		}
		else
		{
			if (videoCapture.isOpened())
				if (videoCapture.read(auxFrame)) {
					currFrame = auxFrame;
					if (isRecording)
						storeFrame();

					newFrameReady = true;
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

void videoShowThread()
{
	while (!stopVideoCapture) {
		using namespace std::chrono_literals;
		if (!newFrameReady) {
			std::this_thread::sleep_for(1ns);
		}
		else
			replaceProssesedFrame();
	}
}


void showEditValueWindow(IFilter* filter) {

	filterToEditValue = dynamic_cast<ISetValue*>(filter);
	if(filterToEditValue != NULL)
		ShowWindow(setValueWindow, SW_SHOW);
	else {
		filterToEditTwoValues = dynamic_cast<ISetTwoValues*>(filter);
		if (filterToEditTwoValues != NULL) {
			ShowWindow(setTwoValuesWindow, SW_SHOW);
		}
	}
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR cmdLine, int showCmd) {

	hInst = hInstance;
	appShow = showCmd;

	//Create and show main window
	mainWindow = CreateDialog(hInstance, MAKEINTRESOURCE(IDD_MAINDIALOG), 0, MainProc);
	ShowWindow(mainWindow, showCmd);

	camSelectorWindow = CreateDialog(hInstance, MAKEINTRESOURCE(IDD_CAMSOURCE), mainWindow, CamSelectProc);

	setValueWindow = CreateDialog(hInstance, MAKEINTRESOURCE(IDD_SETVALUE), mainWindow, SetValueProc);

	setTwoValuesWindow = CreateDialog(hInstance, MAKEINTRESOURCE(IDD_SETTWOVALUES), mainWindow, SetTwoValuesProc);

	MSG msg;
	ZeroMemory(&msg, sizeof(MSG));

	//Amount of threads supported by the implementation
	int x = std::thread::hardware_concurrency();

	std::thread worker(videoCaptureThread);
	worker.detach();
	
	std::thread worker2(videoShowThread);
	worker2.detach();

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

	if(worker2.joinable())
		worker2.join();

	for (auto& filter : appliedFilters) {
		delete filter;
	}
	appliedFilters.clear();

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
		cvFPS = 48;
		SetWindowText(hTimeText, "0:00 / 0:00");
	}
}

void PauseVideoCapture(bool pause) {
	pauseVideoCapture = pause;
	SetWindowText(hPlayBtn, pauseVideoCapture ? "Play" : "Pause");
}

void CALLBACK FPSTimerProc(HWND hDlg, UINT msg, UINT_PTR timer, DWORD ticks)
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
		hRecordBtn = GetDlgItem(hDlg, IDC_BTN_RECORD);
		hCancelBtn = GetDlgItem(hDlg, IDC_BTN_CANCEL);
		hProgressSlider = GetDlgItem(hDlg, IDC_SLIDER_PROGRESS);
		hTimeText = GetDlgItem(hDlg, IDC_TEXT_TIMER);
		hFPSText = GetDlgItem(hDlg, IDC_TEXT_FPS);
		
		hAddBtn = GetDlgItem(hDlg, IDC_BTN_ADD);
		hRemoveBtn = GetDlgItem(hDlg, IDC_BTN_REMOVE);
		hFiltersList = GetDlgItem(hDlg, IDC_LIST_FILTERS);
		hActiveList = GetDlgItem(hDlg, IDC_LIST_ACTIVE_FILTERS);

		for (int i = 0; i < ARRAYSIZE(filterNames); i++) {
			int pos = (int)SendMessage(hFiltersList, LB_ADDSTRING, 0,
				(LPARAM)filterNames[i].c_str());

			SendMessage(hFiltersList, LB_SETITEMDATA, pos, (LPARAM)i);
		}


		EnableWindow(hRecordBtn, false);
		EnableWindow(hCancelBtn, false);
		EnableVideoControls(false);
		SetTimer(hDlg, 6480, 1000, FPSTimerProc);
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
		case ID_GUARDARCOMO_IMAGEN:
		{
			SetWindowText(hRecordBtn, (LPCSTR)"Capture");
			EnableWindow(hRecordBtn, true);
			EnableWindow(hCancelBtn, false);
		}
		break;
		case ID_GUARDARCOMO_VIDEO:
		{
			if (videoCapture.isOpened()) {
				SetWindowText(hRecordBtn, (LPCSTR)"Record");
				EnableWindow(hRecordBtn, true);
			}
			else
				MessageBox(hDlg, "Only video sources can be stored as video.", "Error", MB_ICONEXCLAMATION);
		}
		break;
		case IDC_BTN_RECORD:
		{
			char buff[MAX_PATH] = "";
			char btnTxt[12] = "";
			GetWindowText(hRecordBtn, btnTxt, 12);
			if (strcmp(btnTxt, "Capture") == 0) {			//Save as Image
				cv::Mat image = prosFrame;
				if (GetFile(hDlg, 1, buff)) {
					strcat_s(buff, ".jpg");
					cv::imwrite(buff, image);
				}
			}
			else if (strcmp(btnTxt, "Record") == 0) {		//Start Recording
				SetWindowText(hRecordBtn, (LPCSTR)"Recording");
				EnableWindow(hCancelBtn, true);
				EnableWindow(hAddBtn, false);
				EnableWindow(hRemoveBtn, false);
				isRecording = true;
			}
			else if (strcmp(btnTxt, "Recording") == 0) {	//Stop recording 
				SetWindowText(hRecordBtn, (LPCSTR)"Save Video");
				isRecording = false;
			}
			else if (strcmp(btnTxt, "Save Video") == 0) {	//Save as video
				if (GetFile(hDlg, 1, buff)) {
					strcat_s(buff, ".avi");

					cv::Size S = cv::Size((int)videoCapture.get(cv::CAP_PROP_FRAME_WIDTH),    //Acquire input size
						(int)videoCapture.get(cv::CAP_PROP_FRAME_HEIGHT));
					cv::VideoWriter videoWriter(buff, cv::VideoWriter::fourcc('M', 'J', 'P', 'G'),
						30, S, true);

					for (auto& frame : storedFrames) {

						for (auto& filter : appliedFilters) {
							filter->Filter(frame, frame);
						}
						cv::resize(frame, frame, S, 0, 0, cv::INTER_AREA);
						videoWriter.write(frame);
					}

					videoWriter.release();

					storedFrames.clear();
					SetWindowText(hRecordBtn, (LPCSTR)"Record");
					EnableWindow(hRecordBtn, false);
					EnableWindow(hCancelBtn, false);
					EnableWindow(hAddBtn, true);
					EnableWindow(hRemoveBtn, true);
					isRecording = false;
					MessageBox(hDlg, buff, "Complete", MB_ICONINFORMATION);
				}
			}
		}
		break;
		case IDC_BTN_CANCEL:
		{
			storedFrames.clear();
			SetWindowText(hRecordBtn, (LPCSTR)"Record");
			EnableWindow(hRecordBtn, false);
			EnableWindow(hCancelBtn, false);
			EnableWindow(hAddBtn, true);
			EnableWindow(hRemoveBtn, true);
			isRecording = false;
		}
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
				auxFrame = cv::imread(buff);
				currFrame = auxFrame;
				replaceProssesedFrame();
				auxFrame.release();
			}
		}
			break;
		case IDC_BTN_ADD:
		{
			std::string filterName = "";
			IFilter* newFilter = 0;

			int selected = (int)SendMessage(hFiltersList, LB_GETCURSEL, 0, 0);

			switch (selected)
			{
			case 0:		//Average
			{
				filterName = "Average";
				newFilter = new FilterAverage();
			}
				break;
			case 1:		//Weighted Average
			{
				filterName = "Weighted Average";
				newFilter = new FilterWeightedAverage(3.f);
			}
				break;
			case 2:		//Median
			{
				filterName = "Median";
				newFilter = new FilterMedian(3);
			}
				break;
			case 3:		//Gaussian
			{
				filterName = "Gaussian";
				newFilter = new FilterGaussian(0);
			}
				break;
			case 4:		//Sobel
			{
				filterName = "Sobel";
				newFilter = new FilterSobel();
			}
				break;
			case 5:		//Average Substaction
			{
				filterName = "Average Substaction";
				newFilter = new FilterAverageSubstraction();
			}
				break;
			case 6:		//Laplacian
			{
				filterName = "Laplacian";
				newFilter = new FilterLaplacian();
			}
				break;
			case 7:		//Minus Laplacian
			{
				filterName = "Minus Laplacian";
				newFilter = new FilterMinusLaplacian();
			}
				break;
			case 8:		//Directional North
			{
				filterName = "Directional North";
				newFilter = new FilterDirectionalNorth();
			}
				break;
			case 9:	//Directional East
			{
				filterName = "Directional East";
				newFilter = new FilterDirectionalEast();
			}
				break;
			case 10:	//Grayscale
			{
				filterName = "Grayscale";
				newFilter = new FilterGrayscale();
			}
			break;
			case 11:	//Threshold
			{
				filterName = "Threshold";
				newFilter = new FilterThreshold(100, 255);
			}
			break;
			case 12:	//Sepia
			{
				filterName = "Sepia";
				newFilter = new FilterSepia();
			}
			break;
			case 13:	//SimpleEqualization
			{
				filterName = "Simple Equalization";
				newFilter = new FilterSimpleEqualization();
			}
			break;
			case 14:	//Uniform Equalization
			{
				filterName = "Uniform Equalization";
				newFilter = new FilterUniformEqualization();
			}
			break;
			case 15:	//Exponential Equalization
			{
				filterName = "Exponential Equalization";
				newFilter = new FilterExponentialEqualization(0.1f);
			}
			break;
			case 16:	//Power Equalization
			{
				filterName = "Power Equalization";
				newFilter = new FilterPowerEqualization(1.f, 1.2f);
			}
			break;
			case 17:	//Negative
			{
				filterName = "Negative";
				newFilter = new FilterNegative();
			}
			break;
			case 18:	//Vertical Mirror
			{
				filterName = "Vertical Mirror";
				newFilter = new FilterVerticalMirror();
			}
			break;
			case 19:	//Fourths
			{
				filterName = "Fourths";
				newFilter = new FilterFourths();
			}
			break;
			default:
				break;
			}

			appliedFilters.push_back(newFilter);

			(int)SendMessage(hActiveList, LB_ADDSTRING, 0,
				(LPARAM)filterName.c_str());

			if(!videoCapture.isOpened())
				replaceProssesedFrame();

			showEditValueWindow(newFilter);
		}
			break;
		case IDC_BTN_REMOVE:
		{
			int selected = (int)SendMessage(hActiveList, LB_GETCURSEL, 0, 0);

			if (selected > -1) {
				delete appliedFilters.at(selected);
				appliedFilters.erase(appliedFilters.begin() + selected);

				SendMessage(hActiveList, LB_DELETESTRING, selected, 0);

				if (!videoCapture.isOpened())
					replaceProssesedFrame();
			}
		}
			break;
		case IDC_BTN_EDIT:
			{
			int selected = (int)SendMessage(hActiveList, LB_GETCURSEL, 0, 0);

			if (selected > -1) {
				showEditValueWindow(appliedFilters.at(selected));
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
			if (videoCapture.read(auxFrame)) {
				currFrame = auxFrame;
				replaceProssesedFrame();
			}
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
			SetWindowText(hSourcesCB, (LPCSTR)"No video sources were found.");
		}
		else
		{
			SetWindowText(hSourcesCB, (LPCSTR)"Select a video source.");
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
					MessageBox(hDlg, "The selected video source couldn't be opened.", "Error", MB_ICONEXCLAMATION);
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


char prevValue[20] = { NULL };
float originalValue = 0;
LRESULT CALLBACK SetValueProc(HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam) {
	switch (msg)
	{
	case WM_INITDIALOG:
	{
		hValueEdit = GetDlgItem(hDlg, IDC_EDITVALUE);
	}
		break;
	case WM_SHOWWINDOW:
	{
		std::string text = "";
		if (wParam == TRUE && filterToEditValue != 0) {
			originalValue = filterToEditValue->getValue();

			text = std::to_string(originalValue);
		}

		SetWindowText(hValueEdit, text.c_str());
	}
		break;
	case WM_COMMAND:
	{
		int comm = LOWORD(wParam);
		switch (comm)
		{
		case IDC_EDITVALUE:
		{
			int hword = HIWORD(wParam);
			switch (hword)
			{
			case EN_UPDATE:
			{
				char buff[20] = { NULL };
				GetWindowText(hValueEdit, buff, 20);

				if (buff[0] == NULL)
					break;

				bool hasPoint = false;
				bool isNumber = true;
				for (int i = 0; i < 20; i++) {
					if (buff[i] == NULL)
						break;

					if (buff[i] == '.') {
						if (!hasPoint)
							hasPoint = true;
						else {
							isNumber = false;
							break;
						}
					}

					if (!(std::isdigit(buff[i]) || buff[i] == '.')) {
						isNumber = false;
						break;
					}
				}

				if (!isNumber) {
					SetWindowText(hValueEdit, prevValue);
				}
				else {
					strcpy_s(prevValue, buff);

					float val = std::atof(buff);
					filterToEditValue->setValue(val);

					if (!videoCapture.isOpened())
						replaceProssesedFrame();
				}
			}
			break;
			default:
				break;
			}
		}
		break;
		case IDCANCEL:
			if (filterToEditValue != 0) {
				filterToEditValue->setValue(originalValue);

				if (!videoCapture.isOpened())
					replaceProssesedFrame();
			}
		case IDOK:
		{
			ShowWindow(hDlg, SW_HIDE);
			memset(prevValue, 0, 20);
			filterToEditValue = NULL;
		}
		break;
		default:
			break;
		}
	}
		break;
	case WM_DESTROY:
	case WM_CLOSE:
		ShowWindow(hDlg, SW_HIDE); 
		memset(prevValue, 0, 20);
		filterToEditValue = NULL;
		break;
	default:
		break;
	}

	return false;
}

char prevValue2[20] = { NULL };
float originalValue2 = 0;
LRESULT CALLBACK SetTwoValuesProc(HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam) {
	switch (msg)
	{
	case WM_INITDIALOG:
	{
		hValueOneEdit = GetDlgItem(hDlg, IDC_EDITVALUEONE);
		hValueTwoEdit = GetDlgItem(hDlg, IDC_EDITVALUETWO);
	}
		break;
	case WM_SHOWWINDOW:
	{
		std::string text1 = "";
		std::string text2 = "";
		if (wParam == TRUE && filterToEditTwoValues != 0) {
			originalValue = filterToEditTwoValues->getTwoValues(true);
			originalValue2 = filterToEditTwoValues->getTwoValues(false);

			text1 = std::to_string(originalValue);
			text2 = std::to_string(originalValue2);
		}

		SetWindowText(hValueOneEdit, text1.c_str());
		SetWindowText(hValueTwoEdit, text2.c_str());
	}
		break;
	case WM_COMMAND:
	{
		int comm = LOWORD(wParam);
		switch (comm)
		{
		case IDC_EDITVALUEONE:
		{
			int hword = HIWORD(wParam);
			switch (hword)
			{
			case EN_UPDATE:
			{
				char buff[20] = { NULL };
				GetWindowText(hValueOneEdit, buff, 20);

				if (buff[0] == NULL)
					break;

				bool hasPoint = false;
				bool isNumber = true;
				for (int i = 0; i < 20; i++) {
					if (buff[i] == NULL)
						break;

					if (buff[i] == '.') {
						if (!hasPoint)
							hasPoint = true;
						else {
							isNumber = false;
							break;
						}
					}

					if (!(std::isdigit(buff[i]) || buff[i] == '.')) {
						isNumber = false;
						break;
					}
				}

				if (!isNumber) {
					SetWindowText(hValueOneEdit, prevValue);
				}
				else {
					strcpy_s(prevValue, buff);

					float val = std::atof(buff);
					filterToEditTwoValues->setTwoValues(val, true);

					if (!videoCapture.isOpened())
						replaceProssesedFrame();
				}
			}
			break;
			default:
				break;
			}
		}
		break;
		case IDC_EDITVALUETWO:
		{
			int hword = HIWORD(wParam);
			switch (hword)
			{
			case EN_UPDATE:
			{
				char buff[20] = { NULL };
				GetWindowText(hValueTwoEdit, buff, 20);

				if (buff[0] == NULL)
					break;

				bool hasPoint = false;
				bool isNumber = true;
				for (int i = 0; i < 20; i++) {
					if (buff[i] == NULL)
						break;

					if (buff[i] == '.') {
						if (!hasPoint)
							hasPoint = true;
						else {
							isNumber = false;
							break;
						}
					}

					if (!(std::isdigit(buff[i]) || buff[i] == '.')) {
						isNumber = false;
						break;
					}
				}

				if (!isNumber) {
					SetWindowText(hValueTwoEdit, prevValue2);
				}
				else {
					strcpy_s(prevValue2, buff);

					float val = std::atof(buff);
					filterToEditTwoValues->setTwoValues(val, false);

					if (!videoCapture.isOpened())
						replaceProssesedFrame();
				}
			}
			break;
			default:
				break;
			}
		}
		break;
		case IDCANCEL:
			if (filterToEditTwoValues != 0) {
				filterToEditTwoValues->setTwoValues(originalValue, true);
				filterToEditTwoValues->setTwoValues(originalValue2, false);

				if (!videoCapture.isOpened())
					replaceProssesedFrame();
			}
		case IDOK:
		{
			ShowWindow(hDlg, SW_HIDE);
			memset(prevValue, 0, 20);
			memset(prevValue2, 0, 20);
			filterToEditTwoValues = NULL;
		}
		break;
		default:
			break;
		}
	}
		break;
	case WM_DESTROY:
	case WM_CLOSE:
		ShowWindow(hDlg, SW_HIDE); 
		memset(prevValue, 0, 20);
		memset(prevValue2, 0, 20);
		filterToEditTwoValues = NULL;
		break;
	default:
		break;
	}

	return false;
}
