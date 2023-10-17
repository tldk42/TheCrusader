// Written by jaegang lim

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BalianPreview.generated.h"

UCLASS()
class THECRUSADER_API ABalianPreview : public AActor
{
	GENERATED_BODY()

public:
	ABalianPreview();

protected:
	virtual void BeginPlay() override;

private:
	UPROPERTY(EditDefaultsOnly)
	USceneComponent* SceneComponent;
	
	UPROPERTY(EditDefaultsOnly)
	USceneCaptureComponent2D* SceneCaptureComponent2D;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = true))
	USkeletalMeshComponent* CharacterMesh;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = true))
	USkeletalMeshComponent* Face;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = true))
	USkeletalMeshComponent* Torso;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = true))
	USkeletalMeshComponent* Arm;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = true))
	USkeletalMeshComponent* Leg;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = true))
	USkeletalMeshComponent* Feet;
};
