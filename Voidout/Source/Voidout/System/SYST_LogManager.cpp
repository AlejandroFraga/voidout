

#include "SYST_LogManager.h"
#include "Kismet/KismetSystemLibrary.h" 
#include "UObject/Object.h"

void USYST_LogManager::PrintString(UObject* WorldContextObject, const FString& InString, bool bPrintToScreen, bool bPrintToLog, const FLinearColor& TextColor, float Duration)
{
#if !(UE_BUILD_SHIPPING || UE_BUILD_TEST)
	if(WorldContextObject != nullptr && !InString.IsEmpty())
	{
		UKismetSystemLibrary::PrintString(WorldContextObject, InString, bPrintToScreen, bPrintToLog, TextColor, Duration);
	}
#endif
}

void USYST_LogManager::PrintWarning(UObject* WorldContextObject, const FString& InString, bool bPrintToScreen, bool bPrintToLog)
{
#if !(UE_BUILD_SHIPPING || UE_BUILD_TEST)
	if (WorldContextObject != nullptr && !InString.IsEmpty())
	{
		UKismetSystemLibrary::PrintString(WorldContextObject, InString, bPrintToScreen, bPrintToLog, FLinearColor(1.f, 1.f, 0.f), 10.f);
	}
#endif
}

void USYST_LogManager::PrintError(UObject* WorldContextObject, const FString& InString, bool bPrintToScreen, bool bPrintToLog)
{
#if !(UE_BUILD_SHIPPING || UE_BUILD_TEST)
	if (WorldContextObject != nullptr && !InString.IsEmpty())
	{
		UKismetSystemLibrary::PrintString(WorldContextObject, InString, bPrintToScreen, bPrintToLog, FLinearColor(1.f, 0.f, 0.f), 20.f);
	}
#endif
}

void USYST_LogManager::PrintSpecial(UObject* WorldContextObject, const FString& InString, bool bPrintToScreen, bool bPrintToLog)
{
#if !(UE_BUILD_SHIPPING || UE_BUILD_TEST)
	if (WorldContextObject != nullptr && !InString.IsEmpty())
	{
		UKismetSystemLibrary::PrintString(WorldContextObject, InString, bPrintToScreen, bPrintToLog, FLinearColor(0.8f, 0.4f, 1.f), 10.f);
	}
#endif
}