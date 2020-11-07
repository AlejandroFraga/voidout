
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "GMMD_Base.generated.h"

UCLASS()
class AGMMD_Base : public AGameModeBase
{
	GENERATED_BODY()

public:
	AGMMD_Base();

	virtual void PreInitializeComponents() override;
};
