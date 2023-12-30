// Written by jaegang lim

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Data/UIDataStructs.h"
#include "RadialWidget.generated.h"

class URadial_Scrollable;
class ATCPlayerController;
class URadial_Description;
class URadial_Item;
class UCanvasPanelSlot;
class UVerticalBox;
class UOverlay;

DECLARE_MULTICAST_DELEGATE_OneParam(FOnItemSelected, const FRadialMenuSendData&)
// DECLARE_MULTICAST_DELEGATE_OneParam(FOnSkillUpdated)

/**
 * 
 */
UCLASS()
class THECRUSADER_API URadialWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	FOnItemSelected ItemSelected;

public:
	UFUNCTION(BlueprintCallable, Category = "Menu", meta = (CompactNodeTitle = "ToggleMenu"))
	void ToggleMenu();

	UFUNCTION(BlueprintCallable, Category = "Menu", meta = (CompactNodeTitle = "HideMenu"))
	void HideMenu();

protected:
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

	UFUNCTION(BlueprintCallable, Category = "초기화", meta = (CompactNodeTitle = "초기화"))
	void InitializeMenu();


#pragma region Menu


	UFUNCTION(BlueprintCallable, Category = "Menu", meta = (CompactNodeTitle = "PopulateMenu"))
	void PopulateMenu(const int FirstIndex, const int LastIndex);

	UFUNCTION(BlueprintCallable, Category = "Menu", meta = (CompactNodeTitle = "CreateScrollableWidget"))
	void ScrollMenu(const float ScrollDelta);

	UFUNCTION(BlueprintCallable, Category = "Menu", meta = (CompactNodeTitle = "UpdateDescription"))
	void UpdateDescription(bool bClear);

	UFUNCTION(BlueprintCallable, Category = "Menu", meta = (CompactNodeTitle = "CalculateSectorAngle"))
	void CalculateSectorAngle();

	UFUNCTION(BlueprintCallable, Category = "Menu", meta = (CompactNodeTitle = "CalculateOuterRingAngles"))
	void CalculateOuterRingAngles();

	UFUNCTION(BlueprintCallable, Category = "Menu", meta = (CompactNodeTitle = "CalculateGapIndexes"))
	void CalculateGapIndexes(const int LastIteratedIndex, int& FirstIndex, int& LastIndex) const;
#pragma endregion Menu

#pragma region Item

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "아이템",
		meta = (CompactNodeTitle = "CreateItems"))
	void CreateItems(const int ItemIndex, bool bEmpty);

	UFUNCTION(BlueprintCallable, Category = "아이템", meta = (CompactNodeTitle = "ClearItems"))
	void ClearItems();

	UFUNCTION(BlueprintCallable, Category = "아이템", meta = (CompactNodeTitle = "ModifyOrAddItems"))
	void ModifyOrAddItems();

	UFUNCTION(BlueprintCallable, Category = "아이템", meta = (CompactNodeTitle = "GetPosition"))
	FVector2D GetPosition(const int ItemIndex) const;

	UFUNCTION(BlueprintCallable, Category = "아이템", meta = (CompactNodeTitle = "GetSelectedItemType"))
	ERadialMenu_ItemType GetSelectedItemType() const;

	UFUNCTION(BlueprintCallable, Category = "아이템", meta = (CompactNodeTitle = "GetSelectedItemData"))
	FText GetSelectedItemData();

	UFUNCTION(BlueprintCallable, Category = "아이템", meta = (CompactNodeTitle = "GetSelectedItemStruct"))
	FRadialMenuItem GetSelectedItemStruct();

	UFUNCTION(BlueprintCallable, Category = "아이템", meta = (CompactNodeTitle = "GetSelectedItemWidget"))
	URadial_Item* GetSelectedItemWidget();

	UFUNCTION(BlueprintCallable, Category = "아이템", meta = (CompactNodeTitle = "GetSelectedRingItemData"))
	FText GetSelectedRingItemData() const;

#pragma endregion Item

