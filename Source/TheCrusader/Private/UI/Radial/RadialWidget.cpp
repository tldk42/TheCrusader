// Written by jaegang lim


#include "UI/Radial/RadialWidget.h"

#include "Blueprint/WidgetLayoutLibrary.h"
#include "Components/CanvasPanelSlot.h"
#include "Components/Overlay.h"
#include "Components/OverlaySlot.h"
#include "Components/VerticalBox.h"
#include "Components/VerticalBoxSlot.h"
#include "Kismet/KismetMathLibrary.h"
#include "Player/TCPlayerController.h"
#include "Sound/SoundCue.h"
#include "UI/Radial/Radial_Description.h"
#include "UI/Radial/Radial_Item.h"
#include "UI/Radial/Radial_Scrollable.h"

void URadialWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	if (bShowMenu)
	{
		GetSelection();
	}
}

void URadialWidget::InitializeMenu()
{
	RadialMenuSetting = *DT_RadialMenuSetting->FindRow<
		FRadialMenuSetting>(RadialMenuSetting_RowName, TEXT("Not Found"));

	bCenterMouseToRadialMenu = RadialMenuSetting.bCenterMouseToRadialMenu;
	SectorEdgeThreshold = RadialMenuSetting.SectorEdgeThreshold;
	DeadZoneTolerance = RadialMenuSetting.DeadZoneTolerance;

	PlayerController = Cast<ATCPlayerController>(GetOwningPlayer());

	MenuContainer_Overlay_As_CanvasSlot = UWidgetLayoutLibrary::SlotAsCanvasSlot(MenuContainer_Overlay);

	if (DescriptionWidgetClass)
	{
		DescriptionWidget = CreateWidget<URadial_Description>(this, DescriptionWidgetClass);
		DescriptionWidget->InitializeDescription(RadialMenuSetting.DescriptionFontSize,
		                                         RadialMenuSetting.DescriptionMaxWidth);
	}

	check(DescriptionWidget);

	UVerticalBoxSlot* VBSlot = MenuCenterContainer_VerticalBox->AddChildToVerticalBox(DescriptionWidget);
	VBSlot->SetVerticalAlignment(VAlign_Fill);
	VBSlot->SetHorizontalAlignment(HAlign_Fill);
}


#pragma region Menu

void URadialWidget::ToggleMenu()
{
	CurrentPage = 0;
	bShowMenu = true;

	MenuCrumbs.Empty();

	const FRadialMenuData* FoundData = DT_RadialMenu->FindRow<FRadialMenuData>(
		RadialMenu_RowName, TEXT("Not Found Radial Menu"));
	if (!FoundData)
	{
		UE_LOG(LogTemp, Error, TEXT("Toggle Failed"));
	}

	Menu = *FoundData;

	MenuCrumbs.Add(RadialMenu_RowName);

	SetVisibility(ESlateVisibility::Visible);
	PlayAnimation(Scale);

	PopulateMenu(0, Menu.ItemsPerPage - 1);

	ShowPlayerUI();

	PlayMenuSound(ERadialSound::OpenMenu);
}

void URadialWidget::PopulateMenu(const int FirstIndex, const int LastIndex)
{
	SelectedItem = -1;

	TotalItems = Menu.Items.Num();

	int TempLastIndex;

	if (Menu.Scrollable)
	{
		ItemsPerPage = Menu.ItemsPerPage;
		TotalPages = GetTotalPages(ItemsPerPage);

		TempLastIndex = LastIndex;

		ShowScrollableUI();
	}
	else
	{
		ItemsPerPage = TotalItems;

		TempLastIndex = ItemsPerPage - 1;

		HideScrollableUI();
	}

	ClearItems();

	CalculateSectorAngle();

	for (int i = FirstIndex; i <= TempLastIndex; ++i)
	{
		CreateItems(i, false);
	}

	int Index1;
	int Index2;
	CalculateGapIndexes(TempLastIndex + 1, Index1, Index2);

	for (int i = Index1; i <= Index2; ++i)
	{
		CreateItems(i, true);
	}
}

void URadialWidget::HideMenu()
{
	if (!IsVisible())
		return;

	bShowMenu = false;
	SelectedItem = -1;

	MenuCrumbs.Empty();
	CurrentPage = 0;

	PlayAnimationReverse(Scale);

	PlayMenuSound(ERadialSound::CloseMenu);

	HidePlayerUI();
}

