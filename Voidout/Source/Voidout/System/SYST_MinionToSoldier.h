// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SYST_MinionToSoldier.generated.h"

UCLASS()
class VOIDOUT_API ASYST_MinionToSoldier : public AActor
{
	GENERATED_BODY()

	// Minion class
	TSubclassOf<class ACHRT_Enemy> m_minion_bpClass;

	// Soldier class
	TSubclassOf<ACHRT_Enemy> m_Soldier_bpClass;

	// Overlap trigger.
	class UBoxComponent* m_overlapTrg{ nullptr };
	
public:	
	// Sets default values for this actor's properties
	ASYST_MinionToSoldier();

	// Called when the game starts or when spawned
	void BeginPlay() override;

	// Array of minions
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Swapper") TArray<AActor*> m_minionArray;

	// Overlap
	// UFUNCTION()
	// void BeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	// Overlap
	UFUNCTION()
	void EndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);
};
