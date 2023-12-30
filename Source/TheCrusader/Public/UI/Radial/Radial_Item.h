// Written by jaegang lim

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Data/UIDataStructs.h"
#include "Radial_Item.generated.h"

class UProgressBar;
class UDataTable;
class UTextBlock;
class UVerticalBox;
class UImage;
class UOverlay;
/**
 * 
 */
UCLASS()
class THECRUSADER_API URadial_Item : public UUserWidget
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	void Animate(bool bReverse);

	FORCEINLINE FRadialMenuItem GetItem() const { return Item; }
	FORCEINLINE TArray<URadial_Item*> GetItems() const { return Items; }

protected:
	UFUNCTION(BlueprintCallable, Category = "초기화", meta = (CompactNodeTitle = "초기화"))
	void InitializeItem();
	UFUNCTION(BlueprintCallable, Category = "초기화", meta = (CompactNodeTitle = "머티리얼 설정"))
	void SetMaterial();
	UFUNCTION(BlueprintCallable, Category = "초기화", meta = (CompactNodeTitle = "텍스트 설정"))
	void SetItemNameText() const;
	UFUNCTION(BlueprintCallable, Category = "초기화", meta = (CompactNodeTitle = "하위 아이템 조사"))
	void CheckForSubItems() const;
	UFUNCTION(BlueprintPure, BlueprintCallable, Category = "초기화", meta = (CompactNodeTitle = "위치 계산"))
	FVector2D CalculateTranslation(const int Index, const float SectorAngle, const float ItemTheta,
	                               const float ItemNum) const;
	UFUNCTION(BlueprintCallable, Category = "아이템", meta = (CompactNodeTitle = "하위 메뉴 표시"))
	void SetSubMenuIndicator() const;
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "아이템",
		meta = (CompactNodeTitle = "Create Items"))
	void CreateItems(const FRadialMenuData& Menu, const float SectorAngle, const float ItemTheta,
	                 const float ItemNum) const;

private:

protected:
#pragma region Widget

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget), Category = "Widget")
	UOverlay* MenuContainer_Overlay;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget), Category = "Widget")
	UVerticalBox* Container_VerticalBox;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget), Category = "Widget")
	UImage* Background_Image;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget), Category = "Widget")
	UImage* Icon_Image;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget), Category = "Widget")
	UProgressBar* CoolTime;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget), Category = "Widget")
	UTextBlock* ItemName_Text;

#pragma endregion Widget

#pragma region Settings

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (ExposeOnSpawn), Category = "Settings")
	UMaterialInstance* Material;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (ExposeOnSpawn), Category = "Settings")
	FRadialMenuItem Item;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (ExposeOnSpawn), Category = "Settings")
	float MenuSize;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (ExposeOnSpawn), Category = "Settings")
	float IconSize;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (ExposeOnSpawn), Category = "Settings")
	int FontSize;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (ExposeOnSpawn), Category = "Settings")
	float OuterRingIconSize;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (ExposeOnSpawn), Category = "Settings")
	float OuterRingDistanceFromCenter;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Settings")
	UTexture2D* DefaultIcon;
#pragma endregion Settings

#pragma region Material Data

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (ExposeOnSpawn), Category = "Material")
	float Sectors;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (ExposeOnSpawn), Category = "Material")
	float SectorIndex;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (ExposeOnSpawn), Category = "Material")
	UMaterialInstanceDynamic* BackgroundMaterial;

#pragma endregion Material Data

#pragma region Item

	UPROPERTY(BlueprintReadWrite, Category = "Item")
	TArray<URadial_Item*> Items;

#pragma endregion Item

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	UDataTable* DT_Menu;
};
