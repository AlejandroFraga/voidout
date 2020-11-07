// Fill out your copyright notice in the Description page of Project Settings.


#include "SYST_SoundEventManager.h"

// Sets default values
ASYST_SoundEventManager::ASYST_SoundEventManager()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void ASYST_SoundEventManager::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ASYST_SoundEventManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}
/*
void ASYST_SoundEventManager::playPlayerAttack(FVector position) {

}

void ASYST_SoundEventManager::playPlayerDeath(FVector position) {

}

void ASYST_SoundEventManager::playPlayerShadowDash(FVector position) {

}

void ASYST_SoundEventManager::playPlayerSteps(FVector position) {

}

void ASYST_SoundEventManager::playPlayerVoidOut(FVector position) {

}
*/