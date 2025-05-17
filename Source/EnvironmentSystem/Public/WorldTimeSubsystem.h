// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Misc/DateTime.h"
#include "Misc/Timespan.h"
#include "Subsystems/WorldSubsystem.h" 
#include "Subsystems/GameInstanceSubsystem.h"
#include "WorldTimeSubsystem.generated.h"

DECLARE_MULTICAST_DELEGATE_OneParam(FOnHourChangedDelegate, FDateTime);
DECLARE_MULTICAST_DELEGATE_OneParam(FOnDayChangedDelegate, FDateTime);
DECLARE_MULTICAST_DELEGATE_OneParam(FOnWeekChangedDelegate, FDateTime);

/**
 * Manages the passage of (date and) time in the world.
 */
UCLASS()
class ENVIRONMENTSYSTEM_API UWorldTimeSubsystem : public UTickableWorldSubsystem 
{
	GENERATED_BODY()

	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;

	//virtual void OnWorldBeginPlay(UWorld& InWorld);
	
	virtual void Tick(float DeltaTime) override;
	void HandleNotifications(FDateTime OldDate) const;

	FOnHourChangedDelegate OnHourChanged {};
	FOnDayChangedDelegate OnDayChanged {};
	FOnWeekChangedDelegate OnWeekChanged {};
	
private:
	/** How much to advance the time simulation each tick */
	FTimespan TickRate{};
	FDateTime WorldDateTime{};

	float RealWorldTickFrequency { 1.f };
	float TickAccumulator { 0.f };

	void TickInternal(float DeltaTime);
};
