// Written by jaegang lim


#include "UI/Radial/RadialButtonBase.h"
#include "Components/Image.h"
#include "Player/TCPlayerController.h"

void URadialButtonBase::LockButton()
{
	bLocked = true;
	Image->SetBrushFromTexture(LockImageTexture);
}

void URadialButtonBase::UnLockButton()
{
	bLocked = false;
	Image->SetBrushFromTexture(LockImageTexture);
}

void URadialButtonBase::NativePreConstruct()
{
	Super::NativePreConstruct();

	Image->SetBrushFromTexture(ImageTexture);
}
