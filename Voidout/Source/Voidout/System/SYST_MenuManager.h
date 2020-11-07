
#pragma once

#include "CoreMinimal.h"
#include "SYST_MenuManager.generated.h"

UCLASS()
class VOIDOUT_API USYST_MenuManager : public UObject
{
	GENERATED_BODY()

	/* VARIABLES */

	/* COMMON */

	class ACTRL_Player* playerController{ nullptr };

	/* MENUS */

	class UUSWG_MenuBase* activeMenu{ nullptr };

	bool isMenuShow{ false };

	TSubclassOf<UUSWG_MenuBase> startMenuClass{ nullptr };

	UUSWG_MenuBase* startMenu{ nullptr };

	TSubclassOf<UUSWG_MenuBase> tutorialClass{ nullptr };

	UUSWG_MenuBase* tutorial{ nullptr };

	TSubclassOf<UUSWG_MenuBase> pauseMenuClass{ nullptr };

	UUSWG_MenuBase* pauseMenu{ nullptr };

	TSubclassOf<UUSWG_MenuBase> endMenuClass{ nullptr };

	UUSWG_MenuBase* endMenu{ nullptr };

	/* FUNCTIONS */

	void Initiate();

	ACTRL_Player* GetCTRLPlayer();

public:

	/* VARIABLES*/

	/* FUNCTIONS */

	USYST_MenuManager();

};
