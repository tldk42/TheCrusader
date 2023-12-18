// Fill out your copyright notice in the Description page of Project Settings.


#include "Component/InteractionComponent.h"

#include "EnhancedInputSubsystems.h"
#include "UI/Interaction_Indicator.h"
#include "Blueprint/UserWidget.h"
#include "Component/Interaction_TargetActor.h"
#include "Components/PostProcessComponent.h"

void UInteractionComponent::BeginPlay()
{
	Super::BeginPlay();

	InitPlayerData();
	InitPooledMarkerWidgets();
}

void UInteractionComponent::TickComponent(float DeltaTime, ELevelTick TickType,
                                          FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	UInteraction_TargetActor* NewTarget = FindBestInteractableTarget();
	UpdateBestInteractable(NewTarget);

	DebugTrace();

	UE_LOG(LogTemp, Display, TEXT("Interaction Tick"));
}

void UInteractionComponent::InitPlayerData()
{
	OwnerController = Cast<APlayerController>(GetOwner());

	InitPostProcessComponent();
	GetWorld()->GetTimerManager().SetTimer(BeginUpdateKeys_TimerHandle,
	                                       this,
	                                       &ThisClass::UpdateInteractionKeys,
	                                       .2f,
	                                       false,
	                                       0.f);
}

// void UInteractionComponent::InitPooledMarkerWidgets()
// {
// 	if (OwnerController->IsLocalController())
// 	{
// 		if (IndicatorWidgetClass)
// 			for (int i = 0; i < DefaultObjectPoolSize; ++i)
// 			{
// 				UInteraction_Indicator* WidgetInstance = CreateWidget<UInteraction_Indicator>(
// 					OwnerController,
// 					IndicatorWidgetClass);
//
// 				WidgetPool.AddUnique(WidgetInstance);
// 				WidgetInstance->AddToViewport();
// 			}
// 	}
// }

void UInteractionComponent::InitPostProcessComponent()
{
	PostProcessComponent = NewObject<UPostProcessComponent>(OwnerController,
	                                                        UPostProcessComponent::StaticClass(),
	                                                        TEXT("PostProcessComponent"));

	OutlineInstanceDynamic_Material = UMaterialInstanceDynamic::Create(Outline_Material, this);

	check(PostProcessComponent);
	check(OwnerController->GetPawn());
	check(OutlineInstanceDynamic_Material);

	PostProcessComponent->SetupAttachment(OwnerController->GetPawn()->GetRootComponent());

	TArray<FWeightedBlendable> TempArray;
	TempArray.Add({0.f, Outline_Material});

	FWeightedBlendables WeightedBlendables =
		PostProcessComponent->Settings.WeightedBlendables;
	WeightedBlendables.Array = TempArray;

	PostProcessComponent->Settings.WeightedBlendables = WeightedBlendables;
}

UInteractionComponent* UInteractionComponent::GetInteractionComponent(const AController* PlayerController)
{
	return Cast<UInteractionComponent>(PlayerController->GetComponentByClass(StaticClass()));
}

void UInteractionComponent::OnInteractionBegin_Event()
{
	BestInteractionTarget->OnInteractionBegin.Broadcast(OwnerController->GetPawn());
	bIsInteracting = true;
}

