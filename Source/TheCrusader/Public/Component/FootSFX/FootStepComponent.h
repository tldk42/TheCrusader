// Written by jaegang lim

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "FootStepComponent.generated.h"

class UNiagaraSystem;

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class THECRUSADER_API UFootStepComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	void FootStep();

	void SetAnimation(UAnimSequenceBase* NewAnimation);

protected:
	virtual void BeginPlay() override;

private:
	void GetRefData();

	void SpawnFootStepParticle();
	void FootLineTrace(const FName& SocketName);

	UFUNCTION()
	void OnActorHit_Event(AActor* SelfActor, AActor* OtherActor, FVector NormalImpulse, const FHitResult& Hit);

protected:
#pragma region Debug

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bDebugMode;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float DebugDuration;

#pragma endregion Debug

#pragma region Footstep

	UPROPERTY(BlueprintReadOnly, Category = "FootStep")
	int SoundIndex;

	UPROPERTY(BlueprintReadOnly, Category = "FootStep")
	int ParticleIndex;

	UPROPERTY(BlueprintReadOnly, Category = "FootStep")
	FVector FootStepLocation;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "FootStep")
	FName ParticleName;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "FootStep")
	FName FootName;

	UPROPERTY(BlueprintReadOnly, Category = "FootStep")
	USoundBase* FootStep_Sound;

	UPROPERTY(BlueprintReadOnly, Category = "FootStep")
	UAnimSequenceBase* Animation;

	UPROPERTY(BlueprintReadOnly, Category = "FootStep")
	UPhysicalMaterial* PhysicalMaterial;

#pragma endregion Footstep

#pragma region Data

	UPROPERTY(BlueprintReadOnly, Category = "Data")
	ACharacter* CharacterRef;

	UPROPERTY(BlueprintReadOnly, Category = "Data")
	USkeletalMeshComponent* MeshComponent;

	UPROPERTY(BlueprintReadOnly, Category = "Data")
	UObject* ObjectRef;

	UPROPERTY(BlueprintReadOnly, Category = "Data")
	USoundBase* ObjectSound;

#pragma endregion Data

#pragma region Sounds

	UPROPERTY(EditDefaultsOnly ,BlueprintReadWrite, Category = "Sound")
	TMap<FName, USoundBase*> SoundMap;

	UPROPERTY(EditDefaultsOnly ,BlueprintReadWrite, Category = "Sound")
	USoundBase* WalkDefault_Sound;

	UPROPERTY(EditDefaultsOnly ,BlueprintReadWrite, Category = "Sound")
	USoundBase* RunDefault_Sound;

	UPROPERTY(EditDefaultsOnly ,BlueprintReadWrite, Category = "Sound")
	USoundBase* ObjectDefault_Sound;

#pragma endregion Sounds

#pragma region Niagara

	UPROPERTY(BlueprintReadOnly, Category = "Niagara")
	UNiagaraSystem* Niagara;

	UPROPERTY(EditDefaultsOnly ,BlueprintReadWrite, Category = "Niagara")
	TMap<FName, UNiagaraSystem*> NiagaraMap;

	UPROPERTY(EditDefaultsOnly ,BlueprintReadWrite, Category = "Niagara")
	UNiagaraSystem* Footstep_Default_Niagara;

	UPROPERTY(EditDefaultsOnly ,BlueprintReadWrite, Category = "Niagara")
	UNiagaraSystem* Object_Default_Niagara;

#pragma endregion Niagara

#pragma region Particle

	UPROPERTY(BlueprintReadOnly, Category = "Particle")
	UParticleSystem* Particle;

	UPROPERTY(EditDefaultsOnly ,BlueprintReadWrite, Category = "Particle")
	TMap<FName, UParticleSystem*> ParticleMap;

	UPROPERTY(EditDefaultsOnly ,BlueprintReadWrite, Category = "Particle")
	UParticleSystem* Footstep_Default_Particle;

	UPROPERTY(EditDefaultsOnly ,BlueprintReadWrite, Category = "Particle")
	UParticleSystem* Object_Default_Particle;

#pragma endregion Particle

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Edit")
	float ParticleScale;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Edit")
	float NiagaraScale;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Edit")
	float ObjectImpactThreshold;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Edit")
	float FootStepTraceHeight;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Edit")
	bool bOneStepAtATime;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Edit")
	bool bObjectParticle;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Edit")
	bool bFootStepParticle;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Edit")
	bool bObjectNiagara;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Edit")
	bool bFootStepNiagara;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Edit")
	bool bSpawnParticleForWalk;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Edit")
	bool bSpawnParticleForRun;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Edit")
	FName LeftFootStepSocket;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Edit")
	FName RightFootStepSocket;
};
