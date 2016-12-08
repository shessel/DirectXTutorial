#include "pch.h"
#include "Demo.h"

using namespace Windows::ApplicationModel;
using namespace Windows::ApplicationModel::Core;
using namespace Windows::ApplicationModel::Activation;
using namespace Windows::UI::Core;
using namespace Windows::UI::Popups;
using namespace Windows::System;
using namespace Windows::Foundation;
using namespace Windows::Graphics::Display;
using namespace Platform;

ref class App sealed : public IFrameworkView {
	bool windowClosed;
	Demo demo;
public:
virtual void Initialize(CoreApplicationView^ AppView) {
		AppView->Activated += ref new TypedEventHandler
			<CoreApplicationView^, IActivatedEventArgs^>(this, &App::OnActivated);
		CoreApplication::Suspending += ref new EventHandler
			<SuspendingEventArgs^>(this, &App::OnSuspend);
		CoreApplication::Resuming += ref new EventHandler
			<Object^>(this, &App::OnResume);
		windowClosed = false;
	}

	virtual void SetWindow(CoreWindow^ Window) {
		Window->PointerPressed += ref new TypedEventHandler
			<CoreWindow^, PointerEventArgs^>(this, &App::OnPointerPressed);
		Window->KeyDown += ref new TypedEventHandler
			<CoreWindow^, KeyEventArgs^>(this, &App::OnKeyDown);
		Window->Closed += ref new TypedEventHandler
			<CoreWindow^, CoreWindowEventArgs^>(this, &App::OnClose);
		Window->SizeChanged += ref new TypedEventHandler
			<CoreWindow^, WindowSizeChangedEventArgs^>(this, &App::OnSizeChanged);
	}

	virtual void Load(String^ EntryPoint) {}

	virtual void Run() {
		demo.Initialize();
		CoreWindow^ Window = CoreWindow::GetForCurrentThread();
		while (!windowClosed) {
			Window->Dispatcher->ProcessEvents(CoreProcessEventsOption::ProcessAllIfPresent);
			demo.Update();
			demo.Render();
		}
	}

	virtual void Uninitialize() {}

	void OnActivated(CoreApplicationView^ CoreAppView, IActivatedEventArgs^ Args) {
		CoreWindow^ Window = CoreWindow::GetForCurrentThread();
		Window->Activate();
	}

	void OnPointerPressed(CoreWindow^ Window, PointerEventArgs^ Args) {
		MessageDialog Dialog("Hello there!", "Whoop whoop!");
		Dialog.ShowAsync();
	}

	void OnKeyDown(CoreWindow^ Window, KeyEventArgs^ Args) {
		MessageDialog Dialog(Args->VirtualKey.ToString(), "Key!");
		Dialog.ShowAsync();
	}

	void OnClose(CoreWindow^ Window, CoreWindowEventArgs^ Args) {
		windowClosed = true;
	}

	void OnSizeChanged(CoreWindow^ Window, WindowSizeChangedEventArgs^ Args) {
		demo.Resize();
	}

	void OnSuspend(Object^ Sender, SuspendingEventArgs^ Args) {}
	void OnResume(Object^ Sender, Object^ Args) {}
};


// the class definition that creates an instance of our core framework class
ref class AppSource sealed : IFrameworkViewSource
{
public:
	virtual IFrameworkView^ CreateView() {
		return ref new App();
	}
};

[MTAThread]    // define main() as a multi-threaded-apartment function
int main(Array<String^>^ args) {
	CoreApplication::Run(ref new AppSource());
	return 0;
}