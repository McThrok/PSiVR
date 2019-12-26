#include "Engine.h"

bool Engine::Initialize(HINSTANCE hInstance, std::string window_title, std::string window_class, int width, int height)
{
	timer.Start();

	if (!this->render_window.Initialize(this, hInstance, window_title, window_class, width, height))
		return false;

	gfx.simulation = &simulation;
	gfx.guiData = &guiData;
	simulation.Init();

	if (!gfx.Initialize(this->render_window.GetHWND(), width, height))
		return false;

	return true;
}

bool Engine::ProcessMessages()
{
	return this->render_window.ProcessMessages();
}

void Engine::Update()
{
	float dt = timer.GetMilisecondsElapsed();
	timer.Restart();

	while (!keyboard.CharBufferIsEmpty())
	{
		unsigned char ch = keyboard.ReadChar();
	}

	while (!keyboard.KeyBufferIsEmpty())
	{
		KeyboardEvent kbe = keyboard.ReadKey();
		unsigned char keycode = kbe.GetKeyCode();
	}

	while (!mouse.EventBufferIsEmpty())
	{
		MouseEvent me = mouse.ReadEvent();
		if (mouse.IsRightDown())
		{
			if (me.GetType() == MouseEvent::EventType::RAW_MOVE)
			{
				this->gfx.camera.AdjustRotation(-(float)me.GetPosY() * 0.01f, 0, -(float)me.GetPosX() * 0.01f);
			}
		}
	}


	if (keyboard.KeyIsPressed(0x10))
	{
		const float frameSpeed = 0.006f;
		if (keyboard.KeyIsPressed('W'))
		{
			this->simulation.AdjustFrame(this->gfx.camera.GetForwardVector() * frameSpeed * dt);
		}
		if (keyboard.KeyIsPressed('S'))
		{
			this->simulation.AdjustFrame(this->gfx.camera.GetBackwardVector() * frameSpeed * dt);
		}
		if (keyboard.KeyIsPressed('A'))
		{
			this->simulation.AdjustFrame(this->gfx.camera.GetLeftVector() * frameSpeed * dt);
		}
		if (keyboard.KeyIsPressed('D'))
		{
			this->simulation.AdjustFrame(this->gfx.camera.GetRightVector() * frameSpeed * dt);
		}
		if (keyboard.KeyIsPressed('Q'))
		{
			this->simulation.AdjustFrame({ 0.0f, 0.0f, frameSpeed * dt });
		}
		if (keyboard.KeyIsPressed('E'))
		{
			this->simulation.AdjustFrame({ 0.0f, 0.0f, -frameSpeed * dt });
		}
		gfx.UpdateFrameMesh();
	}
	else
	{
		const float cameraSpeed = 0.006f;
		if (keyboard.KeyIsPressed('W'))
		{
			this->gfx.camera.AdjustPosition(this->gfx.camera.GetForwardVector() * cameraSpeed * dt);
		}
		if (keyboard.KeyIsPressed('S'))
		{
			this->gfx.camera.AdjustPosition(this->gfx.camera.GetBackwardVector() * cameraSpeed * dt);
		}
		if (keyboard.KeyIsPressed('A'))
		{
			this->gfx.camera.AdjustPosition(this->gfx.camera.GetLeftVector() * cameraSpeed * dt);
		}
		if (keyboard.KeyIsPressed('D'))
		{
			this->gfx.camera.AdjustPosition(this->gfx.camera.GetRightVector() * cameraSpeed * dt);
		}
		if (keyboard.KeyIsPressed('Q'))
		{
			this->gfx.camera.AdjustPosition(0.0f, 0.0f, cameraSpeed * dt);
		}
		if (keyboard.KeyIsPressed('E'))
		{
			this->gfx.camera.AdjustPosition(0.0f, 0.0f, -cameraSpeed * dt);
		}
	}

	simulation.Update(dt);
}

void Engine::RenderFrame()
{
	this->gfx.RenderFrame();
}
