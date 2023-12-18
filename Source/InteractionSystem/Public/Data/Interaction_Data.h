// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Interaction_Data.generated.h"

UENUM(BlueprintType)
enum class EInteractionType : uint8
{
	None,
	Tap,
	Hold,
	Repeat
};

UENUM(BlueprintType)
enum class EInteractionState : uint8
{
	None,
	Waiting,
	Interacting,
	Done
};

UENUM(BlueprintType)
enum class EInteractionResult : uint8
{
	None,
	Completed,
	Canceled,
};

UENUM(BlueprintType)
enum class EInteractionFinishMethod : uint8
{
	None,
	DestroyOnComplete,
	DestroyOnCancel,
	DestroyOnCompleteOrCancel,
	ReactivateOnComplete,
	ReactivateOnCancel,
	ReactivateOnCompleteOrCancel,
	ReactivateOnCancel_DestroyOnComplete,
	ReactivateOnCancelDestroyOnComplete,
	ReactivateOnCancelDeactivateOnComplete,
	ReactivateOnCompleteDestroyOnCancel,
	ReactivateOnCompleteDeactivateOnCancel,
	DeactivateOnComplete,
	DeactivateOnCancel,
	DeactivateOnCompleteOrCancel,
	DeactivateOnCanceledDestroyOnComplete,
	DeactivateOnCanceledReactivateAfterDurationOnComplete,
	DeactivateOnCompletedDestroyOnCancel,
	DeactivateOnCompletedReactivateOnCancel
};

USTRUCT(BlueprintType)
struct FInteractionIcon
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bUseMaterialAsTexture;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UTexture2D* Texture;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UMaterialInstance* Material;
};
