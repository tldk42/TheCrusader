// Written by jaegang lim

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "InteractionWidget.generated.h"

class UProgressBar;
class UTextBlock;
struct FInteractableData;
class ABalian;
/**
 * 
 */
UCLASS()
class THECRUSADER_API UInteractionWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	void UpdateWidget(const FInteractableData* InteractableData);

protected:
	virtual void NativeOnInitialized() override;
	virtual void NativeConstruct() override;

	UFUNCTION(Category = "Interaction Widget | Interactable Data")
	float UpdateInteractionProgress();

public:
	UPROPERTY(VisibleAnywhere, Category = "Interaction Widget | Player Reference")
	ABalian* PlayerRef;

protected:
	UPROPERTY(VisibleAnywhere, Category = "Interaction Widget | Interactable Data", meta = (BindWidget))
	UTextBlock* NameText;

	UPROPERTY(VisibleAnywhere, Category = "Interaction Widget | Interactable Data", meta = (BindWidget))
	UTextBlock* ActionText;

	UPROPERTY(VisibleAnywhere, Category = "Interaction Widget | Interactable Data", meta = (BindWidget))
	UTextBlock* QuantityText;

	UPROPERTY(VisibleAnywhere, Category = "Interaction Widget | Interactable Data", meta = (BindWidget))
	UTextBlock* KeyPressText;

	UPROPERTY(VisibleAnywhere, Category = "Interaction Widget | Interactable Data", meta = (BindWidget))
	UProgressBar* InteractionProgressbar;

	UPROPERTY(VisibleAnywhere, Category = "Interaction Widget | Interactable Data", meta = (BindWidget))
	float CurrentInteractionDuration;
};
