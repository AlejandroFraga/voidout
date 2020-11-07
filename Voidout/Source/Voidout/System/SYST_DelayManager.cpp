

#include "SYST_DelayManager.h"
#include "System/SYST_LogManager.h"

#include "Kismet/KismetSystemLibrary.h"
#include "UObject/Object.h"

/*FLatentActionInfo USYST_DelayManager::Delay(UObject* WorldContextObject, UObject* InCallbackTarget, const FString& FunctionName, float duration, int32 InLinkage, int32 InUUID)
{
	// If any of the args are incorrect
	if (InCallbackTarget == nullptr || WorldContextObject == nullptr || FunctionName.IsEmpty()) return FLatentActionInfo();

	// We set the delay and update the numbers
	char* result = TCHAR_TO_ANSI(*FunctionName);
	TCHAR* FunctionTChar = UTF8_TO_TCHAR(result);
	const FLatentActionInfo latentActionInfo{ (InLinkage > 0 ? InLinkage : m_InLinkage), InUUID > 0 ? InUUID : m_InUUID, FunctionTChar, InCallbackTarget };
	UKismetSystemLibrary::Delay(WorldContextObject, duration, latentActionInfo);
	
	
	if (InLinkage < 0)
		m_InLinkage++;
	if(InUUID < 0)
		m_InUUID++;
	return latentActionInfo;
}*/

FLatentActionInfo USYST_DelayManager::RetriggerableDelay(UObject* WorldContextObject, UObject* InCallbackTarget, const FString& FunctionName, float duration, int32 InLinkage, int32 InUUID)
{
	// If any of the args are incorrect
	if (InCallbackTarget == nullptr || WorldContextObject == nullptr || FunctionName.IsEmpty()) return FLatentActionInfo();

	// We set the delay and update the numbers
	char* result = TCHAR_TO_ANSI(*FunctionName);
	TCHAR* FunctionTChar = UTF8_TO_TCHAR(result);
	const FLatentActionInfo latentActionInfo{ (InLinkage > 0 ? InLinkage : m_InLinkage), (InUUID > 0 ? InUUID : m_InUUID), FunctionTChar, InCallbackTarget };
	UKismetSystemLibrary::RetriggerableDelay(WorldContextObject, duration, latentActionInfo);

	if (InLinkage < 0)
		m_InLinkage++;
	if (InUUID < 0)
		m_InUUID++;

	return latentActionInfo;
}