#pragma region Mouse

	UFUNCTION(BlueprintCallable, Category = "마우스", meta = (CompactNodeTitle = "GetMousePosition"))
	FVector2D GetMousePosition() const;

	UFUNCTION(BlueprintCallable, Category = "마우스", meta = (CompactNodeTitle = "CenterMouse"))
	void CenterMouse();

	virtual FReply NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;

	virtual FReply NativeOnMouseWheel(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;

	virtual FReply NativeOnMouseMove(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;

#pragma endregion Mouse

#pragma region Selection

public:
	UFUNCTION(BlueprintCallable, Category = "Selection", meta = (CompactNodeTitle = "MakeSelection"))
	void MakeSelection();

protected:
	UFUNCTION(BlueprintCallable, Category = "Selection", meta = (CompactNodeTitle = "CheckForNewItemSelection"))
	bool CheckForNewItemSelection(const FVector2D& Vector, const int Item) const;

	UFUNCTION(BlueprintCallable, Category = "Selection", meta = (CompactNodeTitle = "CheckForNewRingItemSelection"))
	static bool CheckForNewRingItemSelection(const FVector2D& Vector, const float EdgeThreshold,
	                                         const UUserWidget* RingItem);

	UFUNCTION(BlueprintCallable, Category = "Selection", meta = (CompactNodeTitle = "CheckInnerItemHovered"))
	bool CheckInnerItemHovered(const int ItemIndex, const FVector2D& Vector, int& NewSelectedIndex) const;

	UFUNCTION(BlueprintCallable, Category = "Selection", meta = (CompactNodeTitle = "CheckOuterRingHovered"))
	void CheckOuterRingHovered(const FVector2D& Vector);

	UFUNCTION(BlueprintCallable, Category = "Selection", meta = (CompactNodeTitle = "GetOuterItemEdgeThreshold"))
	float GetOuterItemEdgeThreshold(const float Num) const;

	UFUNCTION(BlueprintCallable, Category = "Selection", meta = (CompactNodeTitle = "GetSelection"))
	void GetSelection();


	UFUNCTION(BlueprintCallable, Category = "Selection", meta = (CompactNodeTitle = "SetMaterialSelection"))
	void SetMaterialSelection(const int Selection) const;

	UFUNCTION(BlueprintCallable, Category = "Selection", meta = (CompactNodeTitle = "SelectValidIndex"))
	void SelectValidIndex();

	UFUNCTION(BlueprintCallable, Category = "Selection", meta = (CompactNodeTitle = "SelectValidIndex"))
	void SelectionMade(const FText& DisplayName);

#pragma endregion Selection

#pragma region Player Control
	UFUNCTION(BlueprintCallable, Category = "Player", meta = (CompactNodeTitle = "HidePlayerUI"))
	void HidePlayerUI();

	UFUNCTION(BlueprintCallable, Category = "Player", meta = (CompactNodeTitle = "ShowPlayerUI"))
	void ShowPlayerUI();
#pragma endregion Player Control

#pragma region Helper

	UFUNCTION(BlueprintCallable, Category = "HelperFunctions")
	void PlayMenuSound(ERadialSound Sound);

#pragma endregion Helper

#pragma region Pagination

	UFUNCTION(BlueprintCallable, Category = "Pagination", meta = (CompactNodeTitle = "GetTotalPages"))
	int GetTotalPages(const int InItemsPerPage) const;

	UFUNCTION(BlueprintCallable, Category = "Pagination", meta = (CompactNodeTitle = "HidePlayerUI"))
	void HideScrollableUI() const;

	UFUNCTION(BlueprintCallable, Category = "Pagination", meta = (CompactNodeTitle = "ShowScrollableUI"))
	void ShowScrollableUI();

	UFUNCTION(BlueprintCallable, Category = "Pagination", meta = (CompactNodeTitle = "CreateScrollableWidget"))
	void CreateScrollableWidget();

	UFUNCTION(BlueprintCallable, Category = "Pagination", meta = (CompactNodeTitle = "GetPageToNavigateTo"))
	int GetPageToNavigateTo(const float Direction) const;

	UFUNCTION(BlueprintCallable, Category = "Pagination", meta = (CompactNodeTitle = "GetFirstLastIndex"))
	void GetFirstLastIndex(int& FirstIndex, int& LastIndex) const;

	UFUNCTION(BlueprintCallable, Category = "Pagination", meta = (CompactNodeTitle = "PreviousMenu"))
	void PreviousMenu();
	

#pragma endregion Pagination

protected:
#pragma region Widget

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget), Category = "Widget")
	UOverlay* MenuContainer_Overlay;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget), Category = "Widget")
	UOverlay* ItemsContainer_Overlay;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget), Category = "Widget")
	UOverlay* MenuCenterContainer_Overlay;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget), Category = "Widget")
	UVerticalBox* MenuCenterContainer_VerticalBox;

	UPROPERTY(BlueprintReadWrite, Category = "Widget")
	UCanvasPanelSlot* MenuContainer_Overlay_As_CanvasSlot;

	UPROPERTY(BlueprintReadWrite, Category = "Widget")
	TArray<URadial_Item*> MenuItemWidgets;

	UPROPERTY(BlueprintReadWrite, Category = "Widget")
	URadial_Scrollable* ScrollableWidget;

	UPROPERTY(BlueprintReadWrite, Category = "Widget")
	URadial_Item* RingItemWidget;

	UPROPERTY(BlueprintReadWrite, Category = "Widget")
	URadial_Description* DescriptionWidget;

	// UPROPERTY(BlueprintReadWrite, Category = "Widget")
	// TSubclassOf<URadial_Item> RadialItemWidgetClass;

	UPROPERTY(BlueprintReadWrite, Category = "Widget")
	TSubclassOf<URadial_Scrollable> ScrollableWidgetClass;

	UPROPERTY(BlueprintReadWrite, Category = "Widget")
	TSubclassOf<URadial_Description> DescriptionWidgetClass;

