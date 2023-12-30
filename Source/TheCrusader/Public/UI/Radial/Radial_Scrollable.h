// Written by jaegang lim

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Radial_Scrollable.generated.h"

class UTextBlock;
class UHorizontalBox;
class UImage;
/**
 * 
 */
UCLASS()
class THECRUSADER_API URadial_Scrollable : public UUserWidget
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, meta = (CompactNodeTitle = "Initialize"))
	void InitializeScrollable(bool bInGamepad);
	
	UFUNCTION(BlueprintCallable, meta = (CompactNodeTitle = "UpdatePage"))
	void UpdatePage(const int CurrentPage, const int TotalPages) const;

	UFUNCTION(BlueprintCallable, meta = (CompactNodeTitle = "ToggleVisibility"))
	void ToggleVisibility(const bool bInGamepad) const;

protected:
#pragma region Widget

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	UTextBlock* Page_Text;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	UImage* MouseIcon_Image;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	UHorizontalBox* GamepadContainer_HorizontalBox;

#pragma endregion Widget

	UPROPERTY(BlueprintReadOnly, meta = (ExposeOnSpawn))
	bool bGamepad;

	UPROPERTY(BlueprintReadOnly)
	UMaterialInstanceDynamic* MouseScroll;
};
