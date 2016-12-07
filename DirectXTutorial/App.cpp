#include "pch.h"

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
public:
virtual void Initialize(CoreApplicationView^ AppView) {
		AppView->Activated += ref new TypedEventHandler
			<CoreApplicationView^, IActivatedEventArgs^>(this, &App::OnActivated);
		CoreApplication::Suspending += ref new EventHandler
			<SuspendingEventArgs^>(this, &App::OnSuspend);
		CoreApplication::Resuming += ref new EventHandler
			<Object^>(this, &App::OnResume);
	}

	virtual void SetWindow(CoreWindow^ Window) {
		Window->PointerPressed += ref new TypedEventHandler
			<CoreWindow^, PointerEventArgs^>(this, &App::OnPointerPressed);
		Window->KeyDown += ref new TypedEventHandler
			<CoreWindow^, KeyEventArgs^>(this, &App::OnKeyDown);
	}

	virtual void Load(String^ EntryPoint) {}

	virtual void Run() {
		CoreWindow^ Window = CoreWindow::GetForCurrentThread();
		Window->Dispatcher->ProcessEvents(CoreProcessEventsOption::ProcessUntilQuit);
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