void UInteractionComponent::TryInteraction()
{
	TArray<FKey> Keys = GetInteractionKeys();

	if (Keys.Num() > 0)
	{
		for (const FKey& Key : Keys)
		{
			if (OwnerController->WasInputKeyJustPressed(Key))
			{
				LastPressedKey = Key;
				break;
			}
		}
	}

	switch (BestInteractionTarget->GetInteractionType())
	{
	case EInteractionType::Tap:
		if (bKeyJustPressed)
		{
			OnInteractionBegin_Event();

			CurrentInteractionWidget->UpdateInteractionText(true, EInteractionState::Done);
			CurrentInteractionWidget->PlayInteractionCompletedAnimation(EInteractionResult::Completed);

			OnInteractionFinished(BestInteractionTarget, EInteractionResult::Completed);
		}
		break;
	case EInteractionType::Hold:
		if (OwnerController->IsInputKeyDown(LastPressedKey) && bKeyJustPressed)
		{
			if (CurrentHoldTime == 0.f)
			{
				CurrentInteractionWidget->UpdateInteractionText(false, EInteractionState::Interacting);
				OnInteractionBegin_Event();

				CurrentHoldTime += GetWorld()->GetDeltaSeconds();
				const float HoldSec = FMath::Min(
					1.f, FMath::Clamp(CurrentHoldTime, 0.f, BestInteractionTarget->GetHoldSeconds()));
				CurrentInteractionWidget->SetInteractionProgress(HoldSec);

				BestInteractionTarget->OnInteractionUpdated.Broadcast(HoldSec, 0, OwnerController->GetPawn());

				if (HoldSec == 1.f)
				{
					CurrentHoldTime = 0.f;
					CurrentInteractionWidget->PlayInteractionCompletedAnimation(EInteractionResult::Completed);
					OnInteractionFinished(BestInteractionTarget, EInteractionResult::Completed);
				}
			}
		}
		else
		{
			if (CurrentHoldTime != 0.f)
			{
				if (BestInteractionTarget->IsCooldownEnabled())
				{
					CurrentHoldTime = FMath::Max(CurrentHoldTime - GetWorld()->GetDeltaSeconds(), 0.f);

					const float HoldSecond = BestInteractionTarget->GetHoldSeconds();

					const float HoldSec = FMath::Max(
						0.f, FMath::Clamp(CurrentHoldTime, 0.f, HoldSecond) / (HoldSecond > 0 ? HoldSecond : 1));

					CurrentInteractionWidget->SetInteractionProgress(HoldSec);

					if (CurrentHoldTime == 0.f)
					{
						CurrentInteractionWidget->PlayInteractionCompletedAnimation(EInteractionResult::Canceled);
						OnInteractionFinished(BestInteractionTarget, EInteractionResult::Canceled);
					}
					else
					{
						BestInteractionTarget->OnInteractionUpdated.Broadcast(HoldSec, 0, OwnerController->GetPawn());
						if (BestInteractionTarget->IsCancelOnRelease())
						{
							OnInteractionFinished(BestInteractionTarget, EInteractionResult::Canceled);
						}
					}
				}
				else
				{
					CurrentHoldTime = 0.f;
					CurrentInteractionWidget->SetInteractionProgress(0.f);
					CurrentInteractionWidget->PlayInteractionCompletedAnimation(EInteractionResult::Canceled);
					OnInteractionFinished(BestInteractionTarget, EInteractionResult::Canceled);
				}
			}
		}
		break;
	case EInteractionType::Repeat:
		if (OwnerController->WasInputKeyJustPressed(LastPressedKey))
		{
			if (OwnerController->WasInputKeyJustPressed(LastPressedKey))
			{
				RepeatCoolTime = BestInteractionTarget->GetRepeatCooldown();

				if (RepeatedTime == 0)
				{
					CurrentInteractionWidget->UpdateInteractionText(false, EInteractionState::Interacting);
					OnInteractionBegin_Event();
				}

				RepeatedTime++;

				const int TargetRepeatedTime = BestInteractionTarget->GetRepeatedTime();

				const int Alpha = FMath::Min(
					1.f, FMath::Clamp(RepeatedTime, 0, TargetRepeatedTime) / (TargetRepeatedTime > 0
						                                                          ? TargetRepeatedTime
						                                                          : 1));

				BestInteractionTarget->OnInteractionUpdated.Broadcast(Alpha, RepeatedTime, OwnerController->GetPawn());

				CurrentInteractionWidget->SetInteractionProgress(Alpha);

				if (RepeatedTime == BestInteractionTarget->GetRepeatedTime())
				{
					RepeatedTime = 0;
					CurrentInteractionWidget->PlayInteractionCompletedAnimation(EInteractionResult::Completed);
					OnInteractionFinished(BestInteractionTarget, EInteractionResult::Completed);
				}
			}
		}

		if (BestInteractionTarget)
		{
			if (RepeatedTime != 0 && BestInteractionTarget->IsCooldownEnabled())
			{
				RepeatCoolTime = FMath::Max(0, RepeatCoolTime - GetWorld()->GetDeltaSeconds());

				if (RepeatCoolTime == 0.f)
				{
				}
				else
				{
				}
			}
		}
		break;
	default:
		break;
	}

	if (OwnerController->WasInputKeyJustPressed(LastPressedKey))
	{
		bKeyJustPressed = true;
	}

	if (OwnerController->WasInputKeyJustReleased(LastPressedKey))
	{
		bKeyJustPressed = false;
	}
}

