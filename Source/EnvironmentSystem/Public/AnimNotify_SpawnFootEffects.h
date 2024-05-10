// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotify.h"
#include "AnimNotify_SpawnFootEffects.generated.h"

class UNiagaraSystem;

UENUM()
enum class EFootType
{
	LeftFoot,
	RightFoot
};

/**
 * 
 */
UCLASS()
class ENVIRONMENTSYSTEM_API UAnimNotify_SpawnFootEffects : public UAnimNotify
{
	GENERATED_BODY()

public:
	UAnimNotify_SpawnFootEffects();

protected:
	
	virtual FString GetNotifyName_Implementation() const override;
	virtual void Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference) override;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Dynamic Weather")
	TObjectPtr<UMaterialParameterCollection> WeatherMaterialParameterCollection;

	// Only spawn snowy footprints or rain splashes when the snow strength is above this threshold or puddles have formed on the landscape
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Dynamic Weather", meta = (ClampMin=0, ClampMax=1))
	float SpawnSnowFootprintsThreshold { .5f };
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Dynamic Weather|Snow")
	TObjectPtr<UNiagaraSystem> LeftFootDecalSpawner;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Dynamic Weather|Snow")
	TObjectPtr<UNiagaraSystem> RightFootDecalSpawner;

	// The effect used to spawn splashes when the player steps in a puddle
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Dynamic Weather|Rain")
	TObjectPtr<UNiagaraSystem> RainSplashSpawner;

	// The material used to detect rain splashes
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Dynamic Weather|Rain")
	TObjectPtr<UPhysicalMaterial> RainSplashMaterial;

	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Mesh Details")
	FName LeftFootBoneName;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Mesh Details")
	FName RightFootBoneName;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Notification")
	EFootType CurrentLandingFoot;
	
	TObjectPtr<UMaterialParameterCollectionInstance> WeatherMaterialParameterCollectionInstance;
	
	FName SnowStrengthParameterName { "SnowStrength" };
	FName ShowPuddlesParameterName { "ShowPuddles" };
};
