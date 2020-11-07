
#include "ACTR_Light.h"
#include "Character/CHRT_Player.h"
#include "Components/ACMP_LightRaycasting.h"
#include "System/SYST_LogManager.h"
#include "System/SYST_SystemsManager.h"

#include "Components/BoxComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/SphereComponent.h"
#include "Components/SpotLightComponent.h"
#include "Engine/World.h"
#include "Kismet/GameplayStatics.h"
#include "TimerManager.h"


// Called when the game starts or when spawned
void AACTR_Light::BeginPlay()
{
	Super::BeginPlay();
	
	//if it is falling tower, it will be initialized by blueprint.
	if (m_isFallingTower)
		return; 

	float timeToInitialize = 0.1f;
	FTimerHandle m_timerHandler;
	//Wait dot one second to call the setUpLight function to avoid problems with bad initialized components when the diferent lights are spawned:
	GetWorld()->GetTimerManager().SetTimer(m_timerHandler, this, &AACTR_Light::setUpLight, timeToInitialize, false);
}

//setUpLight --> I initialize everything necessary preapre the lights operation.
void AACTR_Light::setUpLight()
{
	// LightCollision
	m_lightCollision = FindComponentByClass<UBoxComponent>();
	if (m_lightCollision == nullptr)
		USYST_LogManager::PrintError(this, "AACTR_Light.setUpLight: No m_lightCollision");

	// DamageSphere
	if (GetComponentsByTag(USphereComponent::StaticClass(), "DamageSphere").Num() > 0)
		m_damageSphere = Cast<USphereComponent>(GetComponentsByTag(USphereComponent::StaticClass(), "DamageSphere")[0]);
	else
		USYST_LogManager::PrintError(this, "AACTR_Light.setUpLight: No m_damageSphere");

	// SecureZoneSphere
	if (GetComponentsByTag(USphereComponent::StaticClass(), "SecureZoneSphere").Num() > 0)
		m_secureZoneSphere = Cast<USphereComponent>(GetComponentsByTag(USphereComponent::StaticClass(), "SecureZoneSphere")[0]);
	else
		USYST_LogManager::PrintError(this, "AACTR_Light.setUpLight: No m_secureZoneSphere");

	// LightZone
	if (GetComponentsByTag(USphereComponent::StaticClass(), "LightZone").Num() > 0)
		m_lightZone = Cast<USphereComponent>(GetComponentsByTag(USphereComponent::StaticClass(), "LightZone")[0]);
	else
		USYST_LogManager::PrintError(this, "AACTR_Light.setUpLight: No m_lightZone");

	// LightCollision
	m_light = FindComponentByClass<USpotLightComponent>();
	if (m_light == nullptr)
		USYST_LogManager::PrintError(this, "AACTR_Light.setUpLight: No m_light");

	//I prepare the damage collisions(overlaps)
	if (m_damageSphere != nullptr) {
		m_damageSphere->OnComponentBeginOverlap.AddDynamic(this, &AACTR_Light::OnOverlapBegin);
		m_damageSphere->OnComponentEndOverlap.AddDynamic(this, &AACTR_Light::OnOverlapEnd);
	}

	//Player and player health references
	m_player = Cast<ACHRT_Player>(UGameplayStatics::GetPlayerCharacter(this, 0));
	if (m_player != nullptr)
		m_playerRaycast = m_player->FindComponentByClass<UACMP_LightRaycasting>();

	if (m_lightCollision != nullptr && m_damageSphere != nullptr && m_secureZoneSphere != nullptr
		&& m_lightZone != nullptr && m_light != nullptr) {
		LightOnBehavior();//Put the collision spheres radius when the lights are on.
	}
}

//If the player is into the damage collider
void AACTR_Light::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (m_player != nullptr && OtherActor == m_player && OtherComp != nullptr && OtherComp->GetClass() == UCapsuleComponent::StaticClass()) {
		//The light can't collide if its a falling tower
		if (m_isFallingTower)
			return;
		m_isColliding = true;
		SYST_SystemsManager::GetSoundEventManager()->playEnergyTowerDamage(GetActorLocation());
	}
	return;
}

//If the player is leaving the damage collider
void AACTR_Light::OnOverlapEnd(UPrimitiveComponent * OverlappedComp, AActor * OtherActor, UPrimitiveComponent * OtherComp, int32 OtherBodyIndex)
{
	if (m_player != nullptr && OtherActor == m_player && OtherComp != nullptr && OtherComp->GetClass() == UCapsuleComponent::StaticClass()) {
		//The light can't collide if its a falling tower
		if (m_isFallingTower)
			return;
		m_isColliding = false;
		SYST_SystemsManager::GetSoundEventManager()->stopEnergyTowerDamage(GetActorLocation());
	}
	return;
}