#pragma endregion Widget

#pragma region Menu

	UPROPERTY(BlueprintReadWrite, Category = "Menu")
	FRadialMenuData Menu;

	UPROPERTY(BlueprintReadWrite, Category = "Menu")
	bool bShowMenu;

#pragma endregion Menu

#pragma region Gamepad

	UPROPERTY(BlueprintReadWrite, Category = "GamepadSupport")
	bool bUsingGamepad;

	UPROPERTY(BlueprintReadWrite, Category = "GamepadSupport")
	FVector2D RightThumbStick;

#pragma endregion Gamepad

#pragma region Partition

	UPROPERTY(BlueprintReadWrite, Category = "Pagination")
	TArray<FName> MenuCrumbs;

	UPROPERTY(BlueprintReadWrite, Category = "Pagination")
	int TotalItems;

	UPROPERTY(BlueprintReadWrite, Category = "Pagination")
	int TotalPages;

	UPROPERTY(BlueprintReadWrite, Category = "Pagination")
	int CurrentPage;

	UPROPERTY(BlueprintReadWrite, Category = "Pagination")
	int ItemsPerPage;

#pragma endregion Partition

#pragma region Settings

	UPROPERTY(BlueprintReadWrite, Category = "Settings")
	FRadialMenuSetting RadialMenuSetting;

	UPROPERTY(BlueprintReadWrite, Category = "Settings")
	FVector2D MenuOffset;

	UPROPERTY(BlueprintReadWrite, Category = "Settings")
	UMaterialInstanceDynamic* MenuMaterial;

	UPROPERTY(BlueprintReadWrite, Category = "Settings")
	bool bKeepPreviousMenuState;

	UPROPERTY(BlueprintReadWrite, Category = "Settings")
	bool bCenterMouseToRadialMenu;

	UPROPERTY(BlueprintReadWrite, Category = "Settings")
	float DeadZoneTolerance;

	UPROPERTY(BlueprintReadWrite, Category = "Settings")
	float EdgeTolerance;

	UPROPERTY(BlueprintReadWrite, Category = "Settings")
	float SectorEdgeThreshold;

#pragma endregion Settings

#pragma region Player

	UPROPERTY(BlueprintReadOnly, Category = "Player")
	ATCPlayerController* PlayerController;

#pragma endregion Player

#pragma region Items

	UPROPERTY(BlueprintReadWrite, Category = "Items")
	int SelectedItem;

	UPROPERTY(BlueprintReadWrite, Category = "Items")
	float ItemTheta;

	UPROPERTY(BlueprintReadWrite, Category = "Items")
	float RingItemTheta;

#pragma endregion Items

#pragma region DataTable

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "DataTable")
	UDataTable* DT_RadialMenuSetting;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "DataTable")
	FName RadialMenuSetting_RowName;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "DataTable")
	UDataTable* DT_RadialMenu;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "DataTable")
	FName RadialMenu_RowName;

#pragma endregion DataTable

#pragma region Animation

	UPROPERTY(BlueprintReadWrite, meta = (BindWidgetAnim), Transient)
	UWidgetAnimation* Scale;

#pragma endregion Animation
};