void URadialWidget::ScrollMenu(const float ScrollDelta)
{
	if (ItemsPerPage >= Menu.Items.Num())
	{
		return;
	}

	int Page = GetPageToNavigateTo(ScrollDelta);
	if (CurrentPage != Page)
	{
		CurrentPage = Page;
	}

	int TempSelectedItem = SelectedItem;

	int FirstIndex;
	int LastIndex;
	GetFirstLastIndex(FirstIndex, LastIndex);
	PopulateMenu(FirstIndex, LastIndex);

	SelectedItem = TempSelectedItem;

	PlayMenuSound(ERadialSound::ScrollMenu);

	if (!MenuItemWidgets.IsValidIndex(SelectedItem))
	{
		SelectValidIndex();
	}

	MenuItemWidgets[SelectedItem]->Animate(false);
	UpdateDescription(false);
}

void URadialWidget::CalculateSectorAngle()
{
	ItemTheta = 360 / ItemsPerPage;

	EdgeTolerance = UKismetMathLibrary::DegCos(ItemTheta / 2) + SectorEdgeThreshold;
}

void URadialWidget::CalculateOuterRingAngles()
{
	if (!MenuItemWidgets.IsValidIndex(SelectedItem))
		return;

	const float OuterRingAngle = ItemTheta / MenuItemWidgets[SelectedItem]->GetItems().Num();

	RingItemTheta = OuterRingAngle;
}

void URadialWidget::CalculateGapIndexes(const int LastIteratedIndex, int& FirstIndex, int& LastIndex) const
{
	FirstIndex = LastIteratedIndex - ItemsPerPage * CurrentPage;
	LastIndex = ItemsPerPage - 1;
}

void URadialWidget::UpdateDescription(const bool bClear)
{
	if (const URadial_Item* Item = GetSelectedItemWidget())
	{
		FText NewText = Item->GetItem().Description;
		if (Item->GetItem().ButtonType == ERadialMenu_ButtonType::RingMenu && RingItemWidget)
		{
			NewText = RingItemWidget->GetItem().Description;
		}
		const FText UpdateText = bClear ? FText() : NewText;
		DescriptionWidget->UpdateDescription(UpdateText);
	}
}

#pragma endregion Menu

#pragma region Item

FVector2D URadialWidget::GetPosition(const int ItemIndex) const
{
	const float IndexOffset = ItemIndex * (360 / ItemsPerPage) - 90;

	const FVector2D TempVector(UKismetMathLibrary::DegCos(IndexOffset), UKismetMathLibrary::DegSin(IndexOffset));

	return TempVector * RadialMenuSetting.DistanceFromCenter;
}

ERadialMenu_ItemType URadialWidget::GetSelectedItemType() const
{
	if (MenuItemWidgets.IsValidIndex(SelectedItem))
	{
		if (MenuItemWidgets[SelectedItem]->GetItem().ButtonType == ERadialMenu_ButtonType::RingMenu && RingItemWidget)
		{
			return RingItemWidget->GetItem().ItemType;;
		}
		return MenuItemWidgets[SelectedItem]->GetItem().ItemType;
	}
	return ERadialMenu_ItemType::Interaction;
}

FText URadialWidget::GetSelectedItemData()
{
	if (MenuItemWidgets.IsValidIndex(SelectedItem))
	{
		if (MenuItemWidgets[SelectedItem]->GetItem().ButtonType == ERadialMenu_ButtonType::RingMenu && RingItemWidget)
		{
			return RingItemWidget->GetItem().DisplayName;;
		}
		return MenuItemWidgets[SelectedItem]->GetItem().DisplayName;
	}
	return FText();
}

FRadialMenuItem URadialWidget::GetSelectedItemStruct()
{
	if (MenuItemWidgets.IsValidIndex(SelectedItem))
	{
		return MenuItemWidgets[SelectedItem]->GetItem();
	}
	return FRadialMenuItem();
}

FText URadialWidget::GetSelectedRingItemData() const
{
	if (RingItemWidget)
	{
		return RingItemWidget->GetItem().DisplayName;
	}
	return FText();
}

