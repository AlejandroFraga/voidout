
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Assets/ACTR_Light.h"
#include "ACTR_SwitchableLight.generated.h"

UCLASS()
class VOIDOUT_API AACTR_SwitchableLight : public AACTR_Light
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AACTR_SwitchableLight();

	UFUNCTION(BlueprintCallable)
	void SwitchLight(float voidoutDuration, int voidoutImpulse);

	UFUNCTION(BlueprintCallable)
	void LightOnPermanent();

	UFUNCTION(BlueprintCallable)
	void LightOffPermanent();

	void SetActionIndicatorShow(bool show);

	//UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Voidout")
	float m_timeWithLightOff{ 3.0f };

	//UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Voidout")
	int m_impulseForce{ 7500 };

	inline bool isLightOnVoidout() const noexcept { return m_isLightOnVoidout; };

	inline bool isLightOnPermanent() const noexcept { return m_isLightOnPermanent; };

	inline int GetConnectionCount() const noexcept { return m_connectionCount; };

	void AddConnectionCount(int numberToadd);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LightOff")
	float m_speedLightOff{ 1.5f };

	UPROPERTY(BlueprintReadWrite)
	class UWidgetComponent* m_voidoutIndicator{ nullptr };

	//Material that will be set to the tower crystal when the tower is off:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	class UMaterialInterface* m_defaultTowerMaterial{ nullptr };

	//Array to storage where is the enemy inside de light
	//UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<AActor*> m_enemiesInLightZone;

protected:

	virtual void BeginPlay() override;

	//Enter in the LightZone
	UFUNCTION(Category = "Voidout")
	void OnLightZoneOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	//Exit the secureZone 
	UFUNCTION(Category = "Voidout")
	void OnLightZoneOverlapEnd(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);
	
	void Tick(float DeltaTime) override;

private:

	//Trigger sphere with the radius cone size by a multiplier assigned by blueprint
	class USphereComponent* m_collisionLightZone{ nullptr };

	int m_connectionCount{ 0 }; //Cuando el explorador termina un camino suma uno al contador

	bool m_isLightOnVoidout{ true }; //Cuando la luz se apaga por la habilidad voidout se pone a false

	bool m_isLightOnPermanent{ true }; //Cuando la luz se apaga del todo se pone a false para siempre

	class UPointLightComponent* pointLight1{ nullptr };

	class UPointLightComponent* pointLight2{ nullptr };

	float m_timeCounter{ 0.0 };

	float m_initialLightIntensity{ 0.0 };

	//Variables to stopLight:

	const int m_jumpForce = 100;

	UStaticMeshComponent* m_meshComponent{ nullptr };

	//Light material from tower.
	UMaterialInterface* m_lightMaterial{ nullptr };

	//Crystal material from tower:
	UMaterialInterface* m_crystalMaterial{ nullptr };

	USpotLightComponent* m_light{ nullptr };

	UStaticMeshComponent* m_ringPulse{ nullptr };

	TArray<AActor*> m_stunedEnemies{};

	void InitializeSwitchLight();

	//Function called when light is off, to eject and stun AI
	void IsLightOffBehavior();

	void TurnOnLight(UPointLightComponent* pointLight, bool on = true);

	void SetWidgetShow(class UUserWidget* userWidget, bool show);

};
