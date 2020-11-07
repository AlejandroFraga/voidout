
#include "CHRT_Player.h"
#include "Components/ACMP_FadeBlocking.h"
#include "Components/ACMP_Health.h"
#include "Controller/CTRL_Player.h"
#include "GameInstance/GMIN_Base.h"
#include "System/SYST_LogManager.h"
#include "System/SYST_SaveGame.h"
#include "System/SYST_SystemsManager.h"

#include "Animation/AnimInstance.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Engine/World.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Misc/OutputDeviceNull.h"
#include "Particles/ParticleSystemComponent.h"
#include "TimerManager.h"


ACHRT_Player::ACHRT_Player()
{
	if (GetCapsuleComponent() == nullptr)
		return;

	// Set size for player capsule
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);

	// Don't rotate character to camera direction
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	if (GetCharacterMovement() == nullptr)
		return;

	// Configure character movement
	GetCharacterMovement()->bOrientRotationToMovement = true; // Rotate character to moving direction
	GetCharacterMovement()->RotationRate = FRotator(0.f, 640.f, 0.f);
	GetCharacterMovement()->bConstrainToPlane = true;
	GetCharacterMovement()->bSnapToPlaneAtStart = true;

	// Create a camera boom...
	MainSpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	if (MainSpringArm == nullptr)
		return;

	MainSpringArm->SetupAttachment(RootComponent);
	MainSpringArm->SetUsingAbsoluteRotation(true); // Don't want arm to rotate when character does
	MainSpringArm->TargetArmLength = cameraDistance;
	MainSpringArm->SetRelativeRotation(FRotator(cameraRotatorX, 45.f, 0.f));
	MainSpringArm->bDoCollisionTest = false; // Don't want to pull camera in when it collides with level

	// Create a camera...
	MainCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("TopDownCamera"));
	if (MainCamera == nullptr)
		return;

	MainCamera->SetupAttachment(MainSpringArm, USpringArmComponent::SocketName);
	MainCamera->FieldOfView = cameraFOV;
	MainCamera->bAutoActivate = true;
	MainCamera->bUsePawnControlRotation = false; // Camera does not rotate relative to arm
	MainCamera->SetConstraintAspectRatio(true);
	MainCamera->AspectRatio = 16.f / 9.f;

	m_health = FindComponentByClass<UACMP_Health>();

	// Activate ticking in order to update the cursor every frame.
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = true;
}

void ACHRT_Player::BeginPlay()
{
	Super::BeginPlay();

	darkSlashPSC = UGameplayStatics::SpawnEmitterAttached(darkSlashPS, GetRootComponent(), "", FVector::ZeroVector, FRotator{ 0.f, 180.f, 0.f }, FVector{ 0.8f, -0.6f, 0.8f }, EAttachLocation::KeepRelativeOffset, false, EPSCPoolMethod::None, false);

	shadowDashPSC = UGameplayStatics::SpawnEmitterAttached(shadowDashPS, GetRootComponent(), "", FVector{ -50.f, 0.f, 0.f }, FRotator{ 0.f, 270.f, 0.f }, FVector{ 1.f, 1.f, 1.f }, EAttachLocation::KeepRelativeOffset, false, EPSCPoolMethod::None, false);

	shadowDashTrailPSC = UGameplayStatics::SpawnEmitterAttached(shadowDashTrailPS, GetRootComponent(), "", FVector{ -50.f, 0.f, 0.f }, FRotator{ 0.f, 270.f, 0.f }, FVector{ 1.f, 1.f, 1.f }, EAttachLocation::KeepRelativeOffset, false, EPSCPoolMethod::None, false);
}

ACTRL_Player* ACHRT_Player::GetCTRLPlayer() const
{
	return Cast<ACTRL_Player>(GetController());
}

UGMIN_Base* ACHRT_Player::GetGMINBase() const
{
	return Cast<UGMIN_Base>(UGameplayStatics::GetGameInstance(this));
}

void ACHRT_Player::DelayDamage(float damage)
{
	if (!IsDying() && !m_health->m_isReceivingDamage)
	{
		m_health->m_isReceivingDamage = true;
		FTimerHandle timerHandler;
		GetWorld()->GetTimerManager().SetTimer(timerHandler, this, &ACHRT_Player::TakeDelayedDamage, m_timeWaitingDamage, false);
		m_delayedDamage = damage;
	}
}

void ACHRT_Player::TakeDelayedDamage()
{
	if (IsDying())
		return;

	m_health->m_isReceivingDamage = false;
	TakeDamage(m_delayedDamage);
	m_delayedDamage = 0.f;

	//I call the hit animation (in the animation blueprint).
	UAnimInstance* animInstance = GetMesh() != nullptr ? GetMesh()->AnimScriptInstance : nullptr;
	if (animInstance != nullptr)
	{
		FOutputDeviceNull ar; //Needed to call a blueprint function
		animInstance->CallFunctionByNameWithArguments(TEXT("HitAnimationActivate"), ar, nullptr, true);
	}

	SYST_SystemsManager::GetSoundEventManager()->playPlayerDamage(GetActorLocation());
}

int ACHRT_Player::TakeDamage(float damage)
{
	if (IsDying())
		return 0;

	return Super::TakeDamage(damage);
}

