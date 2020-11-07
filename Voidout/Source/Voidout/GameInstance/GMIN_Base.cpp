
#include "GMIN_Base.h"
#include "Controller/CTRL_Player.h"
#include "System/SYST_LogManager.h"
#include "System/SYST_SoundEventManager.h"
#include "System/SYST_SystemsManager.h"

#include "GameFramework/GameUserSettings.h"
#include "Kismet/GameplayStatics.h"
#include "UObject/ConstructorHelpers.h"
#include "FMODStudioModule.h"

UGMIN_Base::UGMIN_Base()
{
	IFMODStudioModule::Get();
	// set default sound manager class
	static ConstructorHelpers::FClassFinder <ASYST_SoundEventManager> SYST_SoundEventManager_BP_Class(TEXT("/Game/Blueprints/System/SYST_SoundEventManager_BP"));
	
	if (SYST_SoundEventManager_BP_Class.Class != nullptr)
	{
		soundEventManagerClass = SYST_SoundEventManager_BP_Class.Class;
	}
	else
	{
		soundEventManagerClass = ASYST_SoundEventManager::StaticClass();
	}
}

void UGMIN_Base::UpdateSystemManager()
{
	UWorld* world = GetWorld();
	if (world != nullptr)
	{
		SYST_SystemsManager::InitializeComponents(world, soundEventManagerClass);
	}
}

ASYST_SoundEventManager* UGMIN_Base::GetSoundEventManager()
{
	return SYST_SystemsManager::GetSoundEventManager();
}

//FIXME Until Menu Manager
bool UGMIN_Base::ShowTutorial(bool show, bool pause, bool setActive)
{
	APlayerController* playerCont = UGameplayStatics::GetPlayerController(this, 0);
	if (playerCont == nullptr)
		return false;

	ACTRL_Player* playerCTRL = Cast<ACTRL_Player>(playerCont);
	if (playerCTRL != nullptr)
	{
		return playerCTRL->ShowTutorial(show, pause, setActive);
	}
	return false;
}