URadial_Item* URadialWidget::GetSelectedItemWidget()
{
	if (!MenuItemWidgets.IsValidIndex(SelectedItem))
		return nullptr;
	return MenuItemWidgets[SelectedItem];
}

#pragma endregion Item

#pragma region Mouse

void URadialWidget::CenterMouse()
{
	const FVector2D CenteredPos = UWidgetLayoutLibrary::GetViewportSize(GetWorld()) *
		MenuContainer_Overlay_As_CanvasSlot->
		GetAlignment()
		+ MenuContainer_Overlay_As_CanvasSlot->GetPosition();

	PlayerController->SetMouseLocation(FMath::TruncToInt(CenteredPos.X), FMath::TruncToInt(CenteredPos.Y));
	MenuOffset = -MenuContainer_Overlay_As_CanvasSlot->GetPosition();
}

FVector2D URadialWidget::GetMousePosition() const
{
	FVector2D MousePos;
	float MouseX;
	float MouseY;
	PlayerController->GetMousePosition(MouseX, MouseY);

	MousePos.X = MouseX;
	MousePos.Y = MouseY;

	// 마우스 위치에서 화면 가운데 빼기
	MousePos = MousePos - (UWidgetLayoutLibrary::GetViewportSize(GetWorld()) / 2);

	MousePos += MenuOffset;
	MousePos.Y *= -1;

	return MousePos;
}


FReply URadialWidget::NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	if (bShowMenu)
	{
		if (InMouseEvent.IsMouseButtonDown(EKeys::LeftMouseButton))
		{
			MakeSelection();
			return FReply::Handled();
		}
	}
	return Super::NativeOnMouseButtonDown(InGeometry, InMouseEvent);
}

FReply URadialWidget::NativeOnMouseWheel(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	if (Menu.Scrollable)
	{
		ScrollMenu(-FMath::Clamp(InMouseEvent.GetWheelDelta(), -1.f, 1.f));
		return FReply::Handled();
	}

	return Super::NativeOnMouseWheel(InGeometry, InMouseEvent);
}

FReply URadialWidget::NativeOnMouseMove(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	if (bUsingGamepad)
	{
		if (InMouseEvent.GetCursorDelta().Length() > 2.f)
		{
			// UseGamepad();

			return FReply::Handled();
		}
	}

	return Super::NativeOnMouseMove(InGeometry, InMouseEvent);
}

#pragma endregion Mouse

#pragma region Selection

void URadialWidget::MakeSelection()
{
	if (bShowMenu)
	{
		if (MenuItemWidgets.IsValidIndex(SelectedItem))
		{
			PlayMenuSound(ERadialSound::SelectItem);

			FRadialMenuSendData SendData;
			SendData.ItemType = GetSelectedItemType();
			SendData.RowName = *GetSelectedItemData().ToString();
			const FName TempRow = *GetSelectedItemData().ToString().Replace(TEXT(" "), TEXT(""));

			FRadialMenuData* FoundData;

			switch (MenuItemWidgets[SelectedItem]->GetItem().ButtonType)
			{
			case ERadialMenu_ButtonType::Selectable:
				ItemSelected.Broadcast(SendData);
				HideMenu();
				break;
			case ERadialMenu_ButtonType::SubMenu:
				// Implements Submenu
				if (TempRow.IsEqual("Back"))
				{
					PreviousMenu();
				}
				else
				{
					FoundData = DT_RadialMenu->FindRow<FRadialMenuData>(TempRow, TEXT("Not Found"));
					if (FoundData)
					{
						Menu = *FoundData;
						MenuCrumbs.Add(TempRow);
						PopulateMenu(0, Menu.ItemsPerPage - 1);
						SelectedItem = -1;
					}
				}
				break;
			case ERadialMenu_ButtonType::RingMenu:
				if (RingItemWidget)
				{
					ItemSelected.Broadcast(SendData);
					RingItemWidget = nullptr;
					HideMenu();
				}
				else
				{
					CalculateOuterRingAngles();
				}
				break;
			default: ;
			}
		}
	}
}

void URadialWidget::SetMaterialSelection(const int Selection) const
{
	MenuMaterial->SetScalarParameterValue("Selection", Selection);
}

void URadialWidget::SelectValidIndex()
{
	SelectedItem = MenuItemWidgets.Num() - 1;
}

