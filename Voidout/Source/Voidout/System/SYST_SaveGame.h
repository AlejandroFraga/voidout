
#pragma once

#include <vector>

#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"
#include "Assets/ACTR_EnergyBarrier.h"
#include "SYST_SaveGame.generated.h"

class ATGPT_Explorer;
class AACTR_EnergyTower;
class AACTR_PowerGenerator;
class AACTR_EnegryBarrier;
//Struct with data to save in an AActor (player + any other actor tagged with "Save")
USTRUCT(BlueprintType)
struct FActorRecord
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(BlueprintReadWrite) AActor* m_spawnedActor{ nullptr };
	UPROPERTY(BlueprintReadWrite) UClass* m_class{ nullptr };
	UPROPERTY(BlueprintReadWrite) FName m_name{};
	UPROPERTY(BlueprintReadWrite) FTransform m_transform{};
	UPROPERTY(BlueprintReadWrite) float m_lifespan{ 0.0f };
};

USTRUCT()
struct FHashEx
{
	GENERATED_BODY()
	
	// Actors.
	UPROPERTY() TArray<class ATGPT_Explorer*> m_actors{ };

	// Emplace.
	void Emplace(ATGPT_Explorer* ac) noexcept { m_actors.Emplace(ac); }
};

USTRUCT()
struct FHashComm
{
	GENERATED_BODY()
	
	// Actors.
	UPROPERTY() TArray<AActor*> m_actors{ };

	UPROPERTY() int m_nMinons{ 3 };

	//Energy barrier
	UPROPERTY() class AACTR_EnergyBarrier* m_asignedEnergyBarrier;

	// Emplace.
	void Emplace(AActor* ac) noexcept { m_actors.Emplace(ac); }

	// Ctor
	FHashComm() noexcept = default;
};


USTRUCT()
struct FShootingValues
{
	GENERATED_BODY()


	FShootingValues()
		: FShootingValues(0.1, 0.75, 0.75, 0.85)
	{ }
	
	FShootingValues(float dontShootThr, float successShootThrOriginal, float successShootThr, float successFlaredThr)
		: m_dontShootThr(dontShootThr)
		, m_successShootThrOriginal(successShootThrOriginal)
		, m_successShootThr(successShootThr)
		, m_successFlaredThr(successFlaredThr)
	{}
	
	UPROPERTY() float m_dontShootThr{ 0.1f };							// Unshoot threshold
	UPROPERTY() float m_successShootThrOriginal{ 0.75f };				// Success shoot threshhold.
	UPROPERTY() float m_successShootThr{ m_successShootThrOriginal };	// SuccessShoot.
	UPROPERTY() float m_successFlaredThr{ 0.85f };						// Flared.
};

UCLASS()
class USYST_SaveGame : public USaveGame
{
	GENERATED_BODY()

	// Array of actors to save.
	UPROPERTY() TArray<AActor*> m_actors{};
	UPROPERTY() TArray<FTransform> m_transforms{};
	UPROPERTY() TArray<FShootingValues> m_shoots{};
	
	
	UPROPERTY() TMap<int, FHashEx> m_explorers;
	UPROPERTY() TMap<int, FHashComm> m_commanders;
	

	
	
public:

	UPROPERTY(BlueprintReadWrite)
	FRotator m_playerControllerRotator{};

	//struct instance with data needed in the load-save process
	UPROPERTY(BlueprintReadWrite)
	FActorRecord m_player{};

	AACTR_EnergyBarrier* commanderWall{ nullptr };

	//array with struct instances with data needed in the load-save process
	//of actors tagged as "save"
	UPROPERTY(BlueprintReadWrite)
	TArray<FActorRecord> m_actRecords{};

	UFUNCTION(BlueprintCallable)
	void SaveGame(APawn* PlayerActor);

	UFUNCTION(BlueprintCallable, meta = (WorldContext = "WorldContextObject"))
	void LoadGame(UObject* WorldContextObject);

	UFUNCTION(BlueprintCallable)
	void UnSubscribe();
};

