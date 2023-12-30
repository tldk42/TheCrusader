// Fill out your copyright notice in the Description page of Project Settings.

#include "Component/Interaction_TargetActor.h"

#include "Component/InteractionComponent.h"
#include "Components/SphereComponent.h"
#include "Kismet/KismetSystemLibrary.h"

void UInteraction_TargetActor::BeginPlay()
{
	Super::BeginPlay();

	TargetRef = GetOwner();
	check(TargetRef);

	InitNotifyZones();
	InitHighlightedComponents();
	SelectMarkerComponent(MarkerTargetName);

	OnInteractionBegin.AddDynamic(this, &ThisClass::OnInteractionBeginEvent);
	OnInteractionEnd.AddDynamic(this, &ThisClass::OnInteractionEndEvent);
}

bool UInteraction_TargetActor::IsInteractionEnabled() const
{
	return bInteractionEnabled;
}

bool UInteraction_TargetActor::IsReactivationEnabled() const
{
	switch (InteractionFinishMethod)
	{
	case EInteractionFinishMethod::ReactivateOnComplete:
	case EInteractionFinishMethod::ReactivateOnCancel:
	case EInteractionFinishMethod::ReactivateOnCompleteOrCancel:
	case EInteractionFinishMethod::ReactivateOnCancel_DestroyOnComplete:
	case EInteractionFinishMethod::ReactivateOnCancelDestroyOnComplete:
	case EInteractionFinishMethod::ReactivateOnCancelDeactivateOnComplete:
	case EInteractionFinishMethod::ReactivateOnCompleteDestroyOnCancel:
	case EInteractionFinishMethod::ReactivateOnCompleteDeactivateOnCancel:
	case EInteractionFinishMethod::DeactivateOnComplete:
	case EInteractionFinishMethod::DeactivateOnCancel:
	case EInteractionFinishMethod::DeactivateOnCompleteOrCancel:
	case EInteractionFinishMethod::DeactivateOnCanceledDestroyOnComplete:
	case EInteractionFinishMethod::DeactivateOnCanceledReactivateAfterDurationOnComplete:
	case EInteractionFinishMethod::DeactivateOnCompletedDestroyOnCancel:
	case EInteractionFinishMethod::DeactivateOnCompletedReactivateOnCancel:
		return true;
	default: ;
	}
	return false;
}

bool UInteraction_TargetActor::IsCancelOnRelease() const
{
	switch (InteractionFinishMethod)
	{
	case EInteractionFinishMethod::DestroyOnCancel:
	case EInteractionFinishMethod::DestroyOnCompleteOrCancel:
	case EInteractionFinishMethod::ReactivateOnCancel:
	case EInteractionFinishMethod::ReactivateOnCompleteOrCancel:
	case EInteractionFinishMethod::ReactivateOnCancel_DestroyOnComplete:
	case EInteractionFinishMethod::ReactivateOnCancelDestroyOnComplete:
	case EInteractionFinishMethod::ReactivateOnCancelDeactivateOnComplete:
	case EInteractionFinishMethod::ReactivateOnCompleteDestroyOnCancel:
	case EInteractionFinishMethod::ReactivateOnCompleteDeactivateOnCancel:
	case EInteractionFinishMethod::DeactivateOnCancel:
	case EInteractionFinishMethod::DeactivateOnCompleteOrCancel:
	case EInteractionFinishMethod::DeactivateOnCanceledDestroyOnComplete:
	case EInteractionFinishMethod::DeactivateOnCanceledReactivateAfterDurationOnComplete:
	case EInteractionFinishMethod::DeactivateOnCompletedDestroyOnCancel:
	case EInteractionFinishMethod::DeactivateOnCompletedReactivateOnCancel:
		return true;
	default: ;
	}
	return false;
}

bool UInteraction_TargetActor::IsCooldownEnabled() const
{
	return bCooldownEnabled;
}

bool UInteraction_TargetActor::IsUseCustomKeys() const
{
	return bUseCustomKey;
}

bool UInteraction_TargetActor::IsPendingTimePassed() const
{
	return FMath::Abs(UKismetSystemLibrary::GetGameTimeInSeconds(GetWorld()) - LastInteractionTime) >=
		ReactivateDuration;
}

bool UInteraction_TargetActor::IsInInnerZone(const TArray<UPrimitiveComponent*>& OverlappedComponents) const
{
	if (OverlappedComponents.Contains(InnerNotifyZone))
	{
		return true;
	}
	return false;
}

bool UInteraction_TargetActor::IsInOuterZone(const TArray<UPrimitiveComponent*>& OverlappedComponents) const
{
	if (OverlappedComponents.Contains(OuterNotifyZone))
	{
		return true;
	}
	return false;
}


float UInteraction_TargetActor::GetHoldSeconds() const
{
	return TimeToHold;
}

float UInteraction_TargetActor::GetRepeatCooldown() const
{
	return RepeatCooldown;
}

int UInteraction_TargetActor::GetRepeatedTime() const
{
	return RepeatCount;
}

AActor* UInteraction_TargetActor::GetTargetRef() const
{
	return TargetRef;
}

void UInteraction_TargetActor::AssignInteractor(AController* InController)
{
	AssignedInteractor = InController;
}

void UInteraction_TargetActor::EnableInteraction(const bool bEnable)
{
	bInteractionEnabled = bEnable;

	// TODO: Interaction Manager Handle
}

void UInteraction_TargetActor::SetInteractionEnabled(const bool bNewInteractionEnabled)
{
	bInteractionEnabled = bNewInteractionEnabled;
}

USceneComponent* UInteraction_TargetActor::GetMarkerTargetComponent() const
{
	return MarkerTargetComponent;
}

FVector UInteraction_TargetActor::GetMarkerOffset() const
{
	return MarkerOffset;
}

TArray<FKey> UInteraction_TargetActor::GetCustomKeys() const
{
	return CustomKeys;
}

EInteractionType UInteraction_TargetActor::GetInteractionType() const
{
	return InteractionType;
}

EInteractionFinishMethod UInteraction_TargetActor::GetFinishMethod() const
{
	return InteractionFinishMethod;
}

void UInteraction_TargetActor::InitNotifyZones()
{
	TArray<USceneComponent*> SceneComponents;
	GetOwner()->GetComponents<USceneComponent>(SceneComponents);

	USceneComponent* ComponentToAttach = nullptr;

	for (USceneComponent* SceneComponent : SceneComponents)
	{
		if (SceneComponent->GetName().Equals(AttachingComponentName))
		{
			ComponentToAttach = SceneComponent;
			break;
		}
	}

	InnerNotifyZone = NewObject<USphereComponent>(
		GetOwner(),
		TEXT("InnerNotifyRadius"));

	OuterNotifyZone = NewObject<USphereComponent>(
		GetOwner(),
		TEXT("OuterNotifyRadius"));

	check(InnerNotifyZone);
	check(OuterNotifyZone);


	InnerNotifyZone->SetCollisionResponseToAllChannels(ECR_Ignore);
	InnerNotifyZone->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
	OuterNotifyZone->SetCollisionResponseToAllChannels(ECR_Ignore);
	OuterNotifyZone->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);

	InnerNotifyZone->InitSphereRadius(InnerZoneRadius);
	OuterNotifyZone->InitSphereRadius(InnerZoneRadius + FMath::Max(OuterZoneExtent, 10.f));

	InnerNotifyZone->SetHiddenInGame(!bDebugMode);
	OuterNotifyZone->SetHiddenInGame(!bDebugMode);

	GetOwner()->AddComponent(TEXT("InnerNotify"), false, FTransform::Identity, InnerNotifyZone);

	if (ComponentToAttach)
	{
		InnerNotifyZone->SetupAttachment(ComponentToAttach);
		OuterNotifyZone->SetupAttachment(InnerNotifyZone);
		InnerNotifyZone->RegisterComponent();
		OuterNotifyZone->RegisterComponent();
	}


	InnerNotifyZone->OnComponentBeginOverlap.AddDynamic(this,
	                                                    &ThisClass::OnInnerZoneBeginOverlap);
	InnerNotifyZone->OnComponentEndOverlap.AddDynamic(this, &ThisClass::OnInnerZoneEndOverlap);

	OuterNotifyZone->OnComponentBeginOverlap.AddDynamic(this,
	                                                    &ThisClass::OnOuterZoneBeginOverlap);
	OuterNotifyZone->OnComponentEndOverlap.AddDynamic(this, &ThisClass::OnOuterZoneEndOverlap);
}

void UInteraction_TargetActor::InitHighlightedComponents()
{
	USceneComponent* OwnerRootComp = GetOwner()->GetRootComponent();

	check(OwnerRootComp);

	if (UPrimitiveComponent* PrimitiveComponent = Cast<UPrimitiveComponent>(OwnerRootComp))
	{
		if (ComponentsToHighlights.Contains(PrimitiveComponent->GetName()))
		{
			HighlightedComponents.AddUnique(PrimitiveComponent);
		}
	}

	TArray<USceneComponent*> ChildComponents;
	OwnerRootComp->GetChildrenComponents(true, ChildComponents);

	for (USceneComponent* ChildComponent : ChildComponents)
	{
		if (UPrimitiveComponent* PrimitiveComponent = Cast<UPrimitiveComponent>(ChildComponent))
		{
			if (ComponentsToHighlights.Contains(PrimitiveComponent->GetName()))
			{
				HighlightedComponents.AddUnique(PrimitiveComponent);
			}
		}
	}
}

#pragma region Overlap Event

void UInteraction_TargetActor::OnInnerZoneBeginOverlap(UPrimitiveComponent* OverlappedComp,
                                                       AActor* OtherActor,
                                                       UPrimitiveComponent* OtherComp,
                                                       int32 OtherBodyIndex, bool bFromSweep,
                                                       const FHitResult& SweepResult)
{
	check(OtherActor);

	if (OtherActor == TargetRef)
		return;

	const APawn* Pawn = Cast<APawn>(OtherActor);
	check(Pawn)
	if (AController* Controller = Pawn->GetController())
	{
		if (UInteractionComponent* InteractionComponent = UInteractionComponent::GetInteractionComponent(Controller))
		{
			InteractionComponent->OnInteractionTargetUpdated_Server(true, this);
			AssignInteractor(Controller);
		}
	}
}

void UInteraction_TargetActor::OnInnerZoneEndOverlap(UPrimitiveComponent* OverlappedComp,
                                                     AActor* OtherActor,
                                                     UPrimitiveComponent* OtherComp,
                                                     int32 OtherBodyIndex)
{
	check(OtherActor);

	if (OtherActor == TargetRef)
		return;

	const APawn* Pawn = Cast<APawn>(OtherActor);
	check(Pawn)
	const AController* Controller = Pawn->GetController();

	if (Controller)
		if (UInteractionComponent* InteractionComponent = UInteractionComponent::GetInteractionComponent(Controller))
		{
			InteractionComponent->OnInteractionTargetUpdated_Server(false, this);
		}
}

void UInteraction_TargetActor::OnOuterZoneBeginOverlap(UPrimitiveComponent* OverlappedComp,
                                                       AActor* OtherActor,
                                                       UPrimitiveComponent* OtherComp,
                                                       int32 OtherBodyIndex, bool bFromSweep,
                                                       const FHitResult& SweepResult)
{
	if (!OtherActor || OtherActor == TargetRef)
		return;

	const APawn* Pawn = Cast<APawn>(OtherActor);
	check(Pawn)

	if (const AController* Controller = Pawn->GetController())
	{
		if (UInteractionComponent* InteractionComponent = UInteractionComponent::GetInteractionComponent(Controller))
		{
			InteractionComponent->OnPointOfInterestUpdated_Server(true, this);
		}
	}
}

