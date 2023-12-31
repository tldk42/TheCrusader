﻿// Written by jaegang lim

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "MainMenu.generated.h"

class USkill_Tree;
class UWidgetSwitcher;
class UHorizontalBox;
class ABalian;
/**
 * 
 */
UCLASS()
class THECRUSADER_API UMainMenu : public UUserWidget
{
	GENERATED_BODY()

protected:
	virtual void NativeConstruct() override;
	virtual bool NativeOnDrop(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent,
	                          UDragDropOperation* InOperation) override;

public:
	UFUNCTION(BlueprintCallable)
	void UpdatePage(int32 NewIndex);

public:
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget = true))
	UHorizontalBox* Pages;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget = true))
	UWidgetSwitcher* PageSwitcher;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget = true))
	USkill_Tree* SkillTreeWidget;

	UPROPERTY(BlueprintReadWrite)
	int32 CurrentPageIndex = 0;

	UPROPERTY()
	ABalian* PlayerCharacter;
};
