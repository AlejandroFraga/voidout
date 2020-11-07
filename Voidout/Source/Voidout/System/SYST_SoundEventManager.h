// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SYST_SoundEventManager.generated.h"

UCLASS()
class VOIDOUT_API ASYST_SoundEventManager : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ASYST_SoundEventManager();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	/********************************************************************************************/
	/********************************************************************************************/
	/**********************			GENERAL SOUNDS SECTION					*********************/
	/********************************************************************************************/
	/********************************************************************************************/

	//Plays the Fight01 BSO
	UFUNCTION(BlueprintImplementableEvent, Category = "Fight Sound Event")
		void playFight01(FVector position);

	//Plays the Fight02 BSO
	UFUNCTION(BlueprintImplementableEvent, Category = "Fight Sound Event")
		void playFight02(FVector position);

	//Plays the Fight03 BSO
	UFUNCTION(BlueprintImplementableEvent, Category = "Fight Sound Event")
		void playFight03(FVector position);

	//Plays the Ambient01 BSO
	UFUNCTION(BlueprintImplementableEvent, Category = "Ambient Sound Event")
		void playAmbient01(FVector position);

	//Plays the Ambient General BSO
	UFUNCTION(BlueprintImplementableEvent, Category = "Ambient Sound Event")
		void playGeneralAmbient();

	//Plays the Ambient01 BSO
	UFUNCTION(BlueprintImplementableEvent, Category = "Ambient Sound Event")
		void playAmbient02(FVector position);

	//Plays the Ambient01 BSO
	UFUNCTION(BlueprintImplementableEvent, Category = "Ambient Sound Event")
		void playAmbient03(FVector position);

	//Stops the Fight Music
	UFUNCTION(BlueprintImplementableEvent, Category = "Fight Sound Event")
		void stopFightMusic();

	//Pause Music Bus
	UFUNCTION(BlueprintImplementableEvent, Category = "Fight Sound Event")
		void pauseMusicBus();

	//Play Music Bus
	UFUNCTION(BlueprintImplementableEvent, Category = "Fight Sound Event")
		void playMusicBus();


	/********************************************************************************************/
	/********************************************************************************************/
	/**********************				MENU SOUNDS SECTION					*********************/
	/********************************************************************************************/
	/********************************************************************************************/

	//Plays the start button sound
	UFUNCTION(BlueprintImplementableEvent, Category = "Menu Sound Event")
		void playMenuStartButton();

	//Plays the movement menu sound
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "Menu Sound Event")
		void playMenuMovement();

	//Plays the movement menu sound
	UFUNCTION(BlueprintImplementableEvent, Category = "Menu Sound Event")
		void playMenuSelection();

	//Plays when back on menu
	UFUNCTION(BlueprintImplementableEvent, Category = "Menu Sound Event")
		void playMenuBack();

	//Plays when back on menu
	UFUNCTION(BlueprintImplementableEvent, Category = "Menu Sound Event")
		void stopIntro();


	/********************************************************************************************/
	/********************************************************************************************/
	/**********************				PLAYER SOUNDS SECTION				*********************/
	/********************************************************************************************/
	/********************************************************************************************/

	//Function that plays sound when player use normal attack.
	UFUNCTION(BlueprintImplementableEvent, Category = "Player Sound Event")
		void playPlayerDarkSlash(FVector position);

	//Function that plays sound when player dies.
	UFUNCTION(BlueprintImplementableEvent, Category = "Player Sound Event")
		void playPlayerDeath(FVector position);

	//Function that plays sound when player use Shadow Dash.
	UFUNCTION(BlueprintImplementableEvent, Category = "Player Sound Event")
		void playPlayerShadowDash(FVector position);

	//Function that plays sound when player walks.
	UFUNCTION(BlueprintImplementableEvent, Category = "Player Sound Event")
		void playPlayerSteps(FVector position);

	//Function that plays sound when player use Void Out.
	UFUNCTION(BlueprintImplementableEvent, Category = "Player Sound Event")
		void playPlayerVoidout(FVector position);

	//Function that plays sound when player is damaged.
	UFUNCTION(BlueprintImplementableEvent, Category = "Player Sound Event")
		void playPlayerDamage(FVector position);

	//Function that plays sound when shadow dash is charged
	UFUNCTION(BlueprintImplementableEvent, Category = "Player Sound Event")
		void playAbilitySDCharged();
		
			//Function that plays sound when shadow dash is charged
	UFUNCTION(BlueprintImplementableEvent, Category = "Player Sound Event")
		void playUnreadySkill();

	/********************************************************************************************/
	/********************************************************************************************/
	/**********************			LIGHT TROOPER SOUNDS SECTION			*********************/
	/********************************************************************************************/
	/********************************************************************************************/

	//Plays the sound produced by a Light Trooper shoot
	UFUNCTION(BlueprintImplementableEvent, Category = "Light Trooper Sound Event")
		void playLightTrooperShoot(FVector position);

	//Plays the sound produced by a Light Trooper when dies
	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent, Category = "Light Trooper Sound Event")
		void playLightTrooperDeath(FVector position);

	//Plays the sound produced by a Light Trooper when moves
	UFUNCTION(BlueprintImplementableEvent, Category = "Light Trooper Sound Event")
		void playLightTrooperSteps(FVector position);

	/********************************************************************************************/
	/********************************************************************************************/
	/**********************			SHADOW EXPLORER SOUNDS SECTION			*********************/
	/********************************************************************************************/
	/********************************************************************************************/

	//Plays the sound produced by a Shadow Explorer shoot
	UFUNCTION(BlueprintImplementableEvent, Category = "Shadow Explorer Sound Event")
		void playShadowExplorerShoot(FVector position);

	//Plays the sound produced by a Shadow Explorer when dies
	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent, Category = "Shadow Explorer Sound Event")
		void playShadowExplorerDeath(FVector position);

	//Plays the sound produced by a Shadow Explorer when moves
	UFUNCTION(BlueprintImplementableEvent, Category = "Shadow Explorer Sound Event")
		void playShadowExplorerSteps(FVector position);

	/********************************************************************************************/
	/********************************************************************************************/
	/**********************			COMMANDER SOUNDS SECTION				*********************/
	/********************************************************************************************/
	/********************************************************************************************/

	//Plays the sound produced by a Commander shoot
	UFUNCTION(BlueprintImplementableEvent, Category = "Commander Sound Event")
		void playCommanderDamage(FVector position);

	//Plays the sound produced by a Shadow Explorer when dies
	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent, Category = "Commander Sound Event")
		void playCommanderDeath(FVector position);

	//Plays the sound produced by a Shadow Explorer when moves
	UFUNCTION(BlueprintImplementableEvent, Category = "Commander Sound Event")
		void playCommanderSteps(FVector position);

	/********************************************************************************************/
	/********************************************************************************************/
	/**********************			ENERGY TOWER SOUNDS SECTION				*********************/
	/********************************************************************************************/
	/********************************************************************************************/

	//Plays the sound produced when tower damage player
	UFUNCTION(BlueprintImplementableEvent, Category = "Light Tower Sound Event")
		void playEnergyTowerDamage(FVector position);

	//Stops the sound produced when tower damage player
	UFUNCTION(BlueprintImplementableEvent, Category = "Light Tower Sound Event")
		void stopEnergyTowerDamage(FVector position);

	//Plays the sound produced when energy tower is destructed
	UFUNCTION(BlueprintImplementableEvent, Category = "Light Tower Sound Event")
		void playEnergyTowerDestruction(FVector position);

	//Plays the sound produced by a Light Tower
	UFUNCTION(BlueprintImplementableEvent, Category = "Light Tower Sound Event")
		void playEnergyTowerAmbient(FVector position);

	//Plays the sound produced by a Light Tower when appears
	UFUNCTION(BlueprintImplementableEvent, Category = "Light Tower Sound Event")
		void playEnergyTowerCreation(FVector position);

	/********************************************************************************************/
	/********************************************************************************************/
	/**********************			ENERGY TORCH SOUNDS SECTION				*********************/
	/********************************************************************************************/
	/********************************************************************************************/

	//Plays the sound produced when Torch damage player
	UFUNCTION(BlueprintImplementableEvent, Category = "Light Torch Sound Event")
		void playEnergyTorchDamage(FVector position);

	//Plays the sound produced when light Torch is destructed
	UFUNCTION(BlueprintImplementableEvent, Category = "Light Torch Sound Event")
		void playEnergyTorchDestruction(FVector position);

	//Plays the sound produced by a Light Torch
	UFUNCTION(BlueprintImplementableEvent, Category = "Light Torch Sound Event")
		void playEnergyTorchAmbient(FVector position);

	//Plays the sound produced by a Light Torch when is created
	UFUNCTION(BlueprintImplementableEvent, Category = "Light Torch Sound Event")
		void playEnergyTorchCreated(FVector position);

	/********************************************************************************************/
	/********************************************************************************************/
	/**********************			POWER GENERATOR SOUNDS SECTION			*********************/
	/********************************************************************************************/
	/********************************************************************************************/

	//Plays the sound produced when Power Generator damage player
	UFUNCTION(BlueprintImplementableEvent, Category = "Power Generator Sound Event")
		void playPowerGeneratorDamage(FVector position);

	//Plays the sound produced when Power Generator is destructed
	UFUNCTION(BlueprintImplementableEvent, Category = "Power Generator Sound Event")
		void playPowerGeneratorDestruction(FVector position);

	//Plays the sound produced by a Power Generator
	UFUNCTION(BlueprintImplementableEvent, Category = "Power Generator Sound Event")
		void playPowerGeneratorAmbient(FVector position);

	//Plays the sound produced by a Power Generator when is created
	UFUNCTION(BlueprintImplementableEvent, Category = "Power Generator Sound Event")
		void playPowerGeneratorCreated(FVector position);

	/********************************************************************************************/
	/********************************************************************************************/
	/**********************			ENERGY CORE SOUNDS SECTION				*********************/
	/********************************************************************************************/
	/********************************************************************************************/

	//Plays the sound produced when Energy Core damage player
	UFUNCTION(BlueprintImplementableEvent, Category = "Energy Core Sound Event")
		void playEnergyCoreDamage(FVector position);

	//Plays the sound produced when Energy Core is destructed
	UFUNCTION(BlueprintImplementableEvent, Category = "Energy Core Sound Event")
		void playEnergyCoreDestruction(FVector position);

	//Plays the sound produced by a Energy Core
	UFUNCTION(BlueprintImplementableEvent, Category = "Energy Core Sound Event")
		void playEnergyCoreAmbient(FVector position);

	//Plays the sound produced by a Energy Core when is created
	UFUNCTION(BlueprintImplementableEvent, Category = "Energy Core Sound Event")
		void playEnergyCoreCreated(FVector position);

	/********************************************************************************************/
	/********************************************************************************************/
	/**********************			ENERGY BARRIER SOUNDS SECTION			*********************/
	/********************************************************************************************/
	/********************************************************************************************/

	//Plays the sound produced when Energy Barrier damage player
	UFUNCTION(BlueprintImplementableEvent, Category = "Energy Barrier Sound Event")
		void playEnergyBarrierDamage(FVector position);

	//Plays the sound produced when Energy Core is destructed
	UFUNCTION(BlueprintImplementableEvent, Category = "Energy Barrier Sound Event")
		void playEnergyBarrierOff(FVector position);

	//Plays the sound produced by a Energy Core
	UFUNCTION(BlueprintImplementableEvent, Category = "Energy Barrier Sound Event")
		void playEnergyBarrierAmbient(FVector position);

	//Plays the sound produced by a Energy Core when is created
	UFUNCTION(BlueprintImplementableEvent, Category = "Energy Barrier Sound Event")
		void playEnergyBarrierOn(FVector position);

	/********************************************************************************************/
	/********************************************************************************************/
	/**********************				VOLUME CONTROL SECTION				*********************/
	/********************************************************************************************/
	/********************************************************************************************/

	//General volume, for all events
	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent, Category = "Volume Control")
		void setMasterVolume(float volume);

	//Volume for sound events
	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent, Category = "Volume Control")
		void setSoundsVolume(float volume);

	//Volume for music events
	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent, Category = "Volume Control")
		void setMusicVolume(float volume);


};