void URadialWidget::SelectionMade(const FText& DisplayName)
{
	FRadialMenuSendData SendData;
	SendData.ItemType = GetSelectedItemType();
	SendData.RowName = FName(GetSelectedItemData().ToString());
	ItemSelected.Broadcast(SendData);

	HideMenu();
}

void URadialWidget::GetSelection()
{
	FVector2D LocalVector;

	if (bUsingGamepad)
	{
		LocalVector = RightThumbStick;
	}
	else
	{
		LocalVector = GetMousePosition();
	}

	for (int i = 0; i < ItemsPerPage; ++i)
	{
		int NewSelectedIndex;
		if (CheckInnerItemHovered(i, LocalVector, NewSelectedIndex))
		{
			if (URadial_Item* Item = GetSelectedItemWidget())
			{
				Item->Animate(true);
			}
			SelectedItem = NewSelectedIndex;
			MenuItemWidgets[SelectedItem]->Animate(false);
			UpdateDescription(false);
			if (MenuItemWidgets[SelectedItem]->GetItem().ButtonType != ERadialMenu_ButtonType::RingMenu)
			{
				PlayMenuSound(ERadialSound::HoverItem);
			}
		}
	}

	CheckOuterRingHovered(LocalVector);
}

float URadialWidget::GetOuterItemEdgeThreshold(const float Num) const
{
	return UKismetMathLibrary::DegCos((ItemTheta / Num) / 2);
}

bool URadialWidget::CheckInnerItemHovered(const int ItemIndex, const FVector2D& Vector, int& NewSelectedIndex) const
{
	if (SelectedItem != ItemIndex)
	{
		if (MenuItemWidgets.IsValidIndex(ItemIndex))
		{
			if (CheckForNewItemSelection(Vector, ItemIndex))
			{
				NewSelectedIndex = ItemIndex;
				return true;
			}
		}
	}

	NewSelectedIndex = SelectedItem;
	return false;
}

void URadialWidget::CheckOuterRingHovered(const FVector2D& Vector)
{
	URadial_Item* Item = GetSelectedItemWidget();

	if (Item)
	{
		if (Item->GetItem().ButtonType == ERadialMenu_ButtonType::RingMenu)
		{
			const float TempEdgeThreshold = GetOuterItemEdgeThreshold(Item->GetItems().Num());

			for (URadial_Item*& Element : Item->GetItems())
			{
				if (Element != RingItemWidget && CheckForNewRingItemSelection(Vector, TempEdgeThreshold, Element))
				{
					PlayMenuSound(ERadialSound::HoverItem);
					if (RingItemWidget)
					{
						RingItemWidget->Animate(true);
					}
					RingItemWidget = Element;
					RingItemWidget->Animate(false);
					UpdateDescription(false);
				}
			}
		}
		else
		{
			RingItemWidget = nullptr;
		}
	}
}

bool URadialWidget::CheckForNewItemSelection(const FVector2D& Vector, const int Item) const
{
	if (Vector.Length() > DeadZoneTolerance)
	{
		const float TempItemTheta = Item * ItemTheta;
		const FVector2D TempVector2D(UKismetMathLibrary::DegSin(TempItemTheta),
		                             UKismetMathLibrary::DegCos(TempItemTheta));

		if (UKismetMathLibrary::DotProduct2D(Vector.GetSafeNormal(), TempVector2D) > EdgeTolerance)
		{
			return true;
		}
	}
	return false;
}

bool URadialWidget::CheckForNewRingItemSelection(const FVector2D& Vector, const float EdgeThreshold,
                                                 const UUserWidget* RingItem)
{
	return (UKismetMathLibrary::DotProduct2D(FVector2D(Vector.GetSafeNormal().X, -Vector.GetSafeNormal().Y),
	                                         // ReSharper disable once CppDeprecatedEntity
	                                         RingItem->RenderTransform.Translation.GetSafeNormal()) > EdgeThreshold);
}

#pragma endregion Selection

#pragma region Player

void URadialWidget::HidePlayerUI()
{
	PlayerController->SetInputMode(FInputModeGameOnly());
	PlayerController->ResetIgnoreLookInput();
	PlayerController->SetShowMouseCursor(false);
	StopListeningForAllInputActions();
	UpdateDescription(true);
}

