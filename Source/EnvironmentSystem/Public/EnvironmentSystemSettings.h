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
	
};
