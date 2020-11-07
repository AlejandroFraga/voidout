
#include "ACTR_SwitchableLight.h"
#include "Character/CHRT_Enemy.h"
#include "Character/CHRT_Explorer.h"
#include "Components/ACMP_Health.h"
#include "Components/ACMP_LightStructureSubscription.h"
#include "System/SYST_LogManager.h"

#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Blueprint/AIBlueprintHelperLibrary.h"
#include "BrainComponent.h"
#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Components/PointLightComponent.h"
#include "Components/SphereComponent.h"
#include "Components/SpotLightComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Components/WidgetComponent.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "TimerManager.h"
#include "UObject/ConstructorHelpers.h"
#include "Math/UnrealMathVectorCommon.h"
#include "System/SYST_LogManager.h"


// Sets default values
AACTR_SwitchableLight::AACTR_SwitchableLight()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

void AACTR_SwitchableLight::BeginPlay()
{
	Super::BeginPlay();

	InitializeSwitchLight();
}

void AACTR_SwitchableLight::InitializeSwitchLight()
{
	TArray<UActorComponent*> actorComp = GetComponentsByTag(USphereComponent::StaticClass(), "LightZone");
	if(actorComp.Num() >= 0 && actorComp[0] != nullptr)
		m_collisionLightZone = Cast<USphereComponent>(actorComp[0]);

	m_light = Cast<USpotLightComponent>(GetComponentByClass(USpotLightComponent::StaticClass()));

	TArray<UActorComponent*> pointLights = GetComponentsByClass(UPointLightComponent::StaticClass());
	pointLights.Remove(m_light);
	if (pointLights.Num() > 0 && pointLights[0] != nullptr)
	{
		pointLight1 = Cast<UPointLightComponent>(pointLights[0]);

		if (pointLights.Num() > 1 && pointLights[1] != nullptr)
			pointLight2 = Cast<UPointLightComponent>(pointLights[1]);
	}

	actorComp = GetComponentsByTag(UStaticMeshComponent::StaticClass(), "LightStructureMesh");
	if (actorComp.Num() >= 0 && actorComp[0] != nullptr)
	{
		m_meshComponent = Cast<UStaticMeshComponent>(actorComp[0]);
		m_lightMaterial = m_meshComponent->GetMaterial(1);
		m_crystalMaterial = m_meshComponent->GetMaterial(0);
	}

	actorComp = GetComponentsByTag(UStaticMeshComponent::StaticClass(), "RingPulse");
	if (actorComp.Num() >= 0 && actorComp[0] != nullptr)
		m_ringPulse = Cast<UStaticMeshComponent>(actorComp[0]);


	if (m_light != nullptr)
		m_initialLightIntensity = m_light->Intensity;

	if (m_collisionLightZone != nullptr)
	{
		m_collisionLightZone->OnComponentBeginOverlap.AddDynamic(this, &AACTR_SwitchableLight::OnLightZoneOverlapBegin);
		m_collisionLightZone->OnComponentEndOverlap.AddDynamic(this, &AACTR_SwitchableLight::OnLightZoneOverlapEnd);
	}
}

//Función solo útil para usar con las torres (Detecta a todos los enemigos que hay en la torre para hacer voidout): 
void AACTR_SwitchableLight::OnLightZoneOverlapBegin(UPrimitiveComponent * OverlappedComp, AActor * OtherActor, UPrimitiveComponent * OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult)
{
	auto* pawn = Cast<ACHRT_Enemy>(OtherActor); // If this cast fails, it isn't a enemy.
	if (pawn == nullptr)
		return;

	m_enemiesInLightZone.AddUnique(OtherActor); //Añado al actor en la lista de zona segura.
}

//Función solo útil para usar con las torres: 
void AACTR_SwitchableLight::OnLightZoneOverlapEnd(UPrimitiveComponent * OverlappedComp, AActor * OtherActor, UPrimitiveComponent * OtherComp, int32 OtherBodyIndex)
{
	auto* pawn = Cast<ACHRT_Enemy>(OtherActor); // If this cast fails, it isn't a enemy.
	if (pawn == nullptr)
		return;

	m_enemiesInLightZone.Remove(OtherActor); //Saco al actor de la lista de zona segura.
}

