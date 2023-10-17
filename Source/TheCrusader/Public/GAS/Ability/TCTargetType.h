// Written by jaegang lim

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "TCTargetType.generated.h"

struct FGameplayEventData;
class ATCGASCharacter;
/**
 * 어빌리티가 적용될 대상을 결정
 */
UCLASS(Blueprintable, meta = (ShowWorldContextPin))
class THECRUSADER_API UTCTargetType : public UObject
{
	GENERATED_BODY()

public:
	UTCTargetType()
	{
	}

	UFUNCTION(BlueprintNativeEvent)
	void GetTargets(ATCGASCharacter* TargetingCharacter, AActor* TargetingActor, FGameplayEventData EventData,
	                TArray<FHitResult>& OutHitResults, TArray<AActor*>& OutActor) const;
};

UCLASS(NotBlueprintable)
class UTargetType_UseOwner : public UTCTargetType
{
	GENERATED_BODY()

public:
	UTargetType_UseOwner()
	{
	}

	virtual void GetTargets_Implementation(ATCGASCharacter* TargetingCharacter, AActor* TargetingActor, FGameplayEventData EventData, TArray<FHitResult>& OutHitResults, TArray<AActor*>& OutActor) const override;
};

UCLASS(NotBlueprintable)
class UTargetType_UseEventData : public UTCTargetType
{
	GENERATED_BODY()

public:
	UTargetType_UseEventData()
	{
	}

	virtual void GetTargets_Implementation(ATCGASCharacter* TargetingCharacter, AActor* TargetingActor, FGameplayEventData EventData, TArray<FHitResult>& OutHitResults, TArray<AActor*>& OutActor) const override;
};
