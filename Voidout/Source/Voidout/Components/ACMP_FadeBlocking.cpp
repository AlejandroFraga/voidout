
#include "ACMP_FadeBlocking.h"

UACMP_FadeBlocking::UACMP_FadeBlocking()
{
	PrimaryComponentTick.bCanEverTick = false;

	// Add first collision type
	objectTypes.Add(ECC_WorldStatic);
}

// Called when the game starts
void UACMP_FadeBlocking::BeginPlay()
{
	Super::BeginPlay();

	if (bEnable && GetWorld() != nullptr)
	{
		GetWorld()->GetTimerManager().SetTimer(timerHandle_AddObjectsTimer, this, &UACMP_FadeBlocking::AddObjectsToHide,
			addObjectInterval, true);
		GetWorld()->GetTimerManager().SetTimer(timerHandle_ObjectComputeTimer, this, &UACMP_FadeBlocking::FadeObjWorker,
			calcFadeInterval, true);
	}
}

void UACMP_FadeBlocking::AddObjectsToHide()
{
	if (!bEnable)
		return;

	UGameplayStatics::GetAllActorsOfClass(this, playerClass, characterArray);

	for (AActor* currentActor : characterArray)
	{
		const FVector traceStart = GEngine->GetFirstLocalPlayerController(GetWorld())->PlayerCameraManager->GetCameraLocation();
		const FVector traceEnd = currentActor->GetActorLocation();
		const FRotator traceRot = currentActor->GetActorRotation();
		FVector traceLentgh = traceStart - traceEnd;
		const FQuat acQuat = currentActor->GetActorQuat();

		if (traceLentgh.Size() >= workDistance)
			continue;
		
		FCollisionQueryParams traceParams(TEXT("FadeObjectsTrace"), true, GetOwner());

		traceParams.AddIgnoredActors(actorsIgnore);
		//traceParams.bTraceAsyncScene = true;
		traceParams.bReturnPhysicalMaterial = false;
		// Not tracing complex uses the rough collision instead making tiny objects easier to select.
		traceParams.bTraceComplex = false;

		TArray<FHitResult> hitArray;
		TArray<TEnumAsByte<EObjectTypeQuery>> traceObjectTypes;

		// Convert ECollisionChannel to ObjectType
		for(auto& ot : objectTypes)
		{
			traceObjectTypes.Add(UEngineTypes::ConvertToObjectType(ot.GetValue()));
		}

		// Check distance between camera and player for new object to fade, and add this in array
		GetWorld()->SweepMultiByObjectType(hitArray, traceStart, traceEnd, acQuat, traceObjectTypes,
		                                   FCollisionShape::MakeCapsule(capsuleRadius, capsuleHalfHeight), traceParams);

		for(auto& hA : hitArray)
		{
			if (hA.bBlockingHit && IsValid(hA.GetComponent()) && !fadeObjectsHit.Contains(hA.GetComponent()))
			{
				fadeObjectsHit.AddUnique(hA.GetComponent());
			}
		}
	}

	// Make fade array after complete GetAllActorsOfClass loop
	for (auto* fO : fadeObjectsHit)
	{
		// If not contains this component in fadeObjectsTemp
		if (fadeObjectsTemp.Contains(fO))
			continue;
		
		TArray<UMaterialInterface*> lBaseMaterials;
		TArray<UMaterialInstanceDynamic*> lMidMaterials;

		lBaseMaterials.Empty();
		lMidMaterials.Empty();

		fadeObjectsTemp.AddUnique(fO);

		// For loop all materials ID in object
		for (int nM = 0; nM < fO->GetNumMaterials(); ++nM)
		{
			lMidMaterials.Add(UMaterialInstanceDynamic::Create(fadeMaterial, fO));
			lBaseMaterials.Add(fO->GetMaterial(nM));

			// Set new material on object
			fO->SetMaterial(nM, lMidMaterials.Last());
		}
		// Create new fade object in array of objects to fade
		FFadeObjStruct newObject;
		newObject.NewElement(fO, lBaseMaterials, lMidMaterials, immediatelyFade, true);
		// Add object to array
		fadeObjects.Add(newObject);

		// Set collision on Primitive Component
		fO->SetCollisionResponseToChannel(ECC_Camera, ECR_Ignore);
	}

	// Set hide to visible true if contains
	for (int fOT = 0; fOT < fadeObjectsTemp.Num(); ++fOT)
	{
		if (!fadeObjectsHit.Contains(fadeObjectsTemp[fOT]))
		{
			fadeObjects[fOT].SetHideOnly(false);
		}
	}

	// Clear array
	fadeObjectsHit.Empty();
}

void UACMP_FadeBlocking::FadeObjWorker()
{
	for (int i = 0; i < fadeObjects.Num(); ++i)
	{
		// Index of iteration
		const int fnID = i;
		const float adaptiveFade = fnID == fadeObjects.Num() ? nearObjectFade : farObjectFade;
			
		// For loop fadeMID array
		for (int t = 0; t < fadeObjects[i].fadeMID.Num(); ++t)
		{
			const float currentF = fadeObjects[i].fadeCurrent;

			const float targetF = fadeObjects[i].bToHide ? adaptiveFade : 1.0f;

			const float newFade = FMath::FInterpConstantTo(currentF, targetF, GetWorld()->GetDeltaSeconds(), fadeRate);

			fadeObjects[i].fadeMID[t]->SetScalarParameterValue("Fade", newFade);

			currentFade = newFade;

			fadeObjects[i].SetFadeAndHide(newFade, fadeObjects[i].bToHide);
		}
		// remove index in array
		if (currentFade != 1.0f)
			continue;
		
		for (int bmi = 0; bmi < fadeObjects[fnID].baseMatInterface.Num(); ++bmi)
		{
			fadeObjects[fnID].primitiveComp->SetMaterial(bmi, fadeObjects[fnID].baseMatInterface[bmi]);
		}

		fadeObjects[fnID].primitiveComp->SetCollisionResponseToChannel(ECC_Camera, ECR_Block);
		fadeObjects.RemoveAt(fnID);
		fadeObjectsTemp.RemoveAt(fnID);
	}
}

void UACMP_FadeBlocking::CleanFadeObjects()
{
	if (fadeObjects.Num() <= 0)
		return;
	
	// For loop all fade objects
	for(auto& fo : fadeObjects)
	{
		for (int bmi = 0; bmi < fo.baseMatInterface.Num(); ++bmi)
		{
			fo.primitiveComp->SetMaterial(bmi, fo.baseMatInterface[bmi]);
		}
		fo.primitiveComp->SetCollisionResponseToChannel(ECC_Camera, ECR_Block);
	}
	
	fadeObjects.Empty();
	fadeObjectsTemp.Empty();
}

void UACMP_FadeBlocking::SetEnable(bool _enable)
{
	bEnable = _enable;
}