// Written by jaegang lim


#include "Input/TCInputConfig.h"

const UInputAction* UTCInputConfig::FindNativeInputActionForTag(const FGameplayTag& InputTag, bool bLogNotFound) const
{
	for (const FTCInputAction& TaggedInputAction : TaggedInputActions)
	{
		if (TaggedInputAction.InputAction && (TaggedInputAction.InputTag == InputTag))
		{
			return TaggedInputAction.InputAction;
		}
	}

	if (bLogNotFound)
	{
		UE_LOG(LogTemp, Error, TEXT("Can't find NativeInputAction for InputTag [%s] on InputConfig [%s]."),
		       *InputTag.ToString(),
		       *GetNameSafe(this))
	}

	return nullptr;
}
