// Written by jaegang lim


#include "UI/Page/PageTitle/PageTitle.h"

#include "Components/TextBlock.h"
#include "UI/MainMenu.h"
#include "UI/TC_HUD.h"


void UPageTitle::NativeConstruct()
{
	Super::NativeConstruct();

	PageTitle->SetText(PageText);

	if (const ATC_HUD* HUD = Cast<ATC_HUD>(GetOwningPlayer()->GetHUD()))
	{
		ParentMenu = HUD->GetMainMenuWidget();
	}
}

void UPageTitle::SetAsActivePage() const
{
	PageTitle->SetColorAndOpacity(bActive ? SelectedColor : FColor::White);
}
