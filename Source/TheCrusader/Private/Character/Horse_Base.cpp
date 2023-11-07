// Written by jaegang lim


#include "Character/Horse_Base.h"

#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Character/Balian.h"
#include "Components/ArrowComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/KismetMathLibrary.h"


AHorse_Base::AHorse_Base()
{
	PrimaryActorTick.bCanEverTick = true;

	InteractionCollision = CreateDefaultSubobject<UCapsuleComponent>("InteractionCollision");
	LeftMountPoint = CreateDefaultSubobject<UArrowComponent>("LeftMountPoint");
	RightMountPoint = CreateDefaultSubobject<UArrowComponent>("RightMountPoint");
	LeftArriveDetect = CreateDefaultSubobject<UCapsuleComponent>("LeftArriveDetect");
	RightArriveDetect = CreateDefaultSubobject<UCapsuleComponent>("RightArriveDetect");

	FacialHair = CreateDefaultSubobject<USkeletalMeshComponent>("FacialHair");
	Reins = CreateDefaultSubobject<USkeletalMeshComponent>("Reins");
	Head = CreateDefaultSubobject<USkeletalMeshComponent>("Head");
	Pad = CreateDefaultSubobject<USkeletalMeshComponent>("Pad");
	Saddle = CreateDefaultSubobject<USkeletalMeshComponent>("Saddle");


	InteractionCollision->SetupAttachment(GetRootComponent());
	LeftMountPoint->SetupAttachment(GetRootComponent());
	RightMountPoint->SetupAttachment(GetRootComponent());
	LeftArriveDetect->SetupAttachment(LeftMountPoint);
	RightArriveDetect->SetupAttachment(RightMountPoint);

	FacialHair->SetupAttachment(GetMesh());
	Head->SetupAttachment(GetMesh());
	Reins->SetupAttachment(Head, FName("Reins_Socket"));
	Pad->SetupAttachment(GetMesh());
	Saddle->SetupAttachment(GetMesh());

	LeftArriveDetect->OnComponentBeginOverlap.AddDynamic(this, &ThisClass::OnBeginOverlap);
	RightArriveDetect->OnComponentBeginOverlap.AddDynamic(this, &ThisClass::OnBeginOverlap);

	LeftArriveDetect->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	RightArriveDetect->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

void AHorse_Base::BeginPlay()
{
	Super::BeginPlay();

	AutoPossessPlayer = EAutoReceiveInput::Disabled;
	InteractableData = InstancedInteractableData;
}

void AHorse_Base::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);
}

void AHorse_Base::OnBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
                                 UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep,
                                 const FHitResult& SweepResult)
{
	if (Rider == OtherActor)
	{
		float Location = UKismetMathLibrary::Dot_VectorVector(GetActorForwardVector(), Rider->GetActorForwardVector());
		if (Location > .5f)
		{
			TryMount(Rider, OverlappedComp, "Back");
		}
		else if (Location >= -.3f)
		{
			TryMount(Rider, OverlappedComp, "Side");
		}
		else
		{
			TryMount(Rider, OverlappedComp, "Front");
		}
	}
}


void AHorse_Base::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);


	if (bShouldStop)
	{
		InputForward = UKismetMathLibrary::FInterpTo(InputForward, 0.f, DeltaTime,
		                                             InputForward > 3.f ? .3f : .5f);
	}

	if (bShouldStopTurn)
	{
		InputSide = UKismetMathLibrary::FInterpTo(InputSide, .0f, DeltaTime, InputSide + .00001f);
	}
}

void AHorse_Base::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	check(PlayerInputComponent);

	const APlayerController* PlayerController = Cast<APlayerController>(Controller);

	if (UEnhancedInputLocalPlayerSubsystem* SubsystemInterface = ULocalPlayer::GetSubsystem<
		UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
	{
		SubsystemInterface->AddMappingContext(InputMappingContext, 0);
		SubsystemInterface->AddMappingContext(LookControlsInputMappingContext, 1);
	}

	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent))
	{
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &ThisClass::LookActionBinding);
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Completed, this, &ThisClass::LookActionBinding);
		EnhancedInputComponent->BindAction(LookDeltaAction, ETriggerEvent::Triggered, this,
		                                   &ThisClass::LookDeltaActionBinding);
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &ThisClass::MoveActionBinding);
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Completed, this, &ThisClass::MoveActionBinding);
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Triggered, this, &ThisClass::Jump);
		EnhancedInputComponent->BindAction(DismountAction, ETriggerEvent::Triggered, this,
		                                   &ThisClass::DismountActionBinding);
		EnhancedInputComponent->BindAction(LMBAction, ETriggerEvent::Triggered, Rider.Get(),
		                                   &ABalian::LMBClick);
		EnhancedInputComponent->BindAction(RMBAction, ETriggerEvent::Triggered, Rider.Get(),
		                                   &ABalian::RMBClick);
		EnhancedInputComponent->BindAction(RMBAction, ETriggerEvent::Completed, Rider.Get(),
		                                   &ABalian::RMBCompleted);
	}
}

void AHorse_Base::RemoveMappingContext() const
{
	if (const APlayerController* PlayerController = Cast<APlayerController>(Controller))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<
			UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			Subsystem->RemoveMappingContext(InputMappingContext);
			Subsystem->RemoveMappingContext(LookControlsInputMappingContext);
		}
	}
}

void AHorse_Base::TryMount(ABalian* PlayerCharacter, UPrimitiveComponent* DirectionArrow, FName SectionName)
{
	if (AController* PlayerController = PlayerCharacter->GetController())
	{
		PlayerCharacter->RemoveMappingContext();
		PlayerController->UnPossess();
		PlayerController->Possess(this);

		PlayerCharacter->GetCharacterMovement()->SetMovementMode(MOVE_Flying);
		PlayerCharacter->GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		PlayerCharacter->SetOwningHorse(this);
		PlayerCharacter->SetRiding(true);


		if (DirectionArrow == LeftArriveDetect)
		{
			PlayerCharacter->PlayAnimMontage(RiderMontages[0], 1, SectionName);
		}
		else
		{
			PlayerCharacter->PlayAnimMontage(RiderMontages[1], 1, SectionName);
		}

		LeftArriveDetect->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		RightArriveDetect->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	}
}

void AHorse_Base::TryUnMount()
{
	if (APlayerController* PlayerController = Cast<APlayerController>(GetController()))
	{
		RemoveMappingContext();
		PlayerController->UnPossess();
		PlayerController->Possess(Rider);

		Rider->PlayAnimMontage(RiderMontages[FMath::RandRange(2, 3)]);
		Rider->GetCharacterMovement()->SetMovementMode(MOVE_Walking);
		Rider->DisableInput(PlayerController);

		bShouldStop = true;
		bShouldStopTurn = true;
	}
}

void AHorse_Base::BeginFocus()
{
	IInteractable::BeginFocus();
}

void AHorse_Base::EndFocus()
{
	IInteractable::EndFocus();
}

void AHorse_Base::BeginInteract()
{
	IInteractable::BeginInteract();
}

void AHorse_Base::EndInteract()
{
	IInteractable::EndInteract();
}

void AHorse_Base::Interact(ABalian* PlayerCharacter)
{
	if (PlayerCharacter)
	{
		FVector TargetLocation;
		if (UKismetMathLibrary::Vector_Distance(PlayerCharacter->GetActorLocation(),
		                                        RightMountPoint->GetComponentLocation()) >
			UKismetMathLibrary::Vector_Distance(PlayerCharacter->GetActorLocation(),
			                                    LeftMountPoint->GetComponentLocation()))
		{
			TargetLocation = LeftMountPoint->GetComponentLocation();
			LeftArriveDetect->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
		}
		else
		{
			TargetLocation = RightMountPoint->GetComponentLocation();
			RightArriveDetect->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
		}

		Rider = PlayerCharacter;
		PlayerCharacter->MoveToLocation(TargetLocation);
	}
}

void AHorse_Base::Look(const FVector2D& Value)
{
	TurnAtRate(Value.X);
	LookUpAtRate(Value.Y);
}

void AHorse_Base::TurnAtRate(float Rate)
{
	AddControllerYawInput(Rate * BaseTurnRate * GetWorld()->GetDeltaSeconds());
}

void AHorse_Base::LookUpAtRate(float Rate)
{
	AddControllerPitchInput(Rate * BaseLookUpRate * GetWorld()->GetDeltaSeconds());
}

#pragma region Action Binding

void AHorse_Base::LookActionBinding(const FInputActionValue& ActionValue)
{
	Look(ActionValue.Get<FInputActionValue::Axis2D>());
}

void AHorse_Base::LookDeltaActionBinding(const FInputActionValue& ActionValue)
{
	const FInputActionValue::Axis2D AxisValue = ActionValue.Get<FInputActionValue::Axis2D>();
	APawn::AddControllerYawInput(AxisValue.X);
	APawn::AddControllerPitchInput(AxisValue.Y);
}

void AHorse_Base::MoveActionBinding(const FInputActionValue& InputActionValue)
{
	const FInputActionValue::Axis2D AxisValue = InputActionValue.Get<FInputActionValue::Axis2D>();

	const float InputValueY = bSprinting ? AxisValue.Y * 5 : AxisValue.Y * 3;

	InputForward = UKismetMathLibrary::FInterpTo(InputForward, InputValueY,
	                                             GetWorld()->DeltaTimeSeconds,
	                                             InputForward <= 1.f ? .4f : .24f);

	// 무시할만한 입력 -> 속도 저하
	bShouldStop = InputValueY <= .1f ? true : false;

	bShouldStopTurn = UKismetMathLibrary::Abs(AxisValue.Y) <= .1f ? true : false;

	const float InputValueX = (bSprinting || InputForward > 2.4f) ? AxisValue.X : AxisValue.X * .5f;

	InputSide = InputValueX;
}

void AHorse_Base::DismountActionBinding()
{
	TryUnMount();
}

#pragma endregion Action Binding
