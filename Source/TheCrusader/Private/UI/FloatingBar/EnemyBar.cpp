// Written by jaegang lim


#include "UI/FloatingBar/EnemyBar.h"

#include "Components/Border.h"
#include "Components/Image.h"
#include "Components/ProgressBar.h"

void UEnemyBar::UpdateHP(const float NewValue)
{
	HPBar->SetPercent(NewValue);
}

void UEnemyBar::UpdateStamina(const float NewValue)
{
	StaminaBar->SetPercent(NewValue);
}

void UEnemyBar::NativeConstruct()
{
	Super::NativeConstruct();

	IconHighlightBorder->SetVisibility(ESlateVisibility::Collapsed);
	HPBorder->SetVisibility(ESlateVisibility::Collapsed);
}

void UEnemyBar::HighlightBorder() const
{
	IconHighlightBorder->SetVisibility(ESlateVisibility::Visible);
	HPBorder->SetVisibility(ESlateVisibility::Visible);
}

void UEnemyBar::UnHighlightBorder() const
{
	IconHighlightBorder->SetVisibility(ESlateVisibility::Collapsed);
	HPBorder->SetVisibility(ESlateVisibility::Collapsed);
}
