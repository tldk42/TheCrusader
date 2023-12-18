// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Data/Interaction_Data.h"
#include "InteractionComponent.generated.h"


class UInputAction;
class UPostProcessComponent;
class UInteraction_Indicator;
class UInteraction_TargetActor;

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent), Blueprintable)
class INTERACTIONSYSTEM_API UInteractionComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	static UInteractionComponent* GetInteractionComponent(const AController* PlayerController);

	void OnInteractionBegin_Event();

public:
	void TryInteraction();
	TArray<FKey> GetInteractionKeys();
	bool IsInteractable(UInteraction_TargetActor* ItemToFind) const;


	void OnInteractionTargetUpdated_Server(bool bAdd, UInteraction_TargetActor* InteractionTarget);
	void OnPointOfInterestUpdated_Server(bool bAdd, UInteraction_TargetActor* InteractionTarget);
	void AddToPendingTargets(UInteraction_TargetActor* InteractionTarget);

protected:
	virtual void BeginPlay() override;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType,
	                           FActorComponentTickFunction* ThisTickFunction) override;

	void InitPlayerData();
	UFUNCTION(BlueprintImplementableEvent)
	void InitPooledMarkerWidgets();
	void InitPostProcessComponent();

	UFUNCTION()
	void UpdateInteractionKeys();
	void OnInteractionTargetUpdated_Client(bool bAdd, const UInteraction_TargetActor* InteractionTarget);
	void OnPointOfInterestUpdated_Client(bool bAdd, UInteraction_TargetActor* InteractionTarget);
	void OnInteractionTargetReactivated(UInteraction_TargetActor* InteractionTarget);
	UFUNCTION()
	void OnInteractionTargetDestroyed(AActor* DestroyedActor);
	UFUNCTION()
	void CheckForPendingTargets();

	UInteraction_Indicator* FindEmptyWidget();

private:
	void AddToDeactivatedTargets(UInteraction_TargetActor* InteractionTarget);
	void OnInteractionFinished(UInteraction_TargetActor* Interaction_TargetActor, const EInteractionResult Result);

	UInteraction_TargetActor* FindBestInteractableTarget();
	void UpdateBestInteractable(UInteraction_TargetActor* NewTarget);

	void SetNewTarget(UInteraction_TargetActor* NewTarget, const bool bSelected);
	void ResetData();

	UInteraction_Indicator* FindWidgetByInteractionTarget(const UInteraction_TargetActor* InteractionTarget);

	void DebugTrace() const;

#pragma region Variables

protected:
#pragma region Data

	UPROPERTY(BlueprintReadOnly, Category = "Data")
	TArray<UInteraction_TargetActor*> InteractionTargets;

	UPROPERTY(BlueprintReadOnly, Category = "Data")
	TArray<UInteraction_TargetActor*> PointsOfInterests;

	UPROPERTY(BlueprintReadOnly, Category = "Data")
	TArray<UInteraction_TargetActor*> PendingTargets;

	UPROPERTY(BlueprintReadOnly, Category = "Data")
	TArray<UInteraction_TargetActor*> DeactivatedTargets;

	UPROPERTY(BlueprintReadOnly, Category = "Data")
	TArray<UInteraction_Indicator*> WidgetPool;

	UPROPERTY(BlueprintReadOnly, Category = "Data")
	APlayerController* OwnerController;

	UPROPERTY(BlueprintReadOnly, Category = "Data")
	UPostProcessComponent* PostProcessComponent;

	UPROPERTY(BlueprintReadOnly, Category = "Data")
	UInteraction_Indicator* CurrentInteractionWidget;

	UPROPERTY(BlueprintReadOnly, Category = "Data")
	UInteraction_TargetActor* BestInteractionTarget;

	UPROPERTY(BlueprintReadOnly, Category = "Data")
	TArray<FKey> InteractionKeys;

	UPROPERTY(BlueprintReadOnly, Category = "Data")
	float CurrentHoldTime;

	UPROPERTY(BlueprintReadOnly, Category = "Data")
	float RepeatCoolTime;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data")
	float PendingTargetCheckInterval = .35f;

	UPROPERTY(BlueprintReadOnly, Category = "Data")
	int RepeatedTime;

	UPROPERTY(BlueprintReadOnly, Category = "Data")
	bool bIsInteracting;

	UPROPERTY(BlueprintReadOnly, Category = "Data")
	bool bKeyJustPressed;

	UPROPERTY(BlueprintReadOnly, Category = "Data")
	bool bIsGamepad;

	UPROPERTY(BlueprintReadOnly, Category = "Data")
	FKey LastPressedKey;

	UPROPERTY(BlueprintReadOnly, Category = "Data")
	FTimerHandle BeginUpdateKeys_TimerHandle;

	UPROPERTY(BlueprintReadOnly, Category = "Data")
	FTimerHandle PendingTarget_TimerHandle;

	UPROPERTY(BlueprintReadOnly, Category = "Data")
	UMaterialInstanceDynamic* OutlineInstanceDynamic_Material;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Material")
	UMaterialInterface* Outline_Material;

#pragma endregion Data

#pragma region Main
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Main")
	int DefaultObjectPoolSize;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Main")
	TSubclassOf<UInteraction_Indicator> IndicatorWidgetClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Main")
	UInputAction* InteractionInputAction;
#pragma endregion Main

#pragma region Debug

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Debug")
	bool bDebugMode;

#pragma endregion Debug

#pragma region MarkerSettings

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "MarkerSettings")
	FMargin WidgetScreenMargin;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "MarkerSettings")
	float ScreenRadiusProgress;

#pragma endregion MarkerSettings

#pragma endregion Variables
};
