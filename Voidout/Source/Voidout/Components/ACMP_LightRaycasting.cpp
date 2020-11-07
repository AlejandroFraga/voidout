
#include "ACMP_LightRaycasting.h"
#include "Components/StaticMeshComponent.h"
#include "Assets/ACTR_Light.h"
#include "Components/SphereComponent.h"


// Sets default values for this component's properties
UACMP_LightRaycasting::UACMP_LightRaycasting()
{
	PrimaryComponentTick.bCanEverTick = false;
}

bool UACMP_LightRaycasting::ThrowRaycastToLight(FVector startPosition, FVector endPosition, UActorComponent* lightActorReference)
{
	if (!m_isRaycastPointingLight) {

		FHitResult hitResult;
		if (GetWorld()->LineTraceSingleByChannel(hitResult, startPosition, endPosition, ECC_Visibility, m_traceParams)) {
			// Si cualquiera de los puntos no esta dando a la luz, entonces se comprueba si realmente no le esta dando y si a uno de los puntos le esta dando 
			// la luz significa que al menos una parte del jugador esta reciviendo luz, por lo que tambien debe recibir daño:

			UActorComponent* actorComp = hitResult.GetComponent();
			if (actorComp == nullptr)
				return false;

			// Si el componente forma parte de una luz y además es una static mesh, o un collider esferico, que sea ignorado por el lanzado de rayos:
			if (Cast<AACTR_Light>(actorComp->GetOwner()) != nullptr && actorComp->GetClass() == UStaticMeshComponent::StaticClass()
				|| Cast<AACTR_Light>(actorComp->GetOwner()) != nullptr && actorComp->GetClass() == USphereComponent::StaticClass())
			{
				m_traceParams.AddIgnoredComponent(hitResult.GetComponent()); 
				// Devuelvo true porque si esta tocando una maya de una luz es que esta dentro de la luz.
				return true;
			}
			// Veo si la colision se produce realmente con el actor de la luz.
			return actorComp == lightActorReference;
		}
	}
	return false;
}

bool UACMP_LightRaycasting::RaycastShow(FVector lightPosition, UActorComponent* lightActor)
{
	//Cambiar el lightActor por un actor component
	AActor* owner = GetOwner();
	if (owner == nullptr)
		return false;

	const FVector playerHeadPosition = owner->GetActorLocation() + FVector(0,10,80);
	const FVector playerLeftPosition = owner->GetActorLocation() + FVector(-40,5,40);
	const FVector playerRightPosition = owner->GetActorLocation() + FVector(40,5,40);

	m_isRaycastPointingLight = false;

	//Tiro los 3 raycast a la luz para comprobar si estoy dentro o no.
	m_isRaycastPointingLight = ThrowRaycastToLight(playerHeadPosition,lightPosition, lightActor);
	m_isRaycastPointingLight = ThrowRaycastToLight(playerLeftPosition, lightPosition, lightActor);
	m_isRaycastPointingLight = ThrowRaycastToLight(playerRightPosition, lightPosition, lightActor);

	//Devuelve si te esta dando la luz o no, por lo que no hace falta comprobar nada más:
	return m_isRaycastPointingLight;
}

