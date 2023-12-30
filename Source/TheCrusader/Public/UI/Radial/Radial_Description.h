// Written by jaegang lim

#pragma once

#include "CoreMinimal.h"
#include "RadialWidget.h"
#include "Blueprint/UserWidget.h"
#include "Radial_Description.generated.h"

class UTextBlock;
class USizeBox;
/**
 * 
 */
UCLASS()
class THECRUSADER_API URadial_Description : public UUserWidget
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable)
	void UpdateDescription(const FText& NewDescription) const;
	void InitializeDescription(const int DescriptionFontSize, const float DescriptionMaxWidth);

protected:
	virtual void NativeConstruct() override;

protected:
#pragma region Widget

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget), Category = "Widget")
	USizeBox* Container_SizeBox;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget), Category = "Widget")
	UTextBlock* Description_Text;

#pragma endregion Widget

#pragma region Settings

	UPROPERTY(BlueprintReadWrite, meta = (ExposeOnSpawn), Category = "Settings")
	int FontSize;

	UPROPERTY(BlueprintReadWrite, meta = (ExposeOnSpawn), Category = "Settings")
	int MaxWidth;
#pragma endregion Settings
};
