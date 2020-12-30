#pragma once
#include "Windows.h"
#include <string>
#include <vector>



class Window
{
public:
	Window(int x, int y, int width, int height, const std::string& title, HINSTANCE& hInstance);
	Window(int x, int y, int width, int height, const std::string& title, HWND parenthWnd, const std::string& classname);
	void SetText(const std::string& texttoset);

	HWND GetHandle() {
		return hWnd;
	}
	static bool FileWasDropped()
	{
		return filedropped;
	}
	static std::string GetDroppedfile()
	{
		std::string temp{ std::move(droppedfile) };
		filedropped = false;
		return temp;
	}
	static void EraseDroppedFile()
	{
		droppedfile.clear();
	}
	static bool ProcessMessages();
	~Window();
private:
	static LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
private:
	static bool filedropped;
	HWND hWnd;
	std::string maintitle;
	WNDCLASSEX wc{};
	HINSTANCE hInstance;
public:
	static std::string droppedfile;
};