TArray<FKey> UInteractionComponent::GetInteractionKeys()
{
	if (BestInteractionTarget)
	{
		if (BestInteractionTarget->IsUseCustomKeys())
		{
			return BestInteractionTarget->GetCustomKeys();
		}
	}
	return InteractionKeys;
}

bool UInteractionComponent::IsInteractable(UInteraction_TargetActor* ItemToFind) const
{
	return (ItemToFind->IsInteractionEnabled() && (!PendingTargets.Contains(ItemToFind) && !DeactivatedTargets.
		Contains(ItemToFind)));
}

void UInteractionComponent::UpdateInteractionKeys()
{
	if (OwnerController->IsLocalController())
	{
		if (const UEnhancedInputLocalPlayerSubsystem* SubSystem = ULocalPlayer::GetSubsystem<
			UEnhancedInputLocalPlayerSubsystem>(
			OwnerController->GetLocalPlayer()))
		{
			InteractionKeys = SubSystem->QueryKeysMappedToAction(InteractionInputAction);
			GetWorld()->GetTimerManager().ClearTimer(BeginUpdateKeys_TimerHandle);
		}
	}
}

void UInteractionComponent::OnInteractionTargetUpdated_Server(const bool bAdd,
                                                              UInteraction_TargetActor* InteractionTarget)
{
	if (bAdd)
	{
		if (IsInteractable(InteractionTarget))
		{
			OnInteractionTargetUpdated_Client(bAdd, InteractionTarget);
			InteractionTargets.AddUnique(InteractionTarget);
		}
	}
	else
	{
		InteractionTargets.Remove(InteractionTarget);
		OnInteractionTargetUpdated_Client(bAdd, InteractionTarget);
	}
}

void UInteractionComponent::AddToPendingTargets(UInteraction_TargetActor* InteractionTarget)
{
	PendingTargets.AddUnique(InteractionTarget);

	OnPointOfInterestUpdated_Server(false, InteractionTarget);
	OnInteractionTargetUpdated_Server(false, InteractionTarget);

	if (!PendingTarget_TimerHandle.IsValid())
	{
		GetWorld()->GetTimerManager().SetTimer(PendingTarget_TimerHandle, this, &ThisClass::CheckForPendingTargets,
		                                       PendingTargetCheckInterval, true);
	}
}

void UInteractionComponent::OnPointOfInterestUpdated_Server(const bool bAdd,
                                                            UInteraction_TargetActor* InteractionTarget)
{
	if (bAdd)
	{
		if (IsInteractable(InteractionTarget))
		{
			PointsOfInterests.AddUnique(InteractionTarget);

			AActor* TargetRef = InteractionTarget->GetTargetRef();
			check(TargetRef)
			TargetRef->OnDestroyed.AddDynamic(this, &ThisClass::OnInteractionTargetDestroyed);

			OnPointOfInterestUpdated_Client(true, InteractionTarget);
		}
		else
		{
			if (InteractionTarget->IsReactivationEnabled())
			{
				AddToPendingTargets(InteractionTarget);
			}
		}
	}
	else
	{
		check(InteractionTarget)

		PointsOfInterests.Remove(InteractionTarget);
		OnPointOfInterestUpdated_Client(false, InteractionTarget);
	}
}