void AACTR_SwitchableLight::SwitchLight(float voidoutDuration, int voidoutImpulse)
{
	if (!m_isLightOnVoidout || !m_isLightOnPermanent)
		return;

	if (m_light != nullptr)
		m_light->SetIntensity(0);

	if (m_meshComponent != nullptr) {
		m_meshComponent->SetMaterial(1, m_crystalMaterial);//El material de la luz esta el segundo en el array de materiales de la mesh.
		m_meshComponent->SetMaterial(0, m_defaultTowerMaterial);
	}

	if (m_ringPulse != nullptr)
		m_ringPulse->SetVisibility(false);

	//Si la luz esta enchufada se apaga
	TurnOnLight(pointLight1, false);
	TurnOnLight(pointLight2, false);
	m_isLightOnVoidout = false;
	m_impulseForce = voidoutImpulse;
	m_timeWithLightOff = voidoutDuration;
	LightOffBehavior(); //Activar cuando se pueda seleccionar que luz apagar.
}

void AACTR_SwitchableLight::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (!m_isLightOnPermanent && m_light != nullptr)
		m_light->SetIntensity(FMath::FInterpTo(m_light->Intensity, 0, DeltaTime, m_speedLightOff));

	//Para enchufar la luz automaticamente cuando lleva x segundos apagada, si la luz esta completamente inactiva no debería iniciarse nunca.
	if (m_isLightOnVoidout || !m_isLightOnPermanent)
		return;

	//Para realizar la acción una sola vez y no cada ciclo.
	if (m_timeCounter == 0)
		IsLightOffBehavior();

	m_timeCounter += DeltaTime;
	if (m_timeCounter < m_timeWithLightOff)
		return;
	
	if (m_light != nullptr)
		m_light->SetIntensity(m_initialLightIntensity);

	if (m_meshComponent != nullptr) {
		m_meshComponent->SetMaterial(1, m_lightMaterial);
		m_meshComponent->SetMaterial(0, m_crystalMaterial);
	}

	if (m_ringPulse != nullptr)
		m_ringPulse->SetVisibility(true);

	TurnOnLight(pointLight1, true);
	TurnOnLight(pointLight2, true);
	m_isLightOnVoidout = true;
	m_timeCounter = 0;
	LightOnBehavior();

	//Recorro el array de enemigos stuneados al apagar la luz, para volver a activar la IA de estos enemigos cuando la luz se ha vuelto a encender.
	for (AActor* e : m_stunedEnemies)
	{
		if (e == nullptr)
			continue;

		AAIController* aiController = UAIBlueprintHelperLibrary::GetAIController(e);
		if (aiController == nullptr || aiController->GetBrainComponent() == nullptr || aiController->GetBrainComponent()->IsActive())
			continue;

		UACMP_Health* enemyHealth = Cast<UACMP_Health>(e->GetComponentByClass(UACMP_Health::StaticClass()));

		//Si el enemigo tiene componente de vida y resulta que ha muerto durante voidout no hay que reactivar su logica.
		if (enemyHealth != nullptr && enemyHealth->m_dying)
			continue;

		aiController->GetBrainComponent()->Activate();
		aiController->GetBrainComponent()->GetBlackboardComponent()->SetValueAsBool("AbleToShoot", true);
		aiController->GetBrainComponent()->RestartLogic();

		ACHRT_Explorer* explorer = Cast<ACHRT_Explorer>(e);
		if (explorer != nullptr)
		{
			UBlackboardComponent* pBlackboardComponent = aiController->GetBrainComponent()->GetBlackboardComponent();
			pBlackboardComponent->SetValueAsBool("isOnPositionToShoot", false);
			pBlackboardComponent->SetValueAsInt("TargetPointIndex", 1);
		}
	}
	m_stunedEnemies.Empty();
}

