// Fill out your copyright notice in the Description page of Project Settings.


#include "SYST_Teleporter.h"
#include "SYST_MapLoader.h"
#include "Engine/Engine.h"
#include "SYST_LogManager.h"
#include "Kismet/GameplayStatics.h"
#include "AIController/BLKB_Global.h"
#include "GameFramework/Actor.h"

// Teleporter zones.
// This should go to data, but we don't have time.
// Placeholder !!!!
static std::vector<Loader> s_Loaders =
{
	{"Level1_Zone1", {-33.f, -84.f}},
	{"Level1_Zone3", {564.919f, 10239.441f}},
	{"Level1_Zone6", {-5014.f, 22425.f}},
	{"Level2_Zone1", {-4327.f, 25647.f}},
	{"Level2_Zone2", {-3752.f, 32595.f}},
	{"Level3_Zone3", {-7405.f, 42146.f}},
};

SYST_Teleporter::SYST_Teleporter()
{ }

SYST_Teleporter::~SYST_Teleporter()
{ }

// Teleport.
void SYST_Teleporter::Teleport() noexcept
{
	if(m_idx >= s_Loaders.size())
	{
		m_idx = 0;
	}
	
	auto* plyAc = BLKB_Global::getPlayerActor();
	if (!plyAc) return;

	auto& loader = s_Loaders[++m_idx];
	auto* playerWorld = plyAc->GetWorld();
	FName mapName(*loader.m_map);
	
	const FLatentActionInfo latInfo;
	UGameplayStatics::LoadStreamLevel(playerWorld, mapName, true, false, latInfo);

	FVector newLoc(loader.m_coords.first, loader.m_coords.second, plyAc->GetActorLocation().Z);

	plyAc->SetActorLocation(newLoc);
}
