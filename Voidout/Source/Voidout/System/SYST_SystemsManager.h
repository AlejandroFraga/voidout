
#pragma once

#include "CoreMinimal.h"
#include <memory>
#include "System/SYST_SoundEventManager.h"

class VOIDOUT_API SYST_SystemsManager
{

	class USYST_DelayManager* delayManager{ nullptr };

	class USYST_MenuManager* menuManager{ nullptr };

	ASYST_SoundEventManager* soundEventManager{ nullptr };

	// ---------------------------------------------------------------------- METHODS.
	// Ctor.
	SYST_SystemsManager() = default;

	static void CleanClassActors(const TSubclassOf<ASYST_SoundEventManager>& soundEventManagerClass);

public:

	UWorld* world{ nullptr };

	// ---------------------------------------------------------------------- METHODS.
	// Instance.
	static SYST_SystemsManager* GetInst() noexcept
	{
		static const std::unique_ptr<SYST_SystemsManager> instance{ new SYST_SystemsManager() };
		return instance.get();
	}

	// Initiliaze.
	static void InitializeComponents(UWorld* WorldContext, const TSubclassOf<ASYST_SoundEventManager>& soundEventManagerClass);

	// Getters.
	static USYST_DelayManager* GetDelayManager() { return GetInst()->delayManager; }

	static USYST_MenuManager* GetMenuManager() { return GetInst()->menuManager; }

	static ASYST_SoundEventManager* GetSoundEventManager() { return GetInst()->soundEventManager; }
};
