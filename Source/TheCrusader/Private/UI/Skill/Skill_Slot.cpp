// Written by jaegang lim


#include "UI/Skill/Skill_Slot.h"

#include "Component/Skill/SkillComponent.h"
#include "Components/Button.h"
#include "Components/ProgressBar.h"
#include "UI/MainMenu.h"
#include "UI/TC_HUD.h"
#include "UI/Skill/Skill_Tree.h"

void USkill_Slot::NativePreConstruct()
{
	Super::NativePreConstruct();

	SkillInfo = USkillComponent::GetSkillInfo(Skill_DataTableRowHandle);

	SetStyles(SkillInfo);
}

void USkill_Slot::NativeConstruct()
{
	Super::NativeConstruct();

	SetupSkillSlots();
}

void USkill_Slot::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	Logo->OnHovered.AddDynamic(this, &ThisClass::OnHoveredEvent);
	Logo->OnUnhovered.AddDynamic(this, &ThisClass::OnUnHoveredEvent);
	Logo->OnClicked.AddDynamic(this, &ThisClass::OnClickedEvent);
}

USkill_Tree* USkill_Slot::GetSkillTreeWBP() const
{
	if (const ATC_HUD* HUD = Cast<ATC_HUD>(GetOwningPlayer()->GetHUD()))
	{
		if (IsValid(HUD->GetMainMenuWidget()))
		{
			return HUD->GetMainMenuWidget()->SkillTreeWidget;
		}
	}
	return nullptr;
}

void USkill_Slot::Learn_Implementation()
{
	if (!bCanBeLearned || bLearned)
		return;

	TryToLearn();
	SetIsFocusable(false);
	ProgressBar->SetVisibility(ESlateVisibility::Hidden);
	// disable pressing F
	if (USkill_Tree* Skill_Tree = GetSkillTreeWBP())
	{
		Skill_Tree->UpdateSkillsResult();
		// SetAdjacentSlots();
	}
}

void USkill_Slot::TryToLearn_Implementation()
{
	if (bLearned)
		return;

	if (CheckRequiredSkills(AllSkillSlots, SkillInfo.RequiredSkills))
	{
		PlayVisualEffect();
	}
}

void USkill_Slot::SetupSkillSlots_Implementation()
{
	if (USkill_Tree* Skill_TreeWidget = GetSkillTreeWBP())
	{
		// Palette 는 부모 위젯 스킬들을 배치할 팔레트임
		const TArray<USkill_Slot*> Skill_Slots = Skill_TreeWidget->GetSkill_Slots(PaletteWidget);

		if (Skill_Slots.Num() > 0 && Skill_Slots.Contains(this))
		{
			NextSkillSlots = FindNextSlots();
			// FindConnectors
			
		}
	}
}

void USkill_Slot::PlayVisualEffect()
{
	if (CanLearn())
	{
		if (USkillComponent* SkillComponent = Cast<USkillComponent>(
			GetOwningPlayerPawn()->GetComponentByClass(USkillComponent::StaticClass())))
		{
			SkillComponent->LearnSkill(Skill_DataTableRowHandle);
			bLearned = true;
			PlayAnimation(OnLearned);
			PlaySound(AllowedSound);
			SkillLevel->SetText(FText());
		}
	}
	else
	{
		PlaySound(DisAllowedSound);
	}
}

void USkill_Slot::SetStyles(const FSkill& SkillInfo) const
{
	UTexture2D* LearnedIcon = SkillInfo.LearnedIcon;
	UTexture2D* OriginIcon = SkillInfo.OriginIcon;

	const FSlateColor HoveredColor = (bCanBeLearned && Logo->IsHovered() && !bLearned && CanLearn())
		                                 ? TryingToLearningColor
		                                 : FColor::White;

	FButtonStyle NewButtonStyle;
	NewButtonStyle.Normal.SetResourceObject(OriginIcon);
	NewButtonStyle.Hovered.TintColor = HoveredColor;
	NewButtonStyle.Hovered.SetResourceObject(LearnedIcon);
	NewButtonStyle.Pressed.SetResourceObject(OriginIcon);
	NewButtonStyle.Disabled.SetResourceObject(OriginIcon);

	Logo->SetStyle(NewButtonStyle);

	UpdateBackgroundColor();
}

void USkill_Slot::UpdateBackgroundColor() const
{
	if (bCanBeLearned || Logo->IsHovered() || !bLearned || CanLearn())
	{
		if (bLearned)
		{
			SkillLevel->SetColorAndOpacity(LearnedColor);
			Logo->SetBackgroundColor(LearnedColor);
		}
		else
		{
			if (bTryingToLearn)
			{
				SkillLevel->SetColorAndOpacity(TryingToLearningColor);
				Logo->SetBackgroundColor(TryingToLearningColor);
			}
			else if (bCanBeLearned)
			{
				SkillLevel->SetColorAndOpacity(CanBeLearnedColor);
				Logo->SetBackgroundColor(CanBeLearnedColor);
			}
			else
			{
				SkillLevel->SetColorAndOpacity(CannotBeLearnedColor);
				Logo->SetBackgroundColor(CannotBeLearnedColor);
			}
		}
	}
	else
	{
		SkillLevel->SetColorAndOpacity(TryingToLearningColor);
		Logo->SetBackgroundColor(TryingToLearningColor);
	}
}

