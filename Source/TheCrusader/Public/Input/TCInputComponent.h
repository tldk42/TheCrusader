// Written by jaegang lim

#pragma once

#include "CoreMinimal.h"
#include "EnhancedInputComponent.h"
#include "TCInputConfig.h"
#include "TCInputComponent.generated.h"


UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class THECRUSADER_API UTCInputComponent : public UEnhancedInputComponent
{
	GENERATED_BODY()

public:
	template <class UserClass, typename FuncType>
	void BindActionByTag(const UTCInputConfig* InputConfig, const FGameplayTag& InputTag, ETriggerEvent TriggerEvent,
						 UserClass* Object, FuncType Func);
	
};
template <class UserClass, typename FuncType>
void UTCInputComponent::BindActionByTag(const UTCInputConfig* InputConfig, const FGameplayTag& InputTag,
										ETriggerEvent TriggerEvent, UserClass* Object, FuncType Func)
{
	check(InputConfig);

	if (const UInputAction* IA = InputConfig->FindNativeInputActionForTag(InputTag))
	{
		BindAction(IA, TriggerEvent, Object, Func);
	}
}
