
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SYST_MapLoader.generated.h"

UCLASS()
class VOIDOUT_API ASYST_MapLoader : public AActor
{
	GENERATED_BODY()

	// Trigger.
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	class UBoxComponent* m_overlapTrg{ nullptr };

	bool beginUsed{ false };

	bool endUsed{ false };

	bool IsPlayer(AActor* otherActor, UPrimitiveComponent* otherComp);

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite) USceneComponent* m_scene{};
	
	// Sets default values for this actor's properties
	ASYST_MapLoader();

	// Begin Play
	void BeginPlay() override;

	// Map to load string.
	UPROPERTY(EditAnywhere, Category = "Map Loader", BlueprintReadWrite)
	FName m_mapToLoad{ "" };

	// Map to unload.
	UPROPERTY(EditAnywhere, Category = "Map Loader", BlueprintReadWrite)
	FName m_mapToUnload{ "" };

	// Map to unload.
	UPROPERTY(EditAnywhere, Category = "Map Loader", BlueprintReadWrite)
	AActor* m_energyBarrier{ nullptr };
	
protected:
	// On overlap.
	UFUNCTION()
	void OverlapBegin(UPrimitiveComponent* ovelapedComp, AActor* otherActor, UPrimitiveComponent* otherComp, int32 otherBodyIdx, bool fromSweep, const FHitResult& sweepRes);
	
	// End overlap.
	UFUNCTION()
	void OverlapEnd(UPrimitiveComponent* ovelapedComp, AActor* otherActor, UPrimitiveComponent* otherComp, int32 otherBodyIdx);
};
