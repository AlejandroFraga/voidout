
#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"

#include "Materials/MaterialInterface.h"
#include "Materials/Material.h"
#include "TimerManager.h"
#include "CollisionQueryParams.h"
#include "Engine/Engine.h"
#include "Kismet/GameplayStatics.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "Components/PrimitiveComponent.h"

#include "ACMP_FadeBlocking.generated.h"

USTRUCT()
struct FFadeObjStruct
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY()
	UPrimitiveComponent* primitiveComp;

	UPROPERTY()
	TArray<UMaterialInterface*> baseMatInterface;

	UPROPERTY()
	TArray<UMaterialInstanceDynamic*> fadeMID;

	UPROPERTY()
	float fadeCurrent;

	UPROPERTY()
	bool bToHide;

	void NewElement(UPrimitiveComponent* newComponent, TArray<UMaterialInterface*> newBaseMat,
		TArray<UMaterialInstanceDynamic*> newMID, float currentFade, bool bHide)
	{
		primitiveComp = newComponent;
		baseMatInterface = newBaseMat;
		fadeMID = newMID;
		fadeCurrent = currentFade;
		bToHide = bHide;
	}

	void SetHideOnly(bool hide)
	{
		bToHide = hide;
	}

	void SetFadeAndHide(float newFade, bool newHide)
	{
		fadeCurrent = newFade;
		bToHide = newHide;
	}

	//For Destroy
	void Destroy()
	{
		primitiveComp = nullptr;
	}

	//Constructor
	FFadeObjStruct()
	{
		primitiveComp = nullptr;
		fadeCurrent = 0;
		bToHide = true;
	}
};

/* Fade objects between camera manager and character
*
*/
UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class VOIDOUT_API UACMP_FadeBlocking : public UActorComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	UACMP_FadeBlocking();

	UFUNCTION(BlueprintCallable)
	void CleanFadeObjects();

	void SetEnable(bool _enable = true);

protected:
	// Called when the game starts
	void BeginPlay() override;

	// Check objects between camera manager and character and add to array for fade
	void AddObjectsToHide();

	// Fade worker.
	void FadeObjWorker();

private:

	TArray<FFadeObjStruct> fadeObjects;

	// Some worker timer
	FTimerHandle timerHandle_ObjectComputeTimer;

	FTimerHandle timerHandle_AddObjectsTimer;

	// Temp variable
	float currentFade = 0.f;

	// Now ID
	int32 fadeNowID = 0;

	// Primitive components temp variable
	TArray<UPrimitiveComponent*> fadeObjectsTemp;

	// Primitive components temp variable
	TArray<UPrimitiveComponent*> fadeObjectsHit;

	// Translucent material for fade object
	UPROPERTY(EditAnywhere, Category = "Fade Objects")
	UMaterial* fadeMaterial{ nullptr };

	// Enable or disable fade object worker
	UPROPERTY(EditAnywhere, Category = "Fade Objects")
	bool bEnable{ true };

	// Timer interval
	float addObjectInterval{ 0.1f };

	float calcFadeInterval{ 0.05f };

	UPROPERTY(EditAnywhere, Category = "Fade Objects")
	float workDistance{ 5000.f };

	UPROPERTY(EditAnywhere, Category = "Fade Objects")
	float nearCameraRadius{ 300.f };

	UPROPERTY(EditAnywhere, Category = "Fade Objects")
	UClass* playerClass{ nullptr };

	// Check trace block by this
	UPROPERTY(EditAnywhere, Category = "Fade Objects")
	TArray<TEnumAsByte<ECollisionChannel>> objectTypes;

	UPROPERTY(EditAnywhere, Category = "Fade Objects")
	TArray<AActor*> actorsIgnore;

	// Rate fade increment
	UPROPERTY(EditAnywhere, Category = "Fade Objects")
	float fadeRate{ 10.f };

	// Trace object size
	UPROPERTY(EditAnywhere, Category = "Fade Objects")
	float capsuleHalfHeight{ 10.f };

	// Trace object size
	UPROPERTY(EditAnywhere, Category = "Fade Objects")
	float capsuleRadius{ 5.f };

	// All characters array (maybe you control ( > 1 ) characters)
	TArray<AActor*> characterArray;

	// Fade near and close parameters
	UPROPERTY(EditAnywhere, Category = "Fade Objects")
	float nearObjectFade{ 0.3f };

	// Fade near and close parameters
	UPROPERTY(EditAnywhere, Category = "Fade Objects")
	float farObjectFade{ 0.1f };

	// Instance fade
	UPROPERTY(EditAnywhere, Category = "Fade Objects")
	float immediatelyFade{ 0.5f };
};
