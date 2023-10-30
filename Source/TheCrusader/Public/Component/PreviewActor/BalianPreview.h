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
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, meta = (AllowPrivateAccess = true))
	USkeletalMeshComponent* Body;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, meta = (AllowPrivateAccess = true))
	USkeletalMeshComponent* Face;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, meta = (AllowPrivateAccess = true))
	USkeletalMeshComponent* Torso;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, meta = (AllowPrivateAccess = true))
	USkeletalMeshComponent* Arm;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, meta = (AllowPrivateAccess = true))
	USkeletalMeshComponent* Leg;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, meta = (AllowPrivateAccess = true))
	USkeletalMeshComponent* Feet;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, meta = (AllowPrivateAccess = true))
	USkeletalMeshComponent* Hair;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, meta = (AllowPrivateAccess = true))
	USkeletalMeshComponent* Hat;
};
