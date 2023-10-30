// Written by jaegang lim

#pragma once

#include "CoreMinimal.h"
#include "TCCharacterBase.h"
#include "InventoryPreview.generated.h"

class UItemEquipmentBase;
class ABalian;

UCLASS()
class THECRUSADER_API AInventoryPreview : public ATCCharacterBase
{
	GENERATED_BODY()

public:
	AInventoryPreview(const FObjectInitializer& ObjectInitializer);

	virtual void AttachEquipment(const EEquipmentPart EquipmentPart, const UItemEquipmentBase* ItemToEquip) override;
	virtual void DetachEquipment(EEquipmentPart EquipmentPart) override;

protected:
	virtual void BeginPlay() override;

private:

private:
	UPROPERTY(EditDefaultsOnly)
	USceneCaptureComponent2D* SceneCaptureComponent2D;

	UPROPERTY(EditDefaultsOnly, Category = "ACC")
	UStaticMeshComponent* PreviewWeapon;
	UPROPERTY(EditDefaultsOnly, Category = "ACC")
	USkeletalMeshComponent* PreviewBow;
};
