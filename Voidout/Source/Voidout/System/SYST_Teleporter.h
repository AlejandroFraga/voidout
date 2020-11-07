// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include <string>
#include <unordered_map>

/**
 * 
 */

struct Loader
{	
	FString m_map{};
	std::pair<float, float> m_coords{};
};

class VOIDOUT_API SYST_Teleporter
{
	// Idx to check
	int m_idx{ 0 };
	
public:
	// Ctor.
	SYST_Teleporter();
	
	// Dtor.
	~SYST_Teleporter();

	// teleport.
	void Teleport() noexcept;
};
