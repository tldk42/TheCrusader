// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Interaction_Indicator.h"

#include "Animation/WidgetAnimation.h"
#include "Components/Border.h"

void UInteraction_Indicator::NativeConstruct()
{
	Super::NativeConstruct();

	SetAlignmentInViewport(FVector2D(.5f, .5f));
	SetPositionInViewport(FVector2D(0.f, 0.f), true);

	SetInteractionProgress(0.f);
	UpdateInteractionTarget(nullptr);

	SlideUpStartDelegate.BindDynamic(this, &ThisClass::OnSlideUpAnimStarted);
	SlideUpEndDelegate.BindDynamic(this, &ThisClass::OnSlideUpAnimFinished);

	BindToAnimationStarted(SlideUp, SlideUpStartDelegate);
	BindToAnimationFinished(SlideUp, SlideUpEndDelegate);
}

void UInteraction_Indicator::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	UpdatePositionInViewport();
}

UInteraction_TargetActor* UInteraction_Indicator::GetWidgetInteractionTarget() const
{
	return WidgetInteractionTarget;
}
