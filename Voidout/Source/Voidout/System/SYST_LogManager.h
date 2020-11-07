
#pragma once

#include "CoreMinimal.h"
#include "SYST_LogManager.generated.h"

/**
 * 
 */
UCLASS()
class VOIDOUT_API USYST_LogManager : public UObject
{
	GENERATED_BODY()

public:
	USYST_LogManager() = default;

	UFUNCTION(BlueprintCallable, Category = "Log Manager")
	static void PrintString(UObject* WorldContextObject, const FString& InString, bool bPrintToScreen = true, bool bPrintToLog = true, const FLinearColor& TextColor = FLinearColor(0.f, 0.66f, 1.f), float Duration = 4.f);

	UFUNCTION(BlueprintCallable, Category = "Log Manager")
	static void PrintWarning(UObject* WorldContextObject, const FString& InString, bool bPrintToScreen = true, bool bPrintToLog = true);

	UFUNCTION(BlueprintCallable, Category = "Log Manager")
	static void PrintError(UObject* WorldContextObject, const FString& InString, bool bPrintToScreen = true, bool bPrintToLog = true);

	UFUNCTION(BlueprintCallable, Category = "Log Manager")
	static void PrintSpecial(UObject* WorldContextObject, const FString& InString, bool bPrintToScreen = true, bool bPrintToLog = true);

};
