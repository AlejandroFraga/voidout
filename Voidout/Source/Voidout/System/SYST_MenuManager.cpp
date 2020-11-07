
#include "SYST_MenuManager.h"
#include "Controller/CTRL_Player.h"
#include "UserWidget/USWG_MenuBase.h"

#include "Kismet/GameplayStatics.h"
#include "UObject/ConstructorHelpers.h"


USYST_MenuManager::USYST_MenuManager()
{
	/* MENUS */
	static ConstructorHelpers::FClassFinder<UUSWG_MenuBase> USWG_StartMenu_Class(TEXT("/Game/Blueprints/UserWidget/Menu/USWG_StartMenu"));
	startMenuClass = USWG_StartMenu_Class.Class;

	static ConstructorHelpers::FClassFinder<UUSWG_MenuBase> USWG_TutorialMenu_Class(TEXT("/Game/Blueprints/UserWidget/Tutorials/USWG_Tutorial"));
	tutorialClass = USWG_TutorialMenu_Class.Class;

	static ConstructorHelpers::FClassFinder<UUSWG_MenuBase> USWG_PauseMenu_Class(TEXT("/Game/Blueprints/UserWidget/Menu/USWG_PauseMenu"));
	pauseMenuClass = USWG_PauseMenu_Class.Class;

	static ConstructorHelpers::FClassFinder<UUSWG_MenuBase> USWG_EndMenu_Class(TEXT("/Game/Blueprints/UserWidget/Menu/USWG_EndMenu"));
	endMenuClass = USWG_EndMenu_Class.Class;
}

ACTRL_Player* USYST_MenuManager::GetCTRLPlayer()
{
	if (playerController == nullptr)
	{
		playerController = Cast<ACTRL_Player>(UGameplayStatics::GetPlayerController(this, 0));
	}
	return playerController;
}

void USYST_MenuManager::Initiate()
{
	//tutorial = CreateWidget<UUSWG_MenuBase>(this, tutorialClass);
	//AddMenu(tutorial, 0, true, false, false, false);

	//pauseMenu = CreateWidget<UUSWG_MenuBase>(this, pauseMenuClass);
	//AddMenu(pauseMenu, 998, false, false, false, false);

	//startMenu = CreateWidget<UUSWG_MenuBase>(this, startMenuClass);
	//AddMenu(startMenu, 999);
}
