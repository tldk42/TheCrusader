// Written by jaegang lim

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "RadialButtonBase.generated.h"

UENUM(BlueprintType)
enum class EButtonType : uint8
{
	Idle,
	Fist,
	Sword,
	Bow,
	Horse,
	Torch,
	Locked
};

/**
 * 
 */
UCLASS()
class THECRUSADER_API URadialButtonBase : public UUserWidget
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "State")
	bool bLocked;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "State")
	bool bActivated;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "State")
	EButtonType ButtonType;

protected:
	virtual void NativePreConstruct() override;

private:
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "RadialButton",
		meta = (AllowPrivateAccess = true, BindWidget))
	class UImage* Image;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "RadialButton",
		meta = (AllowPrivateAccess = true))
	class UTexture2D* ImageTexture;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "RadialButton",
		meta = (AllowPrivateAccess = true, BindWidget))
	class UButton* Button;
};
