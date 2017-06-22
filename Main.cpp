
# include <Siv3D.hpp>
#include "Planet.h"

void Main()
{
	Window::SetTitle(L"Age of Discovery");
	Window::Resize(1280, 720);
	Window::SetStyle(WindowStyle::Sizeable);
	Planet planet(512, 0.050);

	while (System::Update())
	{
		if (Input::KeyF5.clicked) planet.recreate();
		planet.update();
		planet.draw();
	}
}