void UInteractionComponent::OnInteractionTargetUpdated_Client(const bool bAdd,
                                                              const UInteraction_TargetActor* InteractionTarget)
{
	UInteraction_Indicator* IndicatorWidget = FindWidgetByInteractionTarget(InteractionTarget);
	if (IndicatorWidget)
	{
		IndicatorWidget->UpdateContentState(bAdd);
	}
}

UInteraction_Indicator* UInteractionComponent::FindEmptyWidget()
{
	for (UInteraction_Indicator* Widget : WidgetPool)
	{
		if (Widget)
		{
			return Widget;
		}
	}
	return nullptr;
}

void UInteractionComponent::OnPointOfInterestUpdated_Client(const bool bAdd,
                                                            UInteraction_TargetActor* InteractionTarget)
{
	if (bAdd)
	{
		InteractionTarget->UpdateWidgetInfo(WidgetScreenMargin, ScreenRadiusProgress);

		if (UInteraction_Indicator* EmptyWidget = FindEmptyWidget())
		{
			EmptyWidget->UpdateInteractionTarget(InteractionTarget);
		}
		else
		{
			UInteraction_Indicator* Indicator = CreateWidget<UInteraction_Indicator>(
				OwnerController, IndicatorWidgetClass);
			check(Indicator)

			WidgetPool.AddUnique(Indicator);
			Indicator->AddToPlayerScreen();
			Indicator->UpdateInteractionTarget(InteractionTarget);
		}
	}
	else
	{
		if (UInteraction_Indicator* Widget = FindWidgetByInteractionTarget(InteractionTarget))
		{
			Widget->UpdateInteractionTarget(nullptr);
		}
	}
}

void UInteractionComponent::OnInteractionTargetReactivated(UInteraction_TargetActor* InteractionTarget)
{
	PendingTargets.Remove(InteractionTarget);

	InteractionTarget->SetInteractionEnabled(true);

	TArray<UPrimitiveComponent*> PrimitiveComponents;
	OwnerController->GetPawn()->GetOverlappingComponents(PrimitiveComponents);

	if (InteractionTarget->IsInInnerZone(PrimitiveComponents))
	{
		OnPointOfInterestUpdated_Server(true, InteractionTarget);
		OnInteractionTargetUpdated_Server(true, InteractionTarget);
	}
	else if (InteractionTarget->IsInOuterZone(PrimitiveComponents))
	{
		OnPointOfInterestUpdated_Server(true, InteractionTarget);
	}

	InteractionTarget->OnInteractionReactivated.Broadcast(OwnerController->GetPawn());
}

void UInteractionComponent::OnInteractionTargetDestroyed(AActor* DestroyedActor)
{
	UInteraction_TargetActor* Interaction_TargetActor = Cast<UInteraction_TargetActor>(
		DestroyedActor->GetComponentByClass(UInteraction_TargetActor::StaticClass()));

	check(Interaction_TargetActor)

	InteractionTargets.Remove(Interaction_TargetActor);
	PointsOfInterests.Remove(Interaction_TargetActor);

	CurrentInteractionWidget->UpdateInteractionTarget(nullptr);

	DestroyedActor->OnDestroyed.Clear();
}

void UInteractionComponent::CheckForPendingTargets()
{
	if (!PendingTargets.IsEmpty())
	{
		for (UInteraction_TargetActor* Target : PendingTargets)
		{
			if (Target->IsPendingTimePassed())
			{
				OnInteractionTargetReactivated(Target);
			}
		}
	}
	else
	{
		GetWorld()->GetTimerManager().ClearTimer(PendingTarget_TimerHandle);
	}
}


void UInteractionComponent::AddToDeactivatedTargets(UInteraction_TargetActor* InteractionTarget)
{
	DeactivatedTargets.AddUnique(InteractionTarget);

	OnPointOfInterestUpdated_Server(false, InteractionTarget);
	OnInteractionTargetUpdated_Server(false, InteractionTarget);
	InteractionTarget->OnDeactivated();
}

void UInteractionComponent::OnInteractionFinished(UInteraction_TargetActor* Interaction_TargetActor,
                                                  const EInteractionResult Result)
{
	bIsInteracting = false;
	ResetData();

	Interaction_TargetActor->OnInteractionEnd.Broadcast(Result, OwnerController->GetPawn());

	switch (Result)
	{
	case EInteractionResult::Completed:
		switch (Interaction_TargetActor->GetFinishMethod())
		{
		case EInteractionFinishMethod::DestroyOnComplete:
		case EInteractionFinishMethod::DestroyOnCompleteOrCancel:
		case EInteractionFinishMethod::ReactivateOnCancel_DestroyOnComplete:
		case EInteractionFinishMethod::DeactivateOnCanceledDestroyOnComplete:
			Interaction_TargetActor->GetTargetRef()->Destroy();
			break;
		case EInteractionFinishMethod::ReactivateOnComplete:
		case EInteractionFinishMethod::ReactivateOnCompleteOrCancel:
		case EInteractionFinishMethod::ReactivateOnCompleteDestroyOnCancel:
		case EInteractionFinishMethod::ReactivateOnCompleteDeactivateOnCancel:
		case EInteractionFinishMethod::DeactivateOnCanceledReactivateAfterDurationOnComplete:
			Interaction_TargetActor->OnAddedToPendingTarget();
			break;
		case EInteractionFinishMethod::ReactivateOnCancelDeactivateOnComplete:
		case EInteractionFinishMethod::DeactivateOnComplete:
		case EInteractionFinishMethod::DeactivateOnCompleteOrCancel:
		case EInteractionFinishMethod::DeactivateOnCompletedDestroyOnCancel:
		case EInteractionFinishMethod::DeactivateOnCompletedReactivateOnCancel:
			AddToDeactivatedTargets(Interaction_TargetActor);
			break;
		default:
			break;
		}
		break;
	case EInteractionResult::Canceled:
		switch (Interaction_TargetActor->GetFinishMethod())
		{
		case EInteractionFinishMethod::DestroyOnCancel:
		case EInteractionFinishMethod::DestroyOnCompleteOrCancel:
		case EInteractionFinishMethod::DeactivateOnCompletedDestroyOnCancel:
		case EInteractionFinishMethod::ReactivateOnCompleteDestroyOnCancel:
			Interaction_TargetActor->GetTargetRef()->Destroy();
			break;
		case EInteractionFinishMethod::ReactivateOnCancel:
		case EInteractionFinishMethod::ReactivateOnCompleteOrCancel:
		case EInteractionFinishMethod::ReactivateOnCancelDestroyOnComplete:
		case EInteractionFinishMethod::ReactivateOnCancelDeactivateOnComplete:
		case EInteractionFinishMethod::DeactivateOnCompletedReactivateOnCancel:
			Interaction_TargetActor->OnAddedToPendingTarget();
			break;
		case EInteractionFinishMethod::DeactivateOnComplete:
		case EInteractionFinishMethod::DeactivateOnCancel:
		case EInteractionFinishMethod::DeactivateOnCompleteOrCancel:
		case EInteractionFinishMethod::DeactivateOnCanceledDestroyOnComplete:
		case EInteractionFinishMethod::DeactivateOnCanceledReactivateAfterDurationOnComplete:
			AddToDeactivatedTargets(Interaction_TargetActor);
			break;
		default:
			break;
		}
		break;
	default:
		break;
	}
}

UInteraction_TargetActor* UInteractionComponent::FindBestInteractableTarget()
{
	float TempDotProduct = 0.f;
	UInteraction_TargetActor* NewTargetActor = nullptr;

	for (UInteraction_TargetActor* InteractableTarget : InteractionTargets)
	{
		if (InteractableTarget->IsInteractionEnabled())
		{
			FVector MarkerLocation = InteractableTarget->GetMarkerTargetComponent()
			                                           ->GetComponentLocation();
			FRotator MarkerRotation = InteractableTarget->GetMarkerTargetComponent()
			                                            ->GetComponentRotation();
			FVector ActorOrigin = MarkerLocation + MarkerRotation.RotateVector(
				InteractableTarget->GetMarkerOffset());


			FVector CamLookAtVec = (ActorOrigin -
				OwnerController->PlayerCameraManager->GetCameraLocation()).GetSafeNormal();

			FVector CamForVec = OwnerController->PlayerCameraManager->GetActorForwardVector();

			const float CamToTargetDot = FVector::DotProduct(CamLookAtVec, CamForVec);

			if (CamToTargetDot > .5f && CamToTargetDot > TempDotProduct)
			{
				// InteractableTarget->GetOwner()->WasRecentlyRendered(.2f)
				TempDotProduct = CamToTargetDot;
				NewTargetActor = InteractableTarget;
			}
		}
	}
	return NewTargetActor;
}


void UInteractionComponent::UpdateBestInteractable(UInteraction_TargetActor* NewTarget)
{
	if (!NewTarget)
	{
		if (BestInteractionTarget)
		{
			SetNewTarget(BestInteractionTarget, false);

			if (bIsInteracting)
			{
				bIsInteracting = false;
				BestInteractionTarget->OnInteractionEnd.Broadcast(
					EInteractionResult::Canceled,
					OwnerController->GetPawn());
				ResetData();
			}
			BestInteractionTarget = nullptr;
			SetNewTarget(nullptr, false);
		}
	}
	else
	{
		if (NewTarget != BestInteractionTarget)
		{
			if (BestInteractionTarget)
			{
				SetNewTarget(BestInteractionTarget, false);
				if (bIsInteracting)
				{
					bIsInteracting = false;
					BestInteractionTarget->OnInteractionEnd.Broadcast(
						EInteractionResult::Canceled,
						OwnerController->GetPawn());
					ResetData();
				}
			}
			BestInteractionTarget = NewTarget;
			SetNewTarget(BestInteractionTarget, true);
		}
	}

	if (BestInteractionTarget)
	{
		TryInteraction();
	}
}

void UInteractionComponent::SetNewTarget(UInteraction_TargetActor* NewTarget,
                                         const bool bSelected)
{
	BestInteractionTarget = NewTarget;

	if (bSelected)
	{
		BestInteractionTarget->SetHighlight(true);

		CurrentInteractionWidget = FindWidgetByInteractionTarget(BestInteractionTarget);
		if (!CurrentInteractionWidget)
		{
			CurrentInteractionWidget = FindEmptyWidget();
			CurrentInteractionWidget->UpdateInteractionTarget(BestInteractionTarget);
		}
		check(CurrentInteractionWidget);
		CurrentInteractionWidget->SetInteractionKeyText(GetInteractionKeys().IsEmpty()
			                                                ? FKey()
			                                                : GetInteractionKeys()[0]);
		CurrentInteractionWidget->OnWidgetNewTarget(true);
	}
	else
	{
		if (CurrentInteractionWidget)
		{
			CurrentInteractionWidget->OnWidgetNewTarget(false);
			CurrentInteractionWidget = nullptr;

			BestInteractionTarget->SetHighlight(false);
		}
	}
}

void UInteractionComponent::ResetData()
{
	bKeyJustPressed = false;
	LastPressedKey = FKey("None");
	CurrentHoldTime = 0.f;
	RepeatedTime = 0;
}

UInteraction_Indicator* UInteractionComponent::FindWidgetByInteractionTarget(
	const UInteraction_TargetActor* InteractionTarget)
{
	for (UInteraction_Indicator* Widget : WidgetPool)
	{
		if (Widget->GetWidgetInteractionTarget() == InteractionTarget)
		{
			return Widget;
		}
	}
	return nullptr;
}

void UInteractionComponent::DebugTrace() const
{
	if (!bDebugMode)
		return;

	UE_LOG(LogTemp, Display,
	       TEXT("Interaction Targets : %d\nPoints of Interests : %d\nPending Targets : %d\nMarker : %s"),
	       InteractionTargets.Num(), PointsOfInterests.Num(), PendingTargets.Num(),
	       *CurrentInteractionWidget->GetName());
}
