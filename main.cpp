#include <filesystem>
#include <iostream>
#include <stdio.h>
#include <assert.h>

#include <Babylon/AppRuntime.h>
#include <Babylon/Graphics/Device.h>
#include <Babylon/ScriptLoader.h>
#include <Babylon/Plugins/NativeEngine.h>
#include <Babylon/Plugins/NativeOptimizations.h>
#include <Babylon/Plugins/NativeInput.h>
#include <Babylon/Polyfills/Console.h>
#include <Babylon/Polyfills/Window.h>
#include <Babylon/Polyfills/XMLHttpRequest.h>
#include <Babylon/Polyfills/Canvas.h>

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#if GLFW_VERSION_MINOR < 2
#error "GLFW 3.2 or later is required"
#endif // GLFW_VERSION_MINOR < 2

#if TARGET_PLATFORM_LINUX
#define GLFW_EXPOSE_NATIVE_X11
#elif TARGET_PLATFORM_OSX
#define GLFW_EXPOSE_NATIVE_COCOA
#elif TARGET_PLATFORM_WINDOWS
#define GLFW_EXPOSE_NATIVE_WIN32
#endif //
#include <GLFW/glfw3native.h>

static void* glfwNativeWindowHandle(GLFWwindow* _window)
{
#if TARGET_PLATFORM_LINUX
	return (void*)(uintptr_t)glfwGetX11Window(_window);
#elif TARGET_PLATFORM_OSX
	return ((NSWindow*)glfwGetCocoaWindow(_window)).contentView;
#elif TARGET_PLATFORM_WINDOWS
	return glfwGetWin32Window(_window);
#endif // TARGET_PLATFORM_
}

std::unique_ptr<Babylon::AppRuntime> g_runtime{};
std::unique_ptr<Babylon::Graphics::Device> g_device{};
std::unique_ptr<Babylon::Graphics::DeviceUpdate> g_update{};
std::unique_ptr<Babylon::Polyfills::Canvas> g_nativeCanvas{};
Babylon::Plugins::NativeInput* g_nativeInput{};

#define INITIAL_WIDTH 1920
#define INITIAL_HEIGHT 1080

void Uninitialize()
{
	if (g_device)
	{
		g_update->Finish();
		g_device->FinishRenderingCurrentFrame();
	}

	g_nativeInput = {};
	g_runtime.reset();
	g_nativeCanvas.reset();
	g_update.reset();
	g_device.reset();
}

void RefreshBabylon(GLFWwindow *window)
{
	Uninitialize();

	int width, height;
	glfwGetWindowSize(window, &width, &height);

	Babylon::Graphics::Configuration graphicsConfig{};
	graphicsConfig.Window = (Babylon::Graphics::WindowT)glfwNativeWindowHandle(window);
	graphicsConfig.Width = width;
	graphicsConfig.Height = height;
	graphicsConfig.MSAASamples = 4;

	g_device = std::make_unique<Babylon::Graphics::Device>(graphicsConfig);
	g_update = std::make_unique<Babylon::Graphics::DeviceUpdate>(g_device->GetUpdate("update"));
	g_device->StartRenderingCurrentFrame();
	g_update->Start();

	g_runtime = std::make_unique<Babylon::AppRuntime>();

	g_runtime->Dispatch([](Napi::Env env)
	{
		g_device->AddToJavaScript( env );

		Babylon::Polyfills::Console::Initialize( env, []( const char* message, auto ) {
			std::cout << message << std::endl;
		} );

		Babylon::Polyfills::Window::Initialize( env );
		Babylon::Polyfills::XMLHttpRequest::Initialize( env );
		g_nativeCanvas = std::make_unique <Babylon::Polyfills::Canvas>( Babylon::Polyfills::Canvas::Initialize( env ) );
		Babylon::Plugins::NativeEngine::Initialize( env );
		Babylon::Plugins::NativeOptimizations::Initialize( env );
		g_nativeInput = &Babylon::Plugins::NativeInput::CreateForJavaScript( env );
	});

	Babylon::ScriptLoader loader{*g_runtime};
	loader.LoadScript("app:///Scripts/bundle.js");
}