void AACTR_SwitchableLight::AddConnectionCount(int numberToadd)
{
	m_connectionCount += numberToadd;
}

void AACTR_SwitchableLight::LightOnPermanent()
{
	if (m_light != nullptr)
		m_light->SetIntensity(m_initialLightIntensity); //Encendido visual de la luz.

	if (m_meshComponent != nullptr) {
		m_meshComponent->SetMaterial(1, m_lightMaterial);
		m_meshComponent->SetMaterial(0, m_crystalMaterial);
	}

	if (m_ringPulse != nullptr)
		m_ringPulse->SetVisibility(true);

	TurnOnLight(pointLight1, true);
	TurnOnLight(pointLight2, true);
	m_isLightOnPermanent = true; //La luz se queda encendida permanentemente.
	LightOnBehavior(); //Encender tambien el daño al player.
}

void AACTR_SwitchableLight::LightOffPermanent()
{
	if (m_light != nullptr)
		//m_light->SetIntensity(0); //Apagado visual de la luz.

	if (m_meshComponent != nullptr) {
		m_meshComponent->SetMaterial(1, m_crystalMaterial);
		m_meshComponent->SetMaterial(0, m_defaultTowerMaterial);
	}

	if (m_ringPulse != nullptr)
		m_ringPulse->SetVisibility(false);

	TurnOnLight(pointLight1, false);
	TurnOnLight(pointLight2, false);
	m_isLightOnVoidout = true; //Todas las luces encendidas, inluso las que tenian efecto voidout,por si se quedaron apagadas al derrotar a todos los enemigos del interior
	m_timeCounter = 0;
	m_isLightOnPermanent = false; //La luz se queda apagada permanentemente.
	LightOffBehavior(); //Apagar tambien el daño al player.
}

void AACTR_SwitchableLight::IsLightOffBehavior()
{
	m_stunedEnemies.Empty();

	for (AActor* e : m_enemiesInLightZone)
	{
		if (e == nullptr)
			continue;

		AAIController* aiController = UAIBlueprintHelperLibrary::GetAIController(e);
		if (aiController != nullptr)
		{
			aiController->StopMovement();
			aiController->GetBrainComponent()->Deactivate();
			aiController->GetBrainComponent()->GetBlackboardComponent()->SetValueAsBool("AbleToShoot", false);
		}
		if (aiController == nullptr || aiController->GetBrainComponent()->IsActive())
			continue;

		FRotator initialRotation = e->GetActorRotation();
		FRotator rotation = UKismetMathLibrary::FindLookAtRotation(e->K2_GetActorLocation(), this->GetActorLocation());
		FVector forward = UKismetMathLibrary::GetForwardVector(rotation);
		e->SetActorRotation(rotation);
		UCharacterMovementComponent* enemyMovement = Cast<UCharacterMovementComponent>(e->GetComponentByClass(UCharacterMovementComponent::StaticClass()));
		if (enemyMovement == nullptr)
			return;
		enemyMovement->Velocity = FVector(-forward.X * m_impulseForce, -forward.Y * m_impulseForce, m_jumpForce);
		e->SetActorRotation(initialRotation);
		m_stunedEnemies.Add(e);
	}
}

void AACTR_SwitchableLight::TurnOnLight(UPointLightComponent* pointLight, bool on)
{
	if (pointLight != nullptr)
		pointLight->SetVisibility(on);
	else
		USYST_LogManager::PrintString(this, "AACTR_SwitchableLight.TurnOnLight: Tried to turn on/off light nullptr");
}

void AACTR_SwitchableLight::SetActionIndicatorShow(bool show)
{
	if (this != nullptr && m_voidoutIndicator != nullptr)
		SetWidgetShow(m_voidoutIndicator->GetUserWidgetObject(), show);
}

void AACTR_SwitchableLight::SetWidgetShow(UUserWidget* userWidget, bool show)
{
	if (userWidget == nullptr)
		return;

	userWidget->SetVisibility(show ? ESlateVisibility::Visible : ESlateVisibility::Hidden);
}
