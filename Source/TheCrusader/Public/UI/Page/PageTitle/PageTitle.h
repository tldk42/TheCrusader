// Written by jaegang lim

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "PageTitle.generated.h"

class UMainMenu;
class UTextBlock;
class UButton;
/**
 * 
 */
UCLASS()
class THECRUSADER_API UPageTitle : public UUserWidget
{
	GENERATED_BODY()

protected:
	virtual void NativeConstruct() override;

public:
	UFUNCTION(BlueprintCallable)
	void SetAsActivePage() const;

public:
	UPROPERTY(BlueprintReadOnly)
	TObjectPtr<UMainMenu> ParentMenu;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget = true))
	UButton* PageButton;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget = true))
	UTextBlock* PageTitle;

	UPROPERTY(EditInstanceOnly, BlueprintReadWrite)
	FText PageText;

	UPROPERTY(EditInstanceOnly, BlueprintReadOnly)
	int32 PageIndex;

	UPROPERTY(EditInstanceOnly, BlueprintReadWrite)
	FSlateColor SelectedColor;

	UPROPERTY(BlueprintReadWrite)
	bool bActive;
};