void ACHRT_Player::Death()
{
	if (GetCTRLPlayer() != nullptr)
		//DisablePlayerMovement
		GetCTRLPlayer()->DisableInput(GetCTRLPlayer());

	SYST_SystemsManager::GetSoundEventManager()->playPlayerDeath(GetActorLocation());

	UACMP_FadeBlocking* fadeBlocking = FindComponentByClass<UACMP_FadeBlocking>();
	if (fadeBlocking != nullptr)
		fadeBlocking->SetEnable(false);

	UAnimInstance* animInstance = GetMesh() != nullptr ? GetMesh()->AnimScriptInstance : nullptr;
	if (animInstance != nullptr)
	{
		//I call the dead animation (in the animation blueprint)
		FOutputDeviceNull ar;
		animInstance->CallFunctionByNameWithArguments(TEXT("DeadAnimation"), ar, nullptr, true);
	}

	Super::Death();
}

void ACHRT_Player::SaveGame()
{
	USaveGame* saveGameAux = UGameplayStatics::CreateSaveGameObject(USYST_SaveGame::StaticClass());
	if (saveGameAux == nullptr)
		return;

	USYST_SaveGame* saveGame = Cast<USYST_SaveGame>(saveGameAux);
	if (saveGame == nullptr)
		return;

	saveGame->SaveGame(this);

	UGameplayStatics::SaveGameToSlot(saveGame, "Save1", 0);
}

void ACHRT_Player::LoadGame()
{
	UACMP_FadeBlocking* fadeBlocking = FindComponentByClass<UACMP_FadeBlocking>();
	if (fadeBlocking != nullptr)
	{
		fadeBlocking->CleanFadeObjects();
		fadeBlocking->SetEnable(true);
	}

	GetCTRLPlayer()->ResetState();

	USYST_SaveGame* saveGame = Cast<USYST_SaveGame>(UGameplayStatics::LoadGameFromSlot("Save1", 0));
	if (saveGame != nullptr)
		saveGame->LoadGame(this);

	ResetDamageScreen();

	if(GetGMINBase() != nullptr)
		GetGMINBase()->UpdateSystemManager();
}

void ACHRT_Player::PlayDarkSlashPS()
{
	if (darkSlashPSC != nullptr)
		darkSlashPSC->Activate();
	else
		USYST_LogManager::PrintError(this, "ACHRT_Player.PlayDarkSlashPS: No darkSlashPSC");
}

void ACHRT_Player::PlayDarkSlashDamagePS()
{
	if (darkSlashDamagePS != nullptr)
		UGameplayStatics::SpawnEmitterAttached(darkSlashDamagePS, GetRootComponent(), "", FVector{ 50.f, 0.f, 25.f }, FRotator{ 0.f, 135.f, 0.f }, FVector{ 1.f, 1.f, 1.f });
	else
		USYST_LogManager::PrintError(this, "ACHRT_Player.PlayDarkSlashDamagePS: No darkSlashDamagePS");
}

void ACHRT_Player::StartShadowDashPS()
{
	if (shadowDashMaterial != nullptr)
		GetMesh()->SetMaterial(0, shadowDashMaterial);
	else
		USYST_LogManager::PrintError(this, "ACHRT_Player.StartShadowDashPS: No shadowDashMaterial");

	if (shadowDashPSC != nullptr)
		shadowDashPSC->Activate();
	else
		USYST_LogManager::PrintError(this, "ACHRT_Player.StartShadowDashPS: No shadowDashPSC");

	if (shadowDashTrailPSC != nullptr)
		shadowDashTrailPSC->Activate();
	else
		USYST_LogManager::PrintError(this, "ACHRT_Player.StartShadowDashPS: No shadowDashTrailPSC");
}

void ACHRT_Player::StopShadowDashPS()
{
	if (normalMaterial != nullptr)
		GetMesh()->SetMaterial(0, normalMaterial);
	else
		USYST_LogManager::PrintError(this, "ACHRT_Player.StartShadowDashPS: No normalMaterial");

	if (shadowDashPSC != nullptr)
		shadowDashPSC->Deactivate();
	else
		USYST_LogManager::PrintError(this, "ACHRT_Player.StopShadowDashPS: No shadowDashPSC");

	if (shadowDashTrailPSC != nullptr)
		shadowDashTrailPSC->Deactivate();
	else
		USYST_LogManager::PrintError(this, "ACHRT_Player.StopShadowDashPS: No shadowDashTrailPSC");
}

void ACHRT_Player::PlayVoidoutPS(FVector location)
{
	if (voidoutMaterial != nullptr)
		GetMesh()->SetMaterial(0, voidoutMaterial);
	else
		USYST_LogManager::PrintError(this, "ACHRT_Player.PlayVoidoutPS: No voidoutMaterial");

	if (voidoutPS != nullptr)
		UGameplayStatics::SpawnEmitterAtLocation(this, voidoutPS, location);
	else
		USYST_LogManager::PrintError(this, "ACHRT_Player.PlayVoidoutPS: No voidoutPS");
}

void ACHRT_Player::StopVoidoutPS()
{
	if (normalMaterial != nullptr)
		GetMesh()->SetMaterial(0, normalMaterial);
	else
		USYST_LogManager::PrintError(this, "ACHRT_Player.StopVoidoutPS: No normalMaterial");
}