void UInteraction_TargetActor::OnOuterZoneEndOverlap(UPrimitiveComponent* OverlappedComponent,
                                                     AActor* OtherActor,
                                                     UPrimitiveComponent* OtherComp,
                                                     int32 OtherBodyIndex)
{
	check(OtherActor);

	if (OtherActor == TargetRef)
		return;

	const APawn* Pawn = Cast<APawn>(OtherActor);
	check(Pawn)

	if (const AController* Controller = Pawn->GetController())
		if (UInteractionComponent* InteractionComponent = UInteractionComponent::GetInteractionComponent(Controller))
		{
			InteractionComponent->OnPointOfInterestUpdated_Server(false, this);
			AssignInteractor(nullptr);
		}
}


#pragma endregion Overlap Event

#pragma region Event Binding

void UInteraction_TargetActor::OnDeactivated() const
{
	InnerNotifyZone->SetGenerateOverlapEvents(false);
	OuterNotifyZone->SetGenerateOverlapEvents(false);

	InnerNotifyZone->SetHiddenInGame(true);
	OuterNotifyZone->SetHiddenInGame(true);
}

void UInteraction_TargetActor::OnAddedToPendingTarget()
{
	LastInteractionTime = UKismetSystemLibrary::GetGameTimeInSeconds(GetWorld());
	UInteractionComponent* InteractionComponent = UInteractionComponent::GetInteractionComponent(AssignedInteractor);
	check(InteractionComponent)

	InteractionComponent->AddToPendingTargets(this);
	// for (const AController* Controller : AssignedInteractors)
	// {
	// 	// TODO: Controller의 Interaction System
	// 	UInteractionComponent* InteractionComponent = UInteractionComponent::GetInteractionComponent(Controller);
	// 	check(InteractionComponent)
	//
	// 	InteractionComponent->AddToPendingTargets(this);
	// }

	bInteractionEnabled = false;
}

void UInteraction_TargetActor::OnInteractionBeginEvent(const APawn* InPawn)
{
	// 서버처리
	for (const AController* AssignedController : AssignedInteractors)
	{
		if (AssignedController->GetPawn() != InPawn)
		{
			if (AssignedController->GetComponentByClass(UInteractionComponent::StaticClass()))
			{
				// InteractionComponent->AddToDeactivatedTargets(this);
			}
		}
	}
}

void UInteraction_TargetActor::OnInteractionEndEvent(const EInteractionResult Result,
                                                     const APawn* InPawn)
{
	// 서버처리
	for (const AController* AssignedController : AssignedInteractors)
	{
		if (AssignedController->GetPawn() != InPawn)
		{
			if (AssignedController->GetComponentByClass(UInteractionComponent::StaticClass()))
			{
				// InteractionComponent->AddToDeactivatedTargets(this);
			}
		}
	}
}

#pragma endregion Event Binding

#pragma region UI

void UInteraction_TargetActor::UpdateInteractionWidget(const FMargin& Margin,
                                                       const float NewScreenRadiusProgress)
{
	WidgetMargin = Margin;
	ScreenRadiusProgress = NewScreenRadiusProgress;
}

void UInteraction_TargetActor::SelectMarkerComponent(const FString& InComponentName)
{
	USceneComponent* OwnerRootComp = GetOwner()->GetRootComponent();

	check(OwnerRootComp);

	TArray<USceneComponent*> ChildrenComponents;
	OwnerRootComp->GetChildrenComponents(true, ChildrenComponents);
	ChildrenComponents.Add(OwnerRootComp);

	for (USceneComponent* ChildrenComponent : ChildrenComponents)
	{
		if (MarkerTargetName == ChildrenComponent->GetName())
		{
			MarkerTargetComponent = ChildrenComponent;
			break;
		}
	}

	if (!MarkerTargetComponent)
	{
		MarkerTargetComponent = OwnerRootComp;
	}
}

void UInteraction_TargetActor::SetHighlight(const bool InHighlight)
{
	for (UPrimitiveComponent* Element : HighlightedComponents)
	{
		Element->SetRenderCustomDepth(InHighlight);
	}
}

void UInteraction_TargetActor::UpdateWidgetInfo(const FMargin WidgetScreenMargin, const float InScreenRadiusProgress)
{
	WidgetMargin = WidgetScreenMargin;
	ScreenRadiusProgress = InScreenRadiusProgress;
}

#pragma endregion UI
