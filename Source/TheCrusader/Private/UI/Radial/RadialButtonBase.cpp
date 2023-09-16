// Written by jaegang lim


#include "UI/Radial/RadialButtonBase.h"
#include "Components/Image.h"
#include "Player/TCPlayerController.h"

void URadialButtonBase::NativePreConstruct()
{
	Super::NativePreConstruct();

	Image->SetBrushFromTexture(ImageTexture);
}
