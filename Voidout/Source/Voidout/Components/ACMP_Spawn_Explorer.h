
#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "ACMP_Spawn_Explorer.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class VOIDOUT_API UACMP_Spawn_Explorer : public UActorComponent
{
	GENERATED_BODY()

public:

	// Sets default values for this component's properties
	UACMP_Spawn_Explorer();

	/**Actor to spawn*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawn Explorer")
	TSubclassOf<class ACHRT_Explorer> m_explorer{ nullptr };

	/**Time to wait to spawn*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Spawn Explorer")
	float m_time_to_spawn{ 2.f };

protected:

	FTimerHandle m_timerHandler;

	FVector m_newPos{ FVector::ZeroVector };

	class ATGPT_Explorer* m_point{nullptr};

	void spawnAfterTime();
public:

	void SpawnExplorer(class ATGPT_Explorer* initialPoint);

	float RandomFloat(float a, float b);

};
