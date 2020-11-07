
#include "TGPT_Explorer.h"
#include "Assets/ACTR_EnergyTorche.h"
#include "Assets/ACTR_EnergyTower.h"
#include "Assets/ACTR_Light.h"
#include "System/SYST_LogManager.h"

ATGPT_Explorer::ATGPT_Explorer()
{
	Root = CreateDefaultSubobject<USceneComponent>(FName("Root"));
}

AACTR_Light* ATGPT_Explorer::GetTorcheOrTower()
{
	if (m_energyTorche != nullptr)
		return m_energyTorche;
	if(m_energyTower != nullptr)
		return m_energyTower;
	
	USYST_LogManager::PrintError(this, "ATGPT_Explorer.GetTorcheOrTower: both are nullptr");
	return nullptr;
}