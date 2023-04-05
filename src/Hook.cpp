#include "Hook.h"
namespace WaitMenuRedirected
{
	// https://github.com/max-su-2019/CommonLibSSE

	static void TweenMenu__CloseTweenMenu()
	{
		using func_t = decltype(&TweenMenu__CloseTweenMenu);
		REL::Relocation<func_t> func{ REL::VariantID(51839, 52711, 0x8FE180) };
		return func();
	}

	static void SleepWaitMenu__OpenSleepWaitMenu(bool isSleep)
	{
		using func_t = decltype(&SleepWaitMenu__OpenSleepWaitMenu);
		REL::Relocation<func_t> func{ REL::VariantID(51618, 52490, 0x8EAD70) };
		return func(isSleep);
	}

	void OpenWaitMenuHook::OpenWaitMenu(bool isSleep)
	{
		if (auto menuControls = RE::MenuControls::GetSingleton()) {
			if (menuControls->InBeastForm()) {
				SleepWaitMenu__OpenSleepWaitMenu(isSleep);
			}
		}
		return;
	}

	EventResult MenuControlsHook::ProcessEvent(RE::MenuControls* a_menuControls, RE::InputEvent* const* a_event, RE::BSTEventSource<RE::InputEvent*>* a_eventSource)
	{
		const auto ui = RE::UI::GetSingleton();

		auto IsTopMostInteractionMenu = [](const RE::UI* ui, const RE::GPtr<RE::IMenu>& iMenu) -> bool {
			for (auto iter = ui->menuStack.end() - 1; iter >= ui->menuStack.begin(); iter--) {
				auto menu = iter->get();
				if (menu && (menu->inputContext.get() < RE::IMenu::Context::kTotal || menu->PausesGame() || menu->UsesCursor())) {
					return menu == iMenu.get();
				}
			}

			return false;
		};

		const auto menuStringsHolder = RE::InterfaceStrings::GetSingleton();
		const auto tweenMenu = ui->GetMenu<RE::TweenMenu>(menuStringsHolder->tweenMenu);

		if (!a_menuControls->remapMode && !a_menuControls->isProcessing && tweenMenu && tweenMenu->OnStack() && IsTopMostInteractionMenu(ui, tweenMenu) && !ui->IsMenuOpen(menuStringsHolder->inventoryMenu)) {
			for (auto event = *a_event; event; event = event->next) {
				if (event->eventType != RE::INPUT_EVENT_TYPE::kButton)
					continue;

				const auto button = event->AsButtonEvent();
				if (!button || !button->IsDown())
					continue;

				const auto userEventHolder = RE::UserEvents::GetSingleton();

				if (_strcmpi(button->QUserEvent().c_str(), userEventHolder->wait.c_str()) == 0) {
					TweenMenu__CloseTweenMenu();
					SleepWaitMenu__OpenSleepWaitMenu(false);
					return EventResult::kStop;
				}
			}
		}

		return _ProcessEvent(a_menuControls, a_event, a_eventSource);
	}
}