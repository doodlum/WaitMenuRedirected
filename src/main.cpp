#include "Hook.h"

void Load()
{
	WaitMenuRedirected::MenuControlsHook::InstallHook();
	WaitMenuRedirected::OpenWaitMenuHook::Hook();
}