// Written by jaegang lim


#include "Character/NPC/NPC_Base.h"
#include "Npc/NPCManager.h"
#include "Character/Balian.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "Player/TCPlayerCameraManager.h"
#include "Player/TCPlayerState.h"


ANPC_Base::ANPC_Base(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	PrimaryActorTick.bCanEverTick = false;


	NPCManager = CreateDefaultSubobject<UNPCManager>(TEXT("NPC Manager"));
}

void ANPC_Base::BeginFocus()
{
	IInteractable::BeginFocus();
}

void ANPC_Base::EndFocus()
{
	IInteractable::EndFocus();
}

void ANPC_Base::BeginInteract()
{
	IInteractable::BeginInteract();
}

void ANPC_Base::EndInteract()
{
	IInteractable::EndInteract();
}

void ANPC_Base::Interact(ABalian* PlayerCharacter)
{
	SetActorRotation(UKismetMathLibrary::FindLookAtRotation(GetActorLocation(), PlayerCharacter->GetActorLocation()));

	if (ATCPlayerCameraManager* CameraManager = Cast<ATCPlayerCameraManager>(
		UGameplayStatics::GetPlayerCameraManager(GetWorld(), 0)))
	{
		if (UTCCameraMode* CurrentCamera = CameraManager->GetCurrentCameraMode())
		{
			CurrentCamera->bUseCineCam = true;
			CurrentCamera->bUseCineCamSettings = true;
			// CurrentCamera->FOV = 20.f;
			APlayerController* TCController = UGameplayStatics::GetPlayerController(GetWorld(), 0);
			TCController->SetShowMouseCursor(true);
			TCController->SetInputMode(FInputModeUIOnly());
			// PlayerCharacter->DisableInput(TCController);

			if (ATCPlayerState* PlayerSt = Cast<ATCPlayerState>(PlayerCharacter->GetPlayerState()))
			{
				PlayerSt->StartInteractNPC(this);
			}
		}
	}
}

// Called when the game starts or when spawned
void ANPC_Base::BeginPlay()
{
	Super::BeginPlay();

	InteractableData = InstancedInteractableData;
}

// Called every frame
void ANPC_Base::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

// Called to bind functionality to input
void ANPC_Base::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}
