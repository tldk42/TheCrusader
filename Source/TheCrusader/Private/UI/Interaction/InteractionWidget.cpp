// Written by jaegang lim


#include "UI/Interaction/InteractionWidget.h"

#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"
#include "Interfaces/Interactable.h"

void UInteractionWidget::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	InteractionProgressbar->PercentDelegate.BindUFunction(this, "UpdateInteractionProgress");
}

void UInteractionWidget::NativeConstruct()
{
	Super::NativeConstruct();

	KeyPressText->SetText(FText::FromString("Press"));
	CurrentInteractionDuration = .0f;
}

void UInteractionWidget::UpdateWidget(const FInteractableData* InteractableData)
{
	InteractionProgressbar->SetVisibility(ESlateVisibility::Collapsed);

	switch (InteractableData->InteractableType)
	{
	case EInteractableType::Pickup:
		KeyPressText->SetText(FText::FromString("Press"));
		if (InteractableData->Quantity == 1)
		{
			QuantityText->SetVisibility(ESlateVisibility::Collapsed);
		}
		else
		{
			QuantityText->SetText(FText::Format(NSLOCTEXT("InteractionWidget", "Quantity", "x{0}"),
			                                    InteractableData->Quantity));
			QuantityText->SetVisibility(ESlateVisibility::Visible);
		}

		break;
	case EInteractableType::NonPlayerCharacter:
		QuantityText->SetVisibility(ESlateVisibility::Collapsed);
		break;
	case EInteractableType::Ride:
		KeyPressText->SetText(FText::FromString("Press"));
		InteractionProgressbar->SetVisibility(ESlateVisibility::Collapsed);
		break;
	case EInteractableType::Toggle:
		break;
	case EInteractableType::Container:
		break;
	default: ;
	}

	ActionText->SetText(InteractableData->Action);
	NameText->SetText(InteractableData->Name);
}

float UInteractionWidget::UpdateInteractionProgress()
{
	return 0.f;
}
