﻿// Written by jaegang lim

#include "UI/Radial/RadialMenu.h"

#include "Blueprint/WidgetLayoutLibrary.h"
#include "Components/CanvasPanel.h"
#include "Components/CanvasPanelSlot.h"
#include "Kismet/KismetMathLibrary.h"
#include "Player/TCPlayerController.h"
#include "UI/Radial/RadialButtonBase.h"

void URadialMenu::UpdateActiveBar() const
{
	const auto ActivatedBtn = GetInstancedButton(SelectedIndex);
	if (!ActivatedBtn)
		return;
	if (ActivatedBtn->bLocked)
		return;
	RadialMenuMaterial->SetScalarParameterValue(FName("ActiveRotation"), SectionSize * SelectedIndex);

	ATCPlayerController* Player = Cast<ATCPlayerController>(GetOwningPlayer());
	if (Player)
	{
		Player->UpdatePlayerState(ActivatedBtn->ButtonType);
	}
}


void URadialMenu::NativeConstruct()
{
	if (bSpawned)
		return;

	Super::NativeConstruct();

	RadialMenuMaterial->SetScalarParameterValue(FName("SectionCount"), SectionCount);
	SectionSize = (360 / SectionCount);
	for (int i = 0; i < SectionCount; ++i)
	{
		const float CurrentSection = (SectionSize * i);
		MaxBounds.Add(CurrentSection + SectionSize / 2);
		MinBounds.Add(CurrentSection - SectionSize / 2);
	}

	SpawnButtons();

	InitialRotationAxis = UKismetMathLibrary::RotateAngleAxis(
		FVector(0, -1, 0), 90, FVector(0, 0, 1));

	int index = 0;
	for (UWidget* Button : ButtonPanel->GetAllChildren())
	{
		UCanvasPanelSlot* PanelSlot = UWidgetLayoutLibrary::SlotAsCanvasSlot(Button);

		const FVector Vec = UKismetMathLibrary::RotateAngleAxis(
				InitialRotationAxis, (index) * (360 / SectionCount), FVector(0, 0, 1))
			* 250;

		PanelSlot->SetAnchors(FAnchors(.5f, .5f, .5f, .5f));
		PanelSlot->SetAlignment(FVector2D(.5f, .5f));
		PanelSlot->SetPosition(FVector2D(Vec.X, -Vec.Y));
		PanelSlot->SetSize(FVector2D(150, 150));
		index++;
	}

	bSpawned = true;
}

FReply URadialMenu::NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	UpdateActiveBar();
	return Super::NativeOnMouseButtonDown(InGeometry, InMouseEvent);
}

FReply URadialMenu::NativeOnMouseMove(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	CalculateSection();
	return Super::NativeOnMouseMove(InGeometry, InMouseEvent);
}

void URadialMenu::SpawnButtons()
{
	for (auto Button : Buttons)
	{
		// ButtonInstances.Add();
		ButtonPanel->AddChild(CreateWidget<URadialButtonBase>(this, Button.Get()));
	}
}

float URadialMenu::GetMouseRotation() const
{
	float MouseX, MouseY;
	FVector MouseLocation;
	if (GetOwningPlayer()->GetMousePosition(MouseX, MouseY))
	{
		MouseLocation = FVector(MouseX, MouseY, 0.f);
	}
	const FVector2D ScreenCenter2D = UWidgetLayoutLibrary::GetViewportSize(GetWorld()) / 2;
	const FVector ScreenCenter = FVector(ScreenCenter2D.X, ScreenCenter2D.Y, 0.f);

	// 마우스 위치에서 화면 정중을 바라보는 로테이션
	const FRotator LookRot = UKismetMathLibrary::FindLookAtRotation(MouseLocation, ScreenCenter);
	// UE_LOG(LogTemp, Warning, TEXT("%f"), 180.f - LookRot.Yaw);
	return (180.f - LookRot.Yaw);
}

void URadialMenu::CalculateSection()
{
	for (int i = 0; i < SectionCount; ++i)
	{
		// 마우스가 현재 인덱스의 부채꼴 내부에 존재하면 빠져나가고 후처리한다.
		if (UKismetMathLibrary::InRange_FloatFloat(GetMouseRotation(), MinBounds[i], MaxBounds[i]))
		{
			SelectedIndex = i;
			break;
		}
	}

	// UI 변경 (Material Instance param 변경)	
	RadialMenuMaterial->SetScalarParameterValue(FName("SelectedRotation"), SelectedIndex * SectionSize);
}

URadialButtonBase* URadialMenu::GetInstancedButton(int Index) const
{
	return Cast<URadialButtonBase>(ButtonPanel->GetChildAt(Index));
}
