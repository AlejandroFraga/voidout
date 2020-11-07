
#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "CollisionQueryParams.h"
#include "ACMP_LightRaycasting.generated.h"

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class VOIDOUT_API UACMP_LightRaycasting : public UActorComponent
{
	GENERATED_BODY()

	bool ThrowRaycastToLight(FVector startPosition, FVector endPosition, UActorComponent* lightActorReference);

	bool m_isRaycastPointingLight = false;

	FCollisionQueryParams m_traceParams;

public:	
	// Sets default values for this component's properties
	UACMP_LightRaycasting();

	UFUNCTION(BlueprintCallable)
	bool RaycastShow(FVector lightPosition, UActorComponent* lightActor);

};
