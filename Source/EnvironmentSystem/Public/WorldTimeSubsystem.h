// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "WorldTimeSubsystem.generated.h"

/**
 * Manages the passage of (date and) time in the world.
 */
UCLASS()
class ENVIRONMENTSYSTEM_API UWorldTimeSubsystem : public UWorldSubsystem
{
	GENERATED_BODY()

	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;
};
