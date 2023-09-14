// Written by jaegang lim

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "RadialMenu.generated.h"

class URadialButtonBase;
class UCanvasPanel;
/**
 * 
 */
UCLASS()
class THECRUSADER_API URadialMenu : public UUserWidget
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable)
	void UpdateActiveBar() const;

protected:
	virtual void NativeConstruct() override;
	virtual FReply NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
	virtual FReply NativeOnMouseMove(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;

private:

	void SpawnButtons();
	
	float GetMouseRotation() const;

	void CalculateSection();

	URadialButtonBase* GetInstancedButton(int Index) const;

private:

	bool bSpawned = false;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "RadialMenu",
		meta = (AllowPrivateAccess = true, BindWidget))
	UCanvasPanel* ButtonPanel;

	UPROPERTY(EditInstanceOnly, BlueprintReadWrite, Category = "RadialMenu",
		meta = (AllowPrivateAccess = true, ExposeOnSpawn = true))
	UMaterialInstanceDynamic* RadialMenuMaterial;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "RadialMenu", meta = (AllowPrivateAccess = true))
	int SectionCount;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "RadialMenu", meta = (AllowPrivateAccess = true))
	float SectionSize;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "RadialMenu", meta = (AllowPrivateAccess = true))
	TArray<float> MaxBounds;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "RadialMenu", meta = (AllowPrivateAccess = true))
	TArray<float> MinBounds;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "RadialMenu", meta = (AllowPrivateAccess = true))
	float SelectedIndex;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "RadialMenu", meta = (AllowPrivateAccess = true))
	FVector InitialRotationAxis;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "RadialMenu", meta = (AllowPrivateAccess = true))
	TArray<TSubclassOf<URadialButtonBase>> Buttons;

	// TArray<URadialButtonBase*> ButtonInstances;
};
