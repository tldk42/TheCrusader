// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Data/Interaction_Data.h"
#include "Interaction_Indicator.generated.h"

class UTextBlock;
class UOverlay;
class UHorizontalBox;
class UGridPanel;
class USizeBox;
class UWidgetSwitcher;
class UInteraction_TargetActor;
class UBorder;
/**
 * 
 */
UCLASS()
class INTERACTIONSYSTEM_API UInteraction_Indicator : public UUserWidget
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	void SetInteractionProgress(const float NewProgress);

	UInteraction_TargetActor* GetWidgetInteractionTarget() const;

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	void PlayInteractionCompletedAnimation(const EInteractionResult Result);
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	void PlayInteractionUpdateAnimation();
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	void UpdateInteractionText(bool bAdd, EInteractionState InteractionState);
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	void UpdateInteractionTarget(UInteraction_TargetActor* TargetActor);
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	void UpdateContentState(bool bAdd);
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	void SetInteractionKeyText(const FKey& Key);
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	void OnWidgetNewTarget(bool bAdd);

protected:
	virtual void NativeConstruct() override;

	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	void UpdatePositionInViewport();

protected:
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	void OnSlideUpAnimStarted();

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	void OnSlideUpAnimFinished();

private:
#pragma region Widget

	UPROPERTY(BlueprintReadOnly, Category = "Widgets", meta = (BindWidget, AllowPrivateAccess))
	UWidgetSwitcher* ContentSwitcher;

	UPROPERTY(BlueprintReadOnly, Category = "Widgets", meta = (BindWidget, AllowPrivateAccess))
	USizeBox* POI_Container;

	UPROPERTY(BlueprintReadOnly, Category = "Widgets", meta = (BindWidget, AllowPrivateAccess))
	UBorder* POI_Icon;

	UPROPERTY(BlueprintReadOnly, Category = "Widgets", meta = (BindWidget, AllowPrivateAccess))
	UGridPanel* InteractionContainer;

	UPROPERTY(BlueprintReadOnly, Category = "Widgets", meta = (BindWidget, AllowPrivateAccess))
	UBorder* Icon;

	UPROPERTY(BlueprintReadOnly, Category = "Widgets", meta = (BindWidget, AllowPrivateAccess))
	UHorizontalBox* InteractionInfoBox;

	UPROPERTY(BlueprintReadOnly, Category = "Widgets", meta = (BindWidget, AllowPrivateAccess))
	UBorder* Background;

	UPROPERTY(BlueprintReadOnly, Category = "Widgets", meta = (BindWidget, AllowPrivateAccess))
	UBorder* Foreground;

	UPROPERTY(BlueprintReadOnly, Category = "Widgets", meta = (BindWidget, AllowPrivateAccess))
	UOverlay* TriangleBox;

	UPROPERTY(BlueprintReadOnly, Category = "Widgets", meta = (BindWidget, AllowPrivateAccess))
	UBorder* Triangle;

	UPROPERTY(BlueprintReadOnly, Category = "Widgets", meta = (BindWidget, AllowPrivateAccess))
	UWidgetSwitcher* LootTextSwitcher;

	UPROPERTY(BlueprintReadOnly, Category = "Widgets", meta = (BindWidget, AllowPrivateAccess))
	UBorder* LootKeyIcon;

	UPROPERTY(BlueprintReadOnly, Category = "Widgets", meta = (BindWidget, AllowPrivateAccess))
	UTextBlock* LootKey;

	UPROPERTY(BlueprintReadOnly, Category = "Widgets", meta = (BindWidget, AllowPrivateAccess))
	UTextBlock* InfoText;

#pragma endregion Widget

#pragma region Widget Animation

	FWidgetAnimationDynamicEvent SlideUpStartDelegate;
	FWidgetAnimationDynamicEvent SlideUpEndDelegate;

	UPROPERTY(BlueprintReadOnly, Category = "Animation", Transient, meta = (BindWidgetAnim, AllowPrivateAccess))
	UWidgetAnimation* SlideUp;

	UPROPERTY(BlueprintReadOnly, Category = "Animation", Transient, meta = (BindWidgetAnim, AllowPrivateAccess))
	UWidgetAnimation* LootText;

	UPROPERTY(BlueprintReadOnly, Category = "Animation", Transient, meta = (BindWidgetAnim, AllowPrivateAccess))
	UWidgetAnimation* Flash;

	UPROPERTY(BlueprintReadOnly, Category = "Animation", Transient, meta = (BindWidgetAnim, AllowPrivateAccess))
	UWidgetAnimation* InteractionCompleted;

	UPROPERTY(BlueprintReadOnly, Category = "Animation", Transient, meta = (BindWidgetAnim, AllowPrivateAccess))
	UWidgetAnimation* InteractionCanceled;

	UPROPERTY(BlueprintReadOnly, Category = "Animation", Transient, meta = (BindWidgetAnim, AllowPrivateAccess))
	UWidgetAnimation* InteractionRepeatUpdate;

	UPROPERTY(BlueprintReadOnly, Category = "Animation", Transient, meta = (BindWidgetAnim, AllowPrivateAccess))
	UWidgetAnimation* Rotate;

	UPROPERTY(BlueprintReadOnly, Category = "Animation", Transient, meta = (BindWidgetAnim, AllowPrivateAccess))
	UWidgetAnimation* InteractionTextAnim;

#pragma endregion Widget Animation

#pragma region Component

	UPROPERTY(BlueprintReadWrite, Category = "Component", meta = (AllowPrivateAccess))
	UInteraction_TargetActor* WidgetInteractionTarget;

#pragma endregion Component


#pragma region Property

	UPROPERTY(BlueprintReadWrite, Category = "Property", meta = (AllowPrivateAccess))
	bool bIsInteractionState;

	UPROPERTY(BlueprintReadWrite, Category = "Property", meta = (AllowPrivateAccess))
	bool bShowProgress;

	UPROPERTY(BlueprintReadWrite, Category = "Property", meta = (AllowPrivateAccess))
	bool bClearTarget;

	UPROPERTY(BlueprintReadWrite, Category = "Property", meta = (AllowPrivateAccess))
	bool bCustomClockWise;

	UPROPERTY(BlueprintReadWrite, Category = "Property", meta = (AllowPrivateAccess))
	float CustomProgress;

	UPROPERTY(BlueprintReadWrite, Category = "Property", meta = (AllowPrivateAccess))
	float CurrentProgress;

	UPROPERTY(BlueprintReadWrite, Category = "Property", meta = (AllowPrivateAccess))
	float ScreenRadiusProgress;

	UPROPERTY(BlueprintReadWrite, Category = "Property", meta = (AllowPrivateAccess))
	FVector LastWorldLocation;

	UPROPERTY(BlueprintReadWrite, Category = "Property", meta = (AllowPrivateAccess))
	FMargin ScreenMargin;

	UPROPERTY(BlueprintReadWrite, Category = "Property", meta = (AllowPrivateAccess))
	FVector2D WidgetPositionErrorFactor;

	UPROPERTY(BlueprintReadWrite, Category = "Property", meta = (AllowPrivateAccess))
	FText CurrentInterationText;

#pragma endregion Property
};