static void KeyCallback(GLFWwindow *window, int key, int scancode, int action, int mods)
{
	if (key == GLFW_KEY_R && action == GLFW_PRESS)
	{
		RefreshBabylon(window);
	}
}

static void MouseButtonCallback(GLFWwindow *window, int button, int action, int mods)
{
	if (g_nativeInput == nullptr)
		return;
	
	double xpos, ypos;
	glfwGetCursorPos(window, &xpos, &ypos);
	int32_t x = static_cast<int32_t>(xpos);
	int32_t y = static_cast<int32_t>(ypos);

	if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS)
		g_nativeInput->MouseDown(Babylon::Plugins::NativeInput::LEFT_MOUSE_BUTTON_ID, x, y);
	else if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_RELEASE)
		g_nativeInput->MouseUp(Babylon::Plugins::NativeInput::LEFT_MOUSE_BUTTON_ID, x, y);
	else if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_PRESS)
		g_nativeInput->MouseDown(Babylon::Plugins::NativeInput::RIGHT_MOUSE_BUTTON_ID, x, y);
	else if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_RELEASE)
		g_nativeInput->MouseUp(Babylon::Plugins::NativeInput::RIGHT_MOUSE_BUTTON_ID, x, y);
	else if (button == GLFW_MOUSE_BUTTON_MIDDLE && action == GLFW_PRESS)
		g_nativeInput->MouseDown(Babylon::Plugins::NativeInput::MIDDLE_MOUSE_BUTTON_ID, x, y);
	else if (button == GLFW_MOUSE_BUTTON_MIDDLE && action == GLFW_RELEASE)
		g_nativeInput->MouseUp(Babylon::Plugins::NativeInput::MIDDLE_MOUSE_BUTTON_ID, x, y);
}

static void CursorPositionCallback(GLFWwindow *window, double xpos, double ypos)
{
	if (g_nativeInput == nullptr)
		return;
	
	int32_t x = static_cast<int32_t>(xpos);
	int32_t y = static_cast<int32_t>(ypos);

	g_nativeInput->MouseMove(x, y);
}

static void ScrollCallback(GLFWwindow *window, double xoffset, double yoffset)
{
	if (g_nativeInput == nullptr)
		return;

	g_nativeInput->MouseWheel(Babylon::Plugins::NativeInput::MOUSEWHEEL_Y_ID, static_cast<int>(-yoffset * 100.0));
}

static void WindowResizeCallback(GLFWwindow *window, int width, int height)
{
	if (g_device == nullptr)
		return;
	
	g_device->UpdateSize(width, height);
}

int main()
{
	if (!glfwInit())
		exit(EXIT_FAILURE);

	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
	glfwWindowHint(GLFW_MAXIMIZED, GLFW_TRUE);

	auto window = glfwCreateWindow(INITIAL_WIDTH, INITIAL_HEIGHT, "Simple example", NULL, NULL);

	if (!window)
	{
		glfwTerminate();
		exit(EXIT_FAILURE);
	}

	glfwSetKeyCallback(window, KeyCallback);
	glfwSetWindowSizeCallback(window, WindowResizeCallback);
	glfwSetCursorPosCallback(window, CursorPositionCallback);
	glfwSetMouseButtonCallback(window, MouseButtonCallback);
	glfwSetScrollCallback(window, ScrollCallback);

	RefreshBabylon(window);

	while (!glfwWindowShouldClose(window))
	{
		if (g_device)
		{
			g_update->Finish();
			g_device->FinishRenderingCurrentFrame();
			g_device->StartRenderingCurrentFrame();
			g_update->Start();
		}
		glfwPollEvents();	
	}

	Uninitialize();
	glfwDestroyWindow(window);
	glfwTerminate();
	exit(EXIT_SUCCESS);
}