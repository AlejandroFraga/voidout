#include "SYST_SaveGame.h"
#include "Engine/World.h"
#include "GameFramework/Character.h"
#include "Kismet/GameplayStatics.h"
#include "Blueprint/AIBlueprintHelperLibrary.h"
#include "Containers/Array.h"
#include "character/CHRT_Explorer.h"
#include "Components/ACMP_Health.h"
#include "System/SYST_LogManager.h"
#include "AIController/BLKB_Global.h"
#include "Character/CHRT_Commander.h"
#include "Assets/ACTR_EnergyBarrier.h"
#include "TimerManager.h"


//Fills the FActorRecord Struct With the data form "a" AActor and "a" AActor components (scene, primitive and movement) if exists
void CreateActorRecord(AActor* a, FActorRecord& rec) {

	// USYST_LogManager::PrintString(a, "Saving Actor: " + FString::Printf(*a->GetName()), false);
	
	rec.m_class = a->GetClass();
	rec.m_name = a->GetFName();
	rec.m_transform = a->GetTransform();
	rec.m_lifespan = a->GetLifeSpan();
}

//spawn of Actor in the load process
void LoadActor(UWorld* world, FActorRecord& rec, bool isPlayer)
{
	FActorSpawnParameters SpawnParams;
	SpawnParams.Name = rec.m_name;
	SpawnParams.bDeferConstruction = true;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	if (!isPlayer)
		return;
	
	if (auto* a = world->SpawnActor<AActor>(rec.m_class, rec.m_transform, SpawnParams))
	{
		a->SetLifeSpan(rec.m_lifespan);
		rec.m_spawnedActor = a;
	}
}

void FinishLoadActor(FActorRecord& rec)
{
	if (AActor* a = rec.m_spawnedActor)
	{
		a->FinishSpawning(rec.m_transform);
	}
}

//The TArray of other Actors is obtained in Character blueprint event graph calling GetAllactorswithtag "Save"
//Creates an ActorRecord for Player and for tagged "Save" actors and fill the struct with data to save
void USYST_SaveGame::SaveGame(APawn* PlayerActor)
{
	auto* world = PlayerActor->GetWorld(); // Get the world.
	TArray<AActor*> actorsToSave;
	UGameplayStatics::GetAllActorsWithTag(world, FName("save"), actorsToSave); // Get all save actors.

	const auto size{ actorsToSave.Num() };
	
	m_actors.Reserve(size); // Reserve data.
	m_transforms.Reserve(size); // Reserve data.
	m_shoots.Reserve(size); // Reserve data.
	
	

	CreateActorRecord(PlayerActor, m_player);
	// Correct the player Z to avoid spawning below the level
	m_player.m_transform.SetLocation(m_player.m_transform.GetLocation() * FVector(1.f, 1.f, 0.f));
	if (PlayerActor == nullptr || PlayerActor->GetController() == nullptr) return;
	m_playerControllerRotator = PlayerActor->GetController()->GetControlRotation();

	auto Insert = [](int idx, auto& hash, auto& act)
	{
		bool has = hash.Contains(idx);

		for (auto* p : act->m_path) // For each path point.
		{
			if (has)
			{
				hash[idx].Emplace(p);
			}
			else
			{
				has = true;
				auto& me = hash.Add(idx, {});
				me.Emplace(p);
			}
		}
	};
	// Save other actors.
	for (auto* ac : actorsToSave)
	{
		
		auto* e = Cast<ACHRT_Enemy>(ac);
		
		if(e && e->m_parent && Cast<ACHRT_Commander>(e->m_parent)) // If we are a minion controlled by commander.
			continue;
		
		auto* hp{ Cast<UACMP_Health>(ac->GetComponentByClass(UACMP_Health::StaticClass())) }; // Save the hp.
		if (hp && hp->m_dying) continue; // If has a HP_cmp, and its dead...

		const auto idx = m_actors.Emplace(ac);
		m_transforms.Emplace(ac->GetTransform());

		if (e) m_shoots.Emplace(e->m_dontShootThr, e->m_successShootThrOriginal, e->m_successShootThr, e->m_successFlaredThr);	
		
		if (auto* ex = Cast<ACHRT_Explorer>(ac)) // If is an explorer.
		{
			Insert(idx, m_explorers, ex);
		}
		else if (auto* comm = Cast<ACHRT_Commander>(ac)) // If is a commander.
		{
			Insert(idx, m_commanders, comm);
			if (m_commanders.Contains(idx)) {
				m_commanders[idx].m_nMinons = comm->m_nChildren;
				// m_commanders[idx].m_location = comm->GetActorLocation();
				if (e->m_energyBarrierAsigned != nullptr) {
					m_commanders[idx].m_asignedEnergyBarrier = e->m_energyBarrierAsigned;
				}
			}
		}
		
	}
}

