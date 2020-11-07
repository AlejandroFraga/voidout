
#include "SYST_SystemsManager.h"
#include "Character/CHRT_Player.h"
#include "System/SYST_DelayManager.h"
#include "System/SYST_LogManager.h"
#include "System/SYST_MenuManager.h"

#include "Engine/Engine.h"
#include "Kismet/GameplayStatics.h"
#include "UObject/ConstructorHelpers.h"


void SYST_SystemsManager::InitializeComponents(UWorld* WorldContext, const TSubclassOf<ASYST_SoundEventManager>& soundEventManagerClass)
{
	// Initilize the Delay Manager
	GetInst()->delayManager = NewObject<USYST_DelayManager>();

	//GetInst()->menuManager = NewObject<USYST_MenuManager>();

	// Get the world context for future uses
	GetInst()->world = WorldContext;

	GetInst()->CleanClassActors(soundEventManagerClass);

	const FVector location = FVector::ZeroVector;
	const FRotator rotation = FRotator::ZeroRotator;
	FActorSpawnParameters ActorSpawnParams;
	ActorSpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;
	GetInst()->soundEventManager = Cast<ASYST_SoundEventManager>(GetInst()->world->SpawnActor<AActor>(soundEventManagerClass, location, rotation, ActorSpawnParams));
}

void SYST_SystemsManager::CleanClassActors(const TSubclassOf<ASYST_SoundEventManager>& soundEventManagerClass)
{
	TArray<AActor*> out;
	UGameplayStatics::GetAllActorsOfClass(GetInst()->world, soundEventManagerClass, out);
	for (auto* o : out)
	{
		o->Destroy();
	}
}
