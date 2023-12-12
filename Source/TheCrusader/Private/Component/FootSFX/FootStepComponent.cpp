// Written by jaegang lim


#include "Component/FootSFX/FootStepComponent.h"
#include "NiagaraComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "GameFramework/Character.h"
#include "Kismet/GameplayStatics.h"
#include "PhysicalMaterials/PhysicalMaterial.h"


void UFootStepComponent::BeginPlay()
{
	Super::BeginPlay();

	GetRefData();

	check(CharacterRef)

	CharacterRef->OnActorHit.AddDynamic(this, &ThisClass::OnActorHit_Event);
}

void UFootStepComponent::GetRefData()
{
	CharacterRef = Cast<ACharacter>(GetOwner());

	if (CharacterRef)
	{
		MeshComponent = CharacterRef->GetMesh();
	}

	if (!CharacterRef)
	{
		MeshComponent = Cast<USkeletalMeshComponent>(GetOwner());
	}
}

void UFootStepComponent::SpawnFootStepParticle()
{
	// Left
	FootLineTrace(LeftFootStepSocket);

	ParticleName = *PhysicalMaterial->GetName();

	Niagara = NiagaraMap.Contains(ParticleName) ? NiagaraMap[ParticleName] : Footstep_Default_Niagara;
	Particle = ParticleMap.Contains(ParticleName) ? ParticleMap[ParticleName] : Footstep_Default_Particle;

	FVector Location = MeshComponent->GetSocketLocation(FootName);
	FRotator Rotation = MeshComponent->GetSocketRotation(FootName);
	if (bFootStepParticle)
	{
		UParticleSystemComponent* ParticleSystemComponent = UGameplayStatics::SpawnEmitterAtLocation(
			GetWorld(), Particle, Location,
			Rotation, true);

		ParticleSystemComponent->SetWorldScale3D(FVector::One() * ParticleScale);
	}

	if (bFootStepNiagara)
	{
		UNiagaraComponent* NiagaraComponent = UNiagaraFunctionLibrary::SpawnSystemAtLocation(
			GetWorld(), Niagara, MeshComponent->GetSocketLocation(FootName),
			Rotation,
			FVector::One() * NiagaraScale);

		if (NiagaraComponent)
		{
			NiagaraComponent->SetFloatParameter("User.Size", (SoundIndex + 1) * NiagaraScale);
		}
	}

	// Right
	FootLineTrace(RightFootStepSocket);

	ParticleName = *PhysicalMaterial->GetName();

	Niagara = NiagaraMap.Contains(ParticleName) ? NiagaraMap[ParticleName] : Footstep_Default_Niagara;
	Particle = ParticleMap.Contains(ParticleName) ? ParticleMap[ParticleName] : Footstep_Default_Particle;

	Location = MeshComponent->GetSocketLocation(FootName);
	Rotation = MeshComponent->GetSocketRotation(FootName);
	if (bFootStepParticle)
	{
		UParticleSystemComponent* ParticleSystemComponent = UGameplayStatics::SpawnEmitterAtLocation(
			GetWorld(), Particle, Location,
			Rotation, true);

		ParticleSystemComponent->SetWorldScale3D(FVector::One() * ParticleScale);
	}

	if (bFootStepNiagara)
	{
		UNiagaraComponent* NiagaraComponent = UNiagaraFunctionLibrary::SpawnSystemAtLocation(
			GetWorld(), Niagara, MeshComponent->GetSocketLocation(FootName),
			Rotation,
			FVector::One() * NiagaraScale);

		if (NiagaraComponent)
		{
			NiagaraComponent->SetFloatParameter("User.Size", (SoundIndex + 1) * NiagaraScale);
		}
	}
}

void UFootStepComponent::FootLineTrace(const FName& SocketName)
{
	FHitResult HitResult;
	const FVector Start = MeshComponent->GetSocketLocation(SocketName);
	const FVector End = MeshComponent->GetUpVector() * FootStepTraceHeight + Start;
	GetWorld()->LineTraceSingleByChannel(HitResult, Start, End, ECC_Visibility);

	if (HitResult.bBlockingHit)
	{
		PhysicalMaterial = HitResult.PhysMaterial.Get();
		FootStepLocation = HitResult.Location;
		FootName = SocketName;
	}
}

void UFootStepComponent::FootStep()
{
	if (Animation->GetName().Contains("Walk"))
	{
		SoundIndex = 0;
		ParticleName = *(ParticleName.ToString() + "Walk");
	}
	else if (Animation->GetName().Contains("Jog"))
	{
		SoundIndex = 1;
	}

	if ((SoundIndex == 0 && bSpawnParticleForWalk) || (SoundIndex == 1 && bSpawnParticleForRun))
	{
		ParticleName = *(ParticleName.ToString() + "Jog");
		SpawnFootStepParticle();
	}

	FootStep_Sound = SoundMap.Contains(ParticleName) ? SoundMap[ParticleName] : WalkDefault_Sound;


	// TryDebug();

	UGameplayStatics::PlaySoundAtLocation(GetWorld(), FootStep_Sound, FootStepLocation);
}

void UFootStepComponent::SetAnimation(UAnimSequenceBase* NewAnimation)
{
	Animation = NewAnimation;
}

void UFootStepComponent::OnActorHit_Event(AActor* SelfActor, AActor* OtherActor, FVector NormalImpulse,
                                          const FHitResult& Hit)
{
	PhysicalMaterial = Hit.PhysMaterial.Get();

	if (NormalImpulse.Dot(NormalImpulse.GetSafeNormal()) > ObjectImpactThreshold)
	{
		// Hit
		ParticleName = *PhysicalMaterial->GetName();

		ParticleName = *("Object" + ParticleName.ToString());

		Niagara = NiagaraMap.Contains(ParticleName) ? NiagaraMap[ParticleName] : Object_Default_Niagara;
		Particle = ParticleMap.Contains(ParticleName) ? ParticleMap[ParticleName] : Object_Default_Particle;
		ObjectSound = SoundMap.Contains(ParticleName) ? SoundMap[ParticleName] : ObjectDefault_Sound;

		if (bObjectParticle)
		{
			UParticleSystemComponent* ParticleSystemComponent = UGameplayStatics::SpawnEmitterAtLocation(
				GetWorld(), Particle, Hit.Location, FRotator::ZeroRotator, true);

			ParticleSystemComponent->SetWorldScale3D(FVector::One() * ParticleScale);
		}

		if (bObjectNiagara)
		{
			UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), Niagara, Hit.Location, FRotator::ZeroRotator,
			                                               FVector::One() * NiagaraScale);
		}

		UGameplayStatics::PlaySoundAtLocation(GetWorld(), ObjectSound, Hit.Location);
	}
}