TArray<USkill_Slot*> USkill_Slot::FindNextSlots()
{
	TArray<USkill_Slot*> FoundSkill_Slots;
	for (USkill_Slot* Element : AllSkillSlots)
	{
		for (const FDataTableRowHandle RequiredSkill : Element->SkillInfo.RequiredSkills)
		{
			FSkill Skill = USkillComponent::GetSkillInfo(RequiredSkill);
			if (Skill.Name.EqualTo(SkillInfo.Name))
			{
				FoundSkill_Slots.Add(Element);
			}
		}
	}
	return FoundSkill_Slots;
}

bool USkill_Slot::CanLearn() const
{
	if (const USkillComponent* SkillComponent = Cast<USkillComponent>(
		GetOwningPlayerPawn()->GetComponentByClass(USkillComponent::StaticClass())))
	{
		return SkillComponent->GetAvailablePoint() >= SkillInfo.RequiredPoint;
	}
	return false;
}

bool USkill_Slot::CheckRequiredSkills(const TArray<USkill_Slot*>& Skill_Slots,
                                      const TArray<FDataTableRowHandle>& RowHandles) const
{
	const TArray<const USkill_Slot*> FoundedRequiredSkill_Slots = FindRequiredSkillWidgets(Skill_Slots, RowHandles);
	if (FoundedRequiredSkill_Slots.Num() <= 0)
		return false;

	int NotLearnedSkillCounter = 0;

	for (const USkill_Slot* Skill_Slot : FoundedRequiredSkill_Slots)
	{
		// 이전에 존재하는 스킬을 모두 수행?
		if (SkillInfo.bAllRequiredSkillsShouldBeLearned)
		{
			if (!Skill_Slot->bLearned)
			{
				// 이전 스킬 활성화가 안되어있음 (빠져나감)
				return false;
			}
		}
		// 이전 스킬들을 선행할 필요가 없으면
		else
		{
			if (Skill_Slot->bLearned)
			{
				return true;
			}
			NotLearnedSkillCounter++;
		}
	}

	return NotLearnedSkillCounter < FoundedRequiredSkill_Slots.Num();
}

TArray<const USkill_Slot*> USkill_Slot::FindRequiredSkillWidgets(const TArray<USkill_Slot*>& Skill_Slots,
                                                                 const TArray<FDataTableRowHandle>& RowHandles)
{
	TArray<const USkill_Slot*> FoundedSkillSlots;

	for (const USkill_Slot* Skill_Slot : Skill_Slots)
	{
		for (FDataTableRowHandle RowHandle : RowHandles)
		{
			if (RowHandle.RowName == Skill_Slot->Skill_DataTableRowHandle.RowName)
			{
				FoundedSkillSlots.AddUnique(Skill_Slot);
			}
		}
	}

	return FoundedSkillSlots;
}

void USkill_Slot::OnHoveredEvent()
{
	USkill_Tree* Skill_TreeWidget;
	if (bLearned || !bCanBeLearned)
	{
		Skill_TreeWidget = GetSkillTreeWBP();
		if (Skill_TreeWidget)
		{
			Skill_TreeWidget->SkillSlotOnHovered(Skill_DataTableRowHandle, this, bLearned, bCanBeLearned, CanLearn());
		}
		return;
	}

	SetStyles(SkillInfo);

	Logo->SetKeyboardFocus();

	if (!IsAnimationPlaying(OnLearned))
	{
		if (!bHoveredTrigger)
		{
			bHoveredTrigger = true;
			PlayAnimation(OnHovered);

			Skill_TreeWidget = GetSkillTreeWBP();
			if (Skill_TreeWidget)
			{
				Skill_TreeWidget->SkillSlotOnHovered(Skill_DataTableRowHandle, this, bLearned, bCanBeLearned,
				                                     CanLearn());
				FTimerHandle TimerHandle;
				GetWorld()->GetTimerManager().SetTimer(TimerHandle, [this]()
				{
					bHoveredTrigger = false;
				}, 0.2f, false);
			}
		}
	}
}

void USkill_Slot::OnUnHoveredEvent()
{
	if (USkill_Tree* Skill_TreeWidget = GetSkillTreeWBP())
	{
		SetStyles(SkillInfo);

		if (IsAnimationPlaying(OnLearned))
		{
			Skill_TreeWidget->SkillSlotOnUnHovered();
			return;
		}

		if (!bUnHoveredTrigger)
		{
			bUnHoveredTrigger = true;
			PlayAnimation(OnUnHovered);
			Skill_TreeWidget->SkillSlotOnUnHovered();
			FTimerHandle TimerHandle;
			GetWorld()->GetTimerManager().SetTimer(TimerHandle, [this]()
			{
				bUnHoveredTrigger = false;
			}, 0.2f, false);
		}
	}
}

void USkill_Slot::OnClickedEvent()
{
	Learn();
}
