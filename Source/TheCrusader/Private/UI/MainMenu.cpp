// Written by jaegang lim


#include "UI/MainMenu.h"

#include "Character/Balian.h"
#include "Components/HorizontalBox.h"
#include "Components/WidgetSwitcher.h"
#include "UI/Page/PageTitle/PageTitle.h"

void UMainMenu::NativeConstruct()
{
	Super::NativeConstruct();

	PlayerCharacter = Cast<ABalian>(GetOwningPlayerPawn());

	UpdatePage(CurrentPageIndex);
}

bool UMainMenu::NativeOnDrop(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent,
                             UDragDropOperation* InOperation)
{
	return Super::NativeOnDrop(InGeometry, InDragDropEvent, InOperation);
}
void UMainMenu::UpdatePage(const int32 NewIndex)
{
	const TArray<UWidget*> AllPages = Pages->GetAllChildren();
	if (AllPages.Num() > 0)
	{
		if (NewIndex < AllPages.Num() && NewIndex != CurrentPageIndex)
		{
			UPageTitle* PrevIndex = Cast<UPageTitle>(AllPages[CurrentPageIndex]);
			PrevIndex->bActive = false;
			PrevIndex->SetAsActivePage();

			UPageTitle* NextIndex = Cast<UPageTitle>(AllPages[NewIndex]);
			NextIndex->bActive = true;
			NextIndex->SetAsActivePage();

			PageSwitcher->SetActiveWidgetIndex(NewIndex);
			CurrentPageIndex = NewIndex;
		}
	}
}