//Cleans before attempting to load, then loads Player + other Actors from the saved data.
void USYST_SaveGame::LoadGame(UObject* WorldContextObject)
{
	auto* world = WorldContextObject->GetWorld(); // Get World.
	TArray<AActor*> aliveActors;
	UGameplayStatics::GetAllActorsWithTag(world, FName("save"), aliveActors); // Get all save actors.

	UGameplayStatics::GetPlayerCharacter(world, 0)->Destroy(); // Destroy main character.
	LoadActor(world, m_player, true);
	FinishLoadActor(m_player);
	APlayerController* ctrl = UGameplayStatics::GetPlayerController(world, 0);
	const float DeltaTime = UGameplayStatics::GetWorldDeltaSeconds(world);

	if (ctrl != nullptr)
	{
		ctrl->Possess(Cast<APawn>(m_player.m_spawnedActor));
		ctrl->EnableInput(ctrl);
		ctrl->SetControlRotation(m_playerControllerRotator);
		ctrl->UpdateRotation(DeltaTime);
	}

	auto SetPath = [&](auto hash, auto act, int idx)
	{
		auto* val = hash.Find(idx);
		if (!val) return;
		
		auto& newPath = val->m_actors;
		int n = newPath.Num();

		act->m_path.Reserve(n);
		for (auto* p : newPath)
		{
			act->m_path.Emplace(p);
		}
	};
	
	const auto size = m_actors.Num();
	if (size == 0) return;

	

	for (auto i = 0; i < size; ++i)
	{
		auto* ac = m_actors[i];
		if (!ac) continue;
		auto idx = aliveActors.Find(ac);

		auto* comm = Cast<ACHRT_Commander>(ac);
		if(comm && idx != -1) // If is a commander and he's alive.
		{
			

			if (comm->m_energyBarrierAsigned)
			{
				commanderWall = comm->m_energyBarrierAsigned;
				commanderWall->SubscribeToWall();
			}
			
			for (auto c : comm->m_children)
				if(c)	//If the children is alive
					c->Destroy();
			comm->m_children.Empty();
			comm->Destroy();
			idx = -1; // Set to dead.
		}
		
		if (idx != -1) continue; // if != -1 -> actor alive

		auto* acClass = ac->GetClass();

		const auto& acLoc = comm ? comm->m_spawnPos : m_transforms[i].GetLocation(); // If is a commander, set to spawn position.
		const auto& acRot = m_transforms[i].Rotator();


		auto* spawned = UAIBlueprintHelperLibrary::SpawnAIFromClass(world, acClass, nullptr, acLoc, acRot);

		auto* e = Cast<ACHRT_Enemy>(spawned);
		e->m_dontShootThr = m_shoots[i].m_dontShootThr;
		e->m_successShootThrOriginal = m_shoots[i].m_successShootThrOriginal;
		e->m_successShootThr = m_shoots[i].m_successShootThr;
		e->m_successFlaredThr = m_shoots[i].m_successFlaredThr;
		
		
		if (auto* ex = Cast<ACHRT_Explorer>(spawned)) // If is an explorer.
		{
			SetPath(m_explorers, ex, i);
		}
		else if (auto* nComm = Cast<ACHRT_Commander>(spawned)) // If is a commander.
		{

			SetPath(m_commanders, nComm, i);
			if (m_commanders.Contains(i))
			{
				nComm->m_nChildren = m_commanders[i].m_nMinons;
				nComm->UpdateChildren();

				e->m_energyBarrierAsigned = m_commanders[i].m_asignedEnergyBarrier;

			}
			
			if (m_commanders[i].m_asignedEnergyBarrier)
			{
				FTimerHandle timerHandler;
				world->GetTimerManager().SetTimer(timerHandler, this, &USYST_SaveGame::UnSubscribe, 0.3f, false);
			}

		}

		
	}

	m_actors.Empty(); // Empty, because we already loaded.
	m_transforms.Empty();
}

void USYST_SaveGame::UnSubscribe() {
	commanderWall->UnsubscribeToWall();
}