void URadialWidget::ShowPlayerUI()
{
	FInputModeGameAndUI NewInputMode;
	NewInputMode.SetWidgetToFocus(this->TakeWidget());
	NewInputMode.SetHideCursorDuringCapture(true);

	PlayerController->SetInputMode(NewInputMode);
	PlayerController->SetIgnoreLookInput(true);

	if (!bUsingGamepad)
	{
		PlayerController->SetShowMouseCursor(true);
		if (bCenterMouseToRadialMenu)
		{
			CenterMouse();
		}
	}
}

#pragma endregion Player

#pragma region Pagination

int URadialWidget::GetTotalPages(const int InItemsPerPage) const
{
	const bool bPerfectFit = TotalItems % ItemsPerPage == 0;
	const int Pages = TotalItems / InItemsPerPage;
	return bPerfectFit ? Pages : Pages + 1;
}

void URadialWidget::HideScrollableUI() const
{
	MenuCenterContainer_Overlay->SetVisibility(ESlateVisibility::Collapsed);
}

void URadialWidget::ShowScrollableUI()
{
	if (!Menu.Scrollable)
	{
		HideScrollableUI();
		return;
	}

	MenuCenterContainer_Overlay->SetVisibility(ESlateVisibility::HitTestInvisible);

	if (ScrollableWidget)
	{
		ScrollableWidget->ToggleVisibility(bUsingGamepad);
	}
	else
	{
		CreateScrollableWidget();
	}

	if (ScrollableWidget)
	{
		ScrollableWidget->UpdatePage(CurrentPage, TotalPages);
	}
}

void URadialWidget::CreateScrollableWidget()
{
	if (ScrollableWidgetClass)
	{
		ScrollableWidget = CreateWidget<URadial_Scrollable>(this, ScrollableWidgetClass);
		ScrollableWidget->InitializeScrollable(bUsingGamepad);

		if (UOverlaySlot* OverlaySlot = MenuCenterContainer_Overlay->AddChildToOverlay(ScrollableWidget))
		{
			OverlaySlot->SetHorizontalAlignment(HAlign_Center);
			OverlaySlot->SetVerticalAlignment(VAlign_Center);
		}
	}
}

int URadialWidget::GetPageToNavigateTo(const float Direction) const
{
	return FMath::Clamp(FMath::TruncToInt(Direction) + CurrentPage, 0, TotalPages - 1);
}

void URadialWidget::GetFirstLastIndex(int& FirstIndex, int& LastIndex) const
{
	FirstIndex = CurrentPage * ItemsPerPage;
	LastIndex = FMath::Min(FirstIndex + (ItemsPerPage - 1), TotalItems - 1);
}

void URadialWidget::PreviousMenu()
{
	if (!bShowMenu)
		return;

	if (MenuCrumbs.IsValidIndex(1))
	{
		CurrentPage = 0;
		SelectedItem = -1;
		FRadialMenuData* FoundData = DT_RadialMenu->FindRow<FRadialMenuData>(
			MenuCrumbs[MenuCrumbs.Num() - 2], TEXT("NOT FOUND"));
		if (FoundData)
		{
			Menu = *FoundData;
			MenuCrumbs.Pop();
			PopulateMenu(0, Menu.ItemsPerPage - 1);
			PlayMenuSound(ERadialSound::SelectItem);
		}
	}
	else
	{
		HideMenu();
	}
}

#pragma endregion Pagination

void URadialWidget::PlayMenuSound(const ERadialSound Sound)
{
	switch (Sound)
	{
	case ERadialSound::OpenMenu:
		PlaySound(RadialMenuSetting.OpenMenu);
		return;;
	case ERadialSound::CloseMenu:
		PlaySound(RadialMenuSetting.CloseMenu);
		return;;
	case ERadialSound::ScrollMenu:
		PlaySound(RadialMenuSetting.ScrollMenu);
		return;;
	case ERadialSound::HoverItem:
		PlaySound(RadialMenuSetting.HoverMenu);
		return;;
	case ERadialSound::SelectItem:
		PlaySound(RadialMenuSetting.SelectMenu);
		return;;
	default:
		return;
	}
}

void URadialWidget::ClearItems()
{
	ItemsContainer_Overlay->ClearChildren();
	MenuItemWidgets.Empty();
}

void URadialWidget::ModifyOrAddItems()
{
}
