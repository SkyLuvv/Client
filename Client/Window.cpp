#include "Window.h"
#include <fstream>

//have to initialize static member variables this way
std::string Window::droppedfile;
bool Window::filedropped = false;;

LRESULT CALLBACK Window::WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg)
	{
	case WM_CREATE:
	{
		DragAcceptFiles(hWnd, TRUE);
		break;
	}
	case WM_CLOSE:
	{
		PostQuitMessage(69);
		break;
	}
	case WM_DROPFILES:
	{
		std::string filename(1000, '\0');
		//wParam in this case is A handle to an internal structure describing the dropped files.
		if (DragQueryFile((HDROP)wParam, 0, filename.data(), filename.length()))
		{
			Window::droppedfile = std::move(filename);
			filedropped = true;
		}
		break;
	}
	}
	return DefWindowProc(hWnd, msg, wParam, lParam);
}


Window::Window(int x, int y, int width, int height, const std::string& title, HINSTANCE& hInstance)
	:
	hInstance(hInstance),
	maintitle(title)
{
	//first create the class structure
	wc.cbSize = sizeof(wc);
	wc.style = CS_OWNDC;
	wc.lpfnWndProc = WndProc;
	wc.hInstance = hInstance;
	wc.lpszClassName = title.c_str();
	wc.hbrBackground = (HBRUSH)COLOR_WINDOW;
	//then register the class
	RegisterClassEx(&wc);

	hWnd = CreateWindowEx(0, title.c_str(), title.c_str(), WS_BORDER | WS_VISIBLE | WS_MINIMIZEBOX | WS_SYSMENU |
		WS_CAPTION | WS_CLIPCHILDREN, x, y, width, height, nullptr, nullptr, hInstance, nullptr);
}

Window::Window(int x, int y, int width, int height, const std::string& title, HWND parenthWnd, const std::string& classname)
{

	if (classname == "Edit")
	{
		hWnd = CreateWindowEx(0, "Edit", title.c_str(), WS_CHILD | WS_VISIBLE | SS_CENTER | WS_BORDER | ES_MULTILINE
			| ES_AUTOVSCROLL | WS_VSCROLL, x, y, width, height, parenthWnd, nullptr, nullptr, nullptr);
	}
	else if (classname == "Static")
	{
		hWnd = CreateWindowEx(0, "Static", title.c_str(), WS_CHILD | WS_VISIBLE | SS_CENTER | WS_BORDER, x, y, width, height, parenthWnd, nullptr, nullptr, nullptr);
	}
}

void Window::SetText(const std::string& texttoset)
{
	SetWindowText(hWnd, texttoset.c_str());

}

bool Window::ProcessMessages()
{
	//peekmessage doesnt pause and returns if theres no message unlike getmessage
	//Message pump
	MSG msg;
	//If no messages are available, the return value is zero.
	while (GetMessage(&msg, nullptr, 0, 0))
	{
		//translatemessage will generate wm_char messages from our keydown messages
		TranslateMessage(&msg);
		//passes msg back to wnds32 side it checks the window sees what the pointer is in there and passes it to
		//our user-defined wndproc function which then calls defwndproc which gets the default behavior of the window
		//processing all messages
		DispatchMessage(&msg);
	}
	return false;
}

Window::~Window()
{
	UnregisterClass(maintitle.c_str(), hInstance);
}