//Function that cast a ray to calculate the distance between the light and the ground and also places the collision spheres in the right place.
float AACTR_Light::getLightHeight() const
{
	FCollisionQueryParams traceParams;
	traceParams.AddIgnoredActor(this);
	FHitResult hitResult;
	const int traceDistance = 400000;

	if (GetWorld()->LineTraceSingleByChannel(hitResult, m_light->K2_GetComponentLocation(), FVector::DownVector * traceDistance, ECC_Visibility, traceParams)) {

		//I put the triggers at the impact point:
		if(m_damageSphere != nullptr)
			m_damageSphere->SetWorldLocation(hitResult.ImpactPoint);

		if (m_secureZoneSphere != nullptr)
			m_secureZoneSphere->SetWorldLocation(hitResult.ImpactPoint);

		if (m_lightZone != nullptr)
			m_lightZone->SetWorldLocation(hitResult.ImpactPoint);

		return hitResult.Distance;
	}
	return 0;
}

//Public function, the switchLight component can call it when the light turns off to remove the light's triggers radius.
void AACTR_Light::LightOffBehavior() const
{
	if (m_damageSphere == nullptr) {
		return;
	}
	m_damageSphere->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	SYST_SystemsManager::GetSoundEventManager()->stopEnergyTowerDamage(GetActorLocation());
}

//Public function so that the switchLight component can call it when the light turns on, to set the correct light's triggers radius.
// It is also used to initialize light triggers when starting the game.
void AACTR_Light::LightOnBehavior() const
{
	if (m_damageSphere != nullptr)
		m_damageSphere->SetCollisionEnabled(ECollisionEnabled::QueryOnly);

	//The spotlight innerAngle must have the same radius as the outerAngle (+ 3.0f for better visual sensation):
	m_light->OuterConeAngle = m_light->InnerConeAngle + 3.0f;

	//calculate the light radius on the ground:
	const float height = getLightHeight();

	const float angle = 90 - m_light->InnerConeAngle;
	const float radianAngle = angle * (3.14f / 180.0f);
	const float hipo = (height) / (FMath::Sin(radianAngle));
	const float radius = (FMath::Cos(radianAngle)) * (hipo);
	
	//If the light is from a tower
	if (m_isTower) {

		if (m_damageSphere != nullptr)
			m_damageSphere->SetSphereRadius(radius, true);

		if (m_secureZoneSphere != nullptr)
			m_secureZoneSphere->SetSphereRadius(m_damageSphere->GetUnscaledSphereRadius() * m_multiplSecureZone);

		if (m_lightZone != nullptr)
			m_lightZone->SetSphereRadius(m_damageSphere->GetUnscaledSphereRadius(), true);

	}
	//If the light is from a farol
	else
	{
		if (m_damageSphere != nullptr)
			m_damageSphere->SetSphereRadius(radius, true);

		if (m_secureZoneSphere != nullptr)
			m_secureZoneSphere->SetSphereRadius(0.0f);

		if (m_lightZone != nullptr)
			m_lightZone->SetSphereRadius(0.0f);
	}
}

//Light modifications when uses farols
void AACTR_Light::setUpFarol(bool tower)
{
	m_isTower = tower;
}

float AACTR_Light::getlightArea() const noexcept
{
	if (m_damageSphere != nullptr)
		return m_damageSphere->GetUnscaledSphereRadius();
	return 0.0f;
}

float AACTR_Light::getSecureArea() const noexcept
{
	if (m_secureZoneSphere != nullptr)
		return m_secureZoneSphere->GetUnscaledSphereRadius();
	return 0.0f;
}

//Function to call the raycasts and make damage
void AACTR_Light::lightComprobations(float damageMultiplicator, float deltaSeconds, float delayUntilDamage, float &damage)
{
	 //Si es falling tower no hace daño.
	if (m_isFallingTower || m_player == nullptr)
		return;

	//If the character is colliding with the damage sphere.
	if (m_isColliding) {

		//If the player is dead it will be out of the sphere by the respawn, so it is not colliding.
		if (m_player->IsDying())
			m_isColliding = false;

		bool isHitedByLight = false;
		if (m_playerRaycast != nullptr)
		{
			//the player launch the rays to see if the light is hitting him.
			isHitedByLight = m_playerRaycast->RaycastShow(m_light->K2_GetComponentLocation(), m_lightCollision);
		}

		//If the light is hitting the player it takes life depending on the time spent in the light and the damage multiplier.
		if (isHitedByLight)
		{
			m_timeDamageCounter += deltaSeconds;

			//Delay to take damage, can be modified by design using the blueprint.
			if (m_timeDamageCounter >= delayUntilDamage)
			{
				m_timeDamageCounter = 0.0f;

				if (m_timeInsideLight < 1.5f)
				{
					m_timeInsideLight += deltaSeconds;
				}

				//As it gives the light it removes x damage.
				damage = damageMultiplicator * m_timeInsideLight * deltaSeconds;

				m_player->TakeDamage(damage);

				//The player cant recover health inside any light.
				m_player->SetCanRecover(false);
			}
			else
				damage = 0.0f;
		}
		//If the player is inside the light trigger but the light is not touching it (it is behind some object).
		else
			PlayerOutside(damage);
	}
	//If the player goes out of the light trigger.
	else
		PlayerOutside(damage);
}

void AACTR_Light::PlayerOutside(float& damage)
{
	damage = 0;
	//FIXME 0.1?
	m_timeInsideLight = 0.1f;
	//The player can recover health outside any light.
	m_player->SetCanRecover(true);
}

