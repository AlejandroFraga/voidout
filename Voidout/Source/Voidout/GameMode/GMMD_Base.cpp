// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.


#include "GMMD_Base.h"
#include "Character/CHRT_Player.h"
#include "Controller/CTRL_Player.h"
#include "GameInstance/GMIN_Base.h"
#include "HUD/HUD_Base.h"
#include "System/SYST_LogManager.h"
#include "System/SYST_SystemsManager.h"

#include "UObject/ConstructorHelpers.h"
#include "Kismet/GameplayStatics.h"


AGMMD_Base::AGMMD_Base()
{
	// set default controller class
	static ConstructorHelpers::FClassFinder<ACTRL_Player> CTRL_Player_BPClass(TEXT("/Game/Blueprints/Controller/CTRL_Player_BP"));
	if (CTRL_Player_BPClass.Class != nullptr)
	{
		PlayerControllerClass = CTRL_Player_BPClass.Class;
	}
	else
	{
		PlayerControllerClass = ACTRL_Player::StaticClass();
	}

	// set default pawn class
	static ConstructorHelpers::FClassFinder<ACHRT_Player> CHRT_Player_BPClass(TEXT("/Game/Blueprints/Character/CHRT_Player_BP"));
	if (CHRT_Player_BPClass.Class != nullptr)
	{
		DefaultPawnClass = CHRT_Player_BPClass.Class;
	}
	else
	{
		DefaultPawnClass = ACHRT_Player::StaticClass();
	}

	// set default hud class
	static ConstructorHelpers::FClassFinder<AHUD_Base> HUD_Base_BPClass(TEXT("/Game/Blueprints/HUD/HUD_Base_BP"));
	if (HUD_Base_BPClass.Class != nullptr)
	{
		HUDClass = HUD_Base_BPClass.Class;
	}
	else
	{
		HUDClass = AHUD_Base::StaticClass();
	}
}

void AGMMD_Base::PreInitializeComponents()
{
	Super::PreInitializeComponents();

	UGMIN_Base* gameInstance = Cast<UGMIN_Base>(UGameplayStatics::GetGameInstance(this));
	if (gameInstance != nullptr)
	{
		gameInstance->UpdateSystemManager();
	}
}
