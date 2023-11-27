// Written by jaegang lim

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Item_Interaction_Actor.generated.h"

class UItemBase;
class USphereComponent;


/** 
 *  전시용 (상호작용) 스태틱 메시 액터
 *  아이템 정보를 가지고 있고
 *  플레이어와 상호작용 기능을 담당
 *  인벤토리에 아이템 정보를 전달한다.
 */
UCLASS()
class THECRUSADER_API AItem_Interaction_Actor : public AActor
{
	GENERATED_BODY()

public:
	AItem_Interaction_Actor();

protected:
	virtual void BeginPlay() override;

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	USceneComponent* SceneComponent;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	UStaticMeshComponent* StaticMesh;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	TSubclassOf<UUserWidget> RadiusWidgetClass;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	USphereComponent* InteractionAreaCollision;

	UPROPERTY(VisibleAnywhere)
	UItemBase* ItemRef;
};
