// Fill out your copyright notice in the Description page of Project Settings.


#include "AnimNotify_SpawnFootEffects.h"

#include "LandscapeProxy.h"
#include "NiagaraFunctionLibrary.h"
#include "Logging/StructuredLog.h"
#include "Materials/MaterialParameterCollectionInstance.h"

UAnimNotify_SpawnFootEffects::UAnimNotify_SpawnFootEffects()
{
#if WITH_EDITORONLY_DATA
	bShouldFireInEditor = false;
#endif
}

FString UAnimNotify_SpawnFootEffects::GetNotifyName_Implementation() const
{
	return "Spawn Footprints";
}

void UAnimNotify_SpawnFootEffects::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
	const FAnimNotifyEventReference& EventReference)
{
	WeatherMaterialParameterCollectionInstance = MeshComp->GetWorld()->GetParameterCollectionInstance(WeatherMaterialParameterCollection);
	if(not WeatherMaterialParameterCollectionInstance)
	{
		UE_LOG(LogTemp, Error, TEXT("Could not find weather material parameters collection in the world"));
		return;
	}

	// Check if snow has accumulated
	float SnowStrength;
	if(not WeatherMaterialParameterCollectionInstance->GetScalarParameterValue(SnowStrengthParameterName, SnowStrength))
	{
		UE_LOG(LogTemp, Error, TEXT("Could not fetch value of SnowStrength"));
		return;
	}

	// Check if it is raining or has rained recently
	float ShowPuddles;
	if(not WeatherMaterialParameterCollectionInstance->GetScalarParameterValue(ShowPuddlesParameterName, ShowPuddles))
	{
		UE_LOG(LogTemp, Error, TEXT("Could not fetch value of SnowStrength"));
		return;
	}

	bool bIsSnowing = SnowStrength >= SpawnSnowFootprintsThreshold; 
	bool bIsRaining = ShowPuddles >= SpawnSnowFootprintsThreshold; 
	
	if(not bIsSnowing && not bIsRaining)
	{
		return;
	}
	
	FVector const TraceStart = MeshComp->GetSocketLocation(CurrentLandingFoot == EFootType::LeftFoot ? LeftFootBoneName : RightFootBoneName);
	FVector const TraceEnd = TraceStart - FVector(0, 0, 100.f);
	
	FHitResult HitResult;
	FCollisionObjectQueryParams ObjectQueryParams(ECC_WorldStatic);

	FCollisionQueryParams CollisionQueryParams;
	CollisionQueryParams.bReturnPhysicalMaterial = true;
	
	bool const bHit = MeshComp->GetWorld()->LineTraceSingleByObjectType(
		HitResult,
		TraceStart,
		TraceEnd,
		ObjectQueryParams,
		CollisionQueryParams);

	// If hit landscape, should be replaced by physical material
	if(bHit && Cast<ALandscapeProxy>(HitResult.HitObjectHandle.FetchActor()))
	{
		if(bIsSnowing)
		{
			if(UNiagaraSystem* System = (CurrentLandingFoot == EFootType::LeftFoot ? LeftFootDecalSpawner : RightFootDecalSpawner))
			{
				UNiagaraFunctionLibrary::SpawnSystemAtLocation(MeshComp->GetWorld(), System, HitResult.Location, MeshComp->GetOwner()->GetActorRotation());
			}	
		}
		else
		{
			if(RainSplashMaterial && HitResult.PhysMaterial.IsValid() && HitResult.PhysMaterial == RainSplashMaterial && RainSplashSpawner)
			{
				UNiagaraFunctionLibrary::SpawnSystemAtLocation(MeshComp->GetWorld(), RainSplashSpawner, HitResult.Location, MeshComp->GetOwner()->GetActorRotation());
			}
		}
	}
}
