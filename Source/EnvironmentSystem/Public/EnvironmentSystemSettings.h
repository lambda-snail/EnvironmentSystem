// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DeveloperSettings.h"
#include "EnvironmentSystemSettings.generated.h"

/**
 * Settings for the environment system.
 */
UCLASS(Config=Game, DefaultConfig)
class ENVIRONMENTSYSTEM_API UEnvironmentSystemSettings : public UDeveloperSettings
{
	GENERATED_BODY()

public:
	explicit UEnvironmentSystemSettings(FObjectInitializer const& Initializer);
	
	UPROPERTY(EditAnywhere, Config, Category=Test)
	FText TestText{};

	// Each tick, the simulated time will be advanced by this amount.
	UPROPERTY(EditAnywhere, Config, Category=Environment)
	FTimespan TickRate{};
	
	// How many seconds should pass between each update to the weather events queue
	UPROPERTY(EditAnywhere, Config, Category=Environment, meta = (ClampMin=0, UIMin=0))
	float WeatherTickFrequency { 10.f };

	// How many seconds should pass between each update to the time. This governs the day/night
	// cycle and should be used in conjunction with TickRate to control how long a day lasts 
	UPROPERTY(EditAnywhere, Config, Category=Environment, meta = (ClampMin=0, UIMin=0))
	float TimeTickFrequency { 1.f };
};
