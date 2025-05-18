// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "SeasonDataAssetBase.generated.h"

class UWeatherDataAssetBase;

#define LOCTEXT_NAMESPACE "EnvironmentSystem"

USTRUCT(Blueprintable)
struct ENVIRONMENTSYSTEM_API FWeatherInfo
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FText Name;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TObjectPtr<UWeatherDataAssetBase> Weather;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (UIMin=0, ClampMin=0, UIMax=1.f, ClampMax=1.f))
	float Probability {};

	// TODO: Icon in UI - e.g. when displaying forecast
};

/**
 * Store season data.
 */
UCLASS()
class ENVIRONMENTSYSTEM_API USeasonDataAssetBase : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:
	virtual FPrimaryAssetId GetPrimaryAssetId() const override { return FPrimaryAssetId("AssetItems", GetFName()); }

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TObjectPtr<UWeatherDataAssetBase> DefaultWeather;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TArray<FWeatherInfo> WeatherPool;

	// TODO: Icon in UI
};

#undef LOCTEXT_NAMESPACE

UCLASS()
class ENVIRONMENTSYSTEM_API USeasonChangeDataAssetBase : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:
	virtual FPrimaryAssetId GetPrimaryAssetId() const override { return FPrimaryAssetId("AssetItems", GetFName()); }
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TArray<TObjectPtr<USeasonDataAssetBase>> Seasons;
};

// /**
//  * Stores atmosphere and weather data for various weather types.
//  */
// UCLASS()
// class ENVIRONMENTSYSTEM_API UWeatherDataAssetBase : public UPrimaryDataAsset
// {
// 	GENERATED_BODY()
//
// public:
// 	virtual FPrimaryAssetId GetPrimaryAssetId() const override { return FPrimaryAssetId("AssetItems", GetFName()); }
//
// 	UPROPERTY(EditAnywhere, BlueprintReadOnly)
// 	EWeatherTypes WeatherType { EWeatherTypes::Sunny };
//
// 	// Some weathers with a lot of fog may not need the clouds turned on. This flags disables all clouds.
// 	UPROPERTY(EditAnywhere, BlueprintReadOnly)
// 	bool bShouldHideClouds { false };
//
// 	// Light and atmosphere settings for daytime for this weather. Note that directional light here refers to the sun.
// 	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Environment")
// 	FWeatherConfiguration DayTimeConfiguration;
//
// 	// Light and atmosphere settings for daytime for this weather. Note that directional light here refers to the moon.
// 	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Environment")
// 	FWeatherConfiguration NightTimeConfiguration;
//
// 	// Set to true to display stars during night. Stars are always disabled during the day. 
// 	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Environment|Sky")
// 	bool bShouldShowStars { true };
//
// 	// Set to true to display the moon during night. The moon is always disabled during the day.
// 	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Environment|Sky")
// 	bool bShouldShowMoon { true };
//
// 	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Effects|Niagara")
// 	TArray<FWeatherEffectDefinition> WeatherEffects;
// 	
// 	// Set to true to enable rain puddles in the landscape material
// 	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Effects|Material")
// 	bool bShouldShowRainPuddles;
//
// 	// Set to true to enable ripple effects in the landscape material - does nothing if bShouldShowRainPuddles == false
// 	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Effects|Material")
// 	bool bShouldShowRainPuddleRipples;
// };