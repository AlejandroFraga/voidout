// Fill out your copyright notice in the Description page of Project Settings.


#include "SYST_MinionToSoldier.h"
#include "AIController/BLKB_Global.h"
#include "Character/CHRT_Player.h"
#include "Character/CHRT_Enemy.h"
#include "System/SYST_LogManager.h"

#include "Blueprint/AIBlueprintHelperLibrary.h"
#include "ConstructorHelpers.h"
#include "Components/BoxComponent.h"
#include "Components/ACMP_LightStructureSubscription.h"


// D:\workspace\grupo1\Engine\Windows\Engine\Source\Runtime\Engine\Classes\GameFramework\Actor.h

// Sets default values
ASYST_MinionToSoldier::ASYST_MinionToSoldier()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
	// Set trigger
	m_overlapTrg = CreateDefaultSubobject<UBoxComponent>(TEXT("Trigger"));
	RootComponent = m_overlapTrg;

	// Set classes.
	static ConstructorHelpers::FClassFinder<ACHRT_Enemy> minionBPClass(TEXT("/Game/Blueprints/Character/CHRT_Minion_BP"));
	m_minion_bpClass = minionBPClass.Class;

	static ConstructorHelpers::FClassFinder<ACHRT_Enemy> soldierBPClass(TEXT("/Game/Blueprints/Character/CHRT_LightTrooper_BP"));
	m_Soldier_bpClass = soldierBPClass.Class;
}

// Called when the game starts or when spawned
void ASYST_MinionToSoldier::BeginPlay()
{
	Super::BeginPlay();
	m_overlapTrg->OnComponentEndOverlap.AddDynamic(this, &ASYST_MinionToSoldier::EndOverlap);
}


void ASYST_MinionToSoldier::EndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (OtherActor == nullptr || Cast<ACHRT_Player>(OtherActor) == nullptr || OverlappedComponent == nullptr)
		return;

	for(auto m : m_minionArray)
	{
		if (!m) continue; // Ckeck nullptr
		if (m->GetClass() != m_minion_bpClass) continue; // If's not a minion, continue.
		auto* p = UAIBlueprintHelperLibrary::SpawnAIFromClass(m->GetWorld(), m_Soldier_bpClass, nullptr, m->GetActorLocation());
		p->Tags.AddUnique(TEXT("save"));
		auto* triggerCmp = Cast< UACMP_LightStructureSubscription>(Cast<ACHRT_Enemy>(p)->m_parent->GetComponentByClass(UACMP_LightStructureSubscription::StaticClass()));
		if(triggerCmp) triggerCmp->UnsuscribeEnemyFromTower(m);
		m->Destroy();
	}

	// Erases all items of vector.
	m_minionArray.Empty();

	Destroy();
}