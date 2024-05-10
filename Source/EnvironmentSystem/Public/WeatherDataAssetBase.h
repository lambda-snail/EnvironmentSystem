// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "NiagaraTypes.h"
#include "WeatherTypes.h"
#include "Engine/DataAsset.h"
#include "WeatherDataAssetBase.generated.h"

class UNiagaraSystem;

USTRUCT(Blueprintable)
struct ENVIRONMENTSYSTEM_API FDirectionalLightSettings
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (ClampMin=0, ClampMax=150))
	float Intensity { 7.f };

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FLinearColor Color  { 1.f, 1.f, 1.f };

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (ClampMin=0, ClampMax=5))
	float SourceAngle { .5357f };

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (ClampMin=0, ClampMax=12000))
	float Temperature { 6500.f };
};

USTRUCT(Blueprintable)
struct ENVIRONMENTSYSTEM_API FSkylightSettings
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (ClampMin=0, ClampMax=50000))
	float Intensity { 1.f };
};

USTRUCT(Blueprintable)
struct ENVIRONMENTSYSTEM_API FAtmosphereSettings
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (ClampMin=0, ClampMax=1))
	float MultiScattering { 1.f };

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FLinearColor RaylieghScattering { .17f, .41f, 1.f };

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (ClampMin=0, ClampMax=5))
	float MieScatteringScale { .003996f };
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (ClampMin=0, ClampMax=5))
	float MieAbsorbtionScale { .00044f };

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (ClampMin=0, ClampMax=.999))
	float MieAnisotropy { .8f };

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (ClampMin=0, ClampMax=3))
	float AerialPerspectiveViewDistance { 1.f };
};

USTRUCT(Blueprintable)
struct ENVIRONMENTSYSTEM_API FExponentialHeightfogSettings
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FLinearColor EmissiveScale  { 0.f, 0.f, 0.f, 0.f };
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (ClampMin=0, ClampMax=10))
	float ExtinctionScale  { .5f };
};

// Determines the light and atmosphere parameters for a weather type.
USTRUCT(Blueprintable)
struct ENVIRONMENTSYSTEM_API FWeatherConfiguration
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FSkylightSettings SkylightSettings;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FDirectionalLightSettings DirectionalLightSettings;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FExponentialHeightfogSettings ExponentialHeightfogSettings;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FAtmosphereSettings AtmosphereSettings;
};

USTRUCT(Blueprintable)
struct ENVIRONMENTSYSTEM_API FWeatherEffectDefinition
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Effects")
	TObjectPtr<UNiagaraSystem> WeatherEffects;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Effects", meta = (EditCondition="WeatherEffects != nullptr"))
	TMap<FName, float> WeatherEffectsFloatParameters;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Effects", meta = (EditCondition="WeatherEffects != nullptr"))
	TMap<FName, FVector> WeatherEffectsVectorParameters;
};

/**
 * Stores atmosphere and weather data for various weather types.
 */
UCLASS()
class ENVIRONMENTSYSTEM_API UWeatherDataAssetBase : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:
	virtual FPrimaryAssetId GetPrimaryAssetId() const override { return FPrimaryAssetId("AssetItems", GetFName()); }

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	EWeatherTypes WeatherType { EWeatherTypes::Sunny };

	// Some weathers with a lot of fog may not need the clouds turned on. This flags disables all clouds.
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	bool bShouldHideClouds { false };

	// Light and atmosphere settings for daytime for this weather. Note that directional light here refers to the sun.
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Environment")
	FWeatherConfiguration DayTimeConfiguration;

	// Light and atmosphere settings for daytime for this weather. Note that directional light here refers to the moon.
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Environment")
	FWeatherConfiguration NightTimeConfiguration;

	// Set to true to display stars during night. Stars are always disabled during the day. 
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Environment|Sky")
	bool bShouldShowStars { true };

	// Set to true to display the moon during night. The moon is always disabled during the day.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Environment|Sky")
	bool bShouldShowMoon { true };

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Effects|Niagara")
	TArray<FWeatherEffectDefinition> WeatherEffects;
	
	// Set to true to enable rain puddles in the landscape material
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Effects|Material")
	bool bShouldShowRainPuddles;

	// Set to true to enable ripple effects in the landscape material - does nothing if bShouldShowRainPuddles == false
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Effects|Material")
	bool bShouldShowRainPuddleRipples;
};
