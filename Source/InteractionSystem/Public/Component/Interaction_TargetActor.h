// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Data/Interaction_Data.h"
#include "Interaction_TargetActor.generated.h"


class USphereComponent;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnInteractionBegin, const APawn*, Pawn);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnInteractionEnd, const EInteractionResult, InteractionResult,
                                             const APawn*, Pawn);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnInteractionUpdated,
                                               const float, Alpha,
                                               const int, Repeated,
                                               const APawn*, InteractorPawn);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnInteractionReactivated, const APawn*, Pawn);

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnInteractionDeactivated);


UCLASS

(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent), Blueprintable)
class INTERACTIONSYSTEM_API UInteraction_TargetActor : public UActorComponent
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintAssignable, Category = "Interaction")
	FOnInteractionBegin OnInteractionBegin;
	UPROPERTY(BlueprintAssignable, Category = "Interaction")
	FOnInteractionEnd OnInteractionEnd;
	UPROPERTY(BlueprintAssignable, Category = "Interaction")
	FOnInteractionUpdated OnInteractionUpdated;
	UPROPERTY(BlueprintAssignable, Category = "Interaction")
	FOnInteractionReactivated OnInteractionReactivated;
	UPROPERTY(BlueprintAssignable, Category = "Interaction")
	FOnInteractionDeactivated OnInteractionDeactivated;

public:
	bool IsInteractionEnabled() const;
	bool IsReactivationEnabled() const;
	bool IsCancelOnRelease() const;
	bool IsCooldownEnabled() const;
	bool IsUseCustomKeys() const;
	bool IsPendingTimePassed() const;

	bool IsInInnerZone(const TArray<UPrimitiveComponent*>& OverlappedComponents) const;
	bool IsInOuterZone(const TArray<UPrimitiveComponent*>& OverlappedComponents) const;

	float GetHoldSeconds() const;
	float GetRepeatCooldown() const;
	int GetRepeatedTime() const;

	AActor* GetTargetRef() const;

	USceneComponent* GetMarkerTargetComponent() const;
	FVector GetMarkerOffset() const;
	TArray<FKey> GetCustomKeys() const;
	EInteractionType GetInteractionType() const;
	EInteractionFinishMethod GetFinishMethod() const;

	void AssignInteractor(AController* InController);

	void EnableInteraction(bool bEnable);

	void SetInteractionEnabled(bool bNewInteractionEnabled);
	void SetHighlight(const bool InHighlight);

	void UpdateWidgetInfo(const FMargin WidgetScreenMargin, const float InScreenRadiusProgress);

protected:
	virtual void BeginPlay() override;

#pragma region Initialize

	/* 상호작용할 부분(SceneComponent)의 위치를 찾고 그 부분을 중심으로 Inner/Outer NotifyZone을 생성 */
	void InitNotifyZones();
	/* 상호작용할 액터의 컴포넌트들 중에서 강조할 컴포넌트를 찾아서 HighlightedComponents에 삽입 */
	void InitHighlightedComponents();

#pragma endregion Initialize

#pragma region Sphere Overlap Event

	UFUNCTION()
	void OnInnerZoneBeginOverlap(UPrimitiveComponent* OverlappedComp,
	                             AActor* OtherActor, UPrimitiveComponent* OtherComp,
	                             int32 OtherBodyIndex, bool bFromSweep,
	                             const FHitResult& SweepResult);
	UFUNCTION()
	void OnInnerZoneEndOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
	                           UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	UFUNCTION()
	void OnOuterZoneBeginOverlap(UPrimitiveComponent* OverlappedComp,
	                             AActor* OtherActor, UPrimitiveComponent* OtherComp,
	                             int32 OtherBodyIndex, bool bFromSweep,
	                             const FHitResult& SweepResult);
	UFUNCTION()
	void OnOuterZoneEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	                           UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

#pragma endregion Sphere Overlap Event

#pragma region Event Binding

public:
	UFUNCTION()
	void OnAddedToPendingTarget();

	UFUNCTION()
	void OnDeactivated() const;

protected:
	UFUNCTION()
	void OnInteractionBeginEvent(const APawn* InPawn);
	UFUNCTION()
	void OnInteractionEndEvent(const EInteractionResult Result, const APawn* InPawn);

#pragma endregion Event Binding

#pragma region UI

	void UpdateInteractionWidget(const FMargin& Margin, const float NewScreenRadiusProgress);

	/* 상호작용 마커를 어디에 보여줄지 액터 컴포넌트에서 찾고 MarkerTargetComponent를 설정 */
	void SelectMarkerComponent(const FString& InComponentName);

#pragma endregion UI

protected:
#pragma region Debug

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Debug")
	bool bDebugMode;

#pragma endregion Debug

#pragma region Components

	/* 상호작용 가능한 오브젝트를 화면에 알릴 최대 범위 */
	UPROPERTY(BlueprintReadOnly, Category = "InteractionDetect")
	USphereComponent* OuterNotifyZone;

	/* 상호작용 가능한 오브젝트를 화면에 상호작용 버튼을 보여줄 최대 범위 */
	UPROPERTY(BlueprintReadOnly, Category = "InteractionDetect")
	USphereComponent* InnerNotifyZone;

	/* 상호작용 할 오브젝트 레퍼런스 */
	UPROPERTY(BlueprintReadOnly, Category = "Interaction | Actor")
	AActor* TargetRef;

#pragma endregion Components

#pragma region Data

	/* 상호작용 진행률 */
	UPROPERTY(BlueprintReadOnly, Category = "Data")
	float ScreenRadiusProgress;

	/*  */
	UPROPERTY(BlueprintReadOnly, Category = "Data")
	FMargin WidgetMargin;

	UPROPERTY(BlueprintReadOnly, Category = "Data")
	TArray<UPrimitiveComponent*> HighlightedComponents;

	UPROPERTY(BlueprintReadOnly, Category = "Data")
	TArray<AController*> AssignedInteractors;

	UPROPERTY(BlueprintReadOnly, Category = "Data")
	AController* AssignedInteractor;

	UPROPERTY(BlueprintReadOnly, Category = "Data")
	USceneComponent* MarkerTargetComponent;

	UPROPERTY(BlueprintReadOnly, Category = "Data")
	bool bInteractionEnabled = true;

	UPROPERTY(BlueprintReadOnly, Category = "Data")
	float LastInteractionTime;

#pragma endregion Data

#pragma region Marker Settings

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Marker Settings")
	bool bDisplayProgress;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Marker Settings")
	bool bDisplayTriangle;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Marker Settings")
	bool bFlashPOIIcon;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Marker Settings")
	bool bFlashTargetIcon;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Marker Settings")
	bool bRotateInteractionIcon;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Marker Settings")
	FString MarkerTargetName;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Marker Settings")
	FVector MarkerOffset;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Marker Settings")
	FLinearColor TargetIcon_Color;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Marker Settings")
	FLinearColor POIIcon_Color;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Marker Settings")
	FLinearColor Background_Color;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Marker Settings")
	FLinearColor Foreground_Color;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Marker Settings")
	FInteractionIcon Target_Icon;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Marker Settings")
	FInteractionIcon POI_Icon;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Marker Settings")
	FInteractionIcon Background_Icon;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Marker Settings")
	FInteractionIcon Foreground_Icon;

#pragma endregion Marker Settings

#pragma region Highlight Settings

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "HightlihgtSettings")
	FLinearColor Highlight_Color;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "HightlihgtSettings")
	TSet<FString> ComponentsToHighlights;

#pragma endregion Highlight Settings

#pragma region Interaction Settings

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "InteractionSettings")
	bool bCooldownEnabled;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "InteractionSettings")
	int RepeatCount;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "InteractionSettings")
	float RepeatCooldown;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "InteractionSettings")
	float TimeToHold;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "InteractionSettings")
	float InnerZoneRadius;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "InteractionSettings")
	float OuterZoneExtent;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "InteractionSettings")
	FString AttachingComponentName;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "InteractionSettings")
	EInteractionType InteractionType;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "InteractionSettings")
	TMap<EInteractionState, FText> InteractionText;

#pragma endregion Interaction Settings

#pragma region Key Settings

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "InteractionSettings")
	bool bDisplayInteractionKey;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "InteractionSettings")
	bool bUseCustomKey;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "InteractionSettings")
	TArray<FKey> CustomKeys;

#pragma endregion Key Settings

#pragma region OnFinished

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "OnFinished")
	float ReactivateDuration;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "OnFinished")
	EInteractionFinishMethod InteractionFinishMethod;

#pragma endregion OnFinished
};
