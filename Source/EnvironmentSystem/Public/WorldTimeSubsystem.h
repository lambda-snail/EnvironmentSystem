// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Misc/DateTime.h"
#include "Misc/Timespan.h"
#include "Subsystems/WorldSubsystem.h" 
#include "WorldTimeSubsystem.generated.h"

class ADynamicSkySystem;

DECLARE_MULTICAST_DELEGATE_OneParam(FOnHourChangedDelegate, FDateTime);
DECLARE_MULTICAST_DELEGATE_OneParam(FOnDayChangedDelegate, FDateTime);
DECLARE_MULTICAST_DELEGATE_OneParam(FOnWeekChangedDelegate, FDateTime);

DECLARE_MULTICAST_DELEGATE_OneParam(FScheduledCallback, FDateTime);

/**
 * Manages the passage of (date and) time in the world.
 */
UCLASS()
class ENVIRONMENTSYSTEM_API UWorldTimeSubsystem : public UTickableWorldSubsystem 
{
	GENERATED_BODY()

public:
	void RegisterDynamicSkySystem(TObjectPtr<ADynamicSkySystem> DynamicSkySystem);
	void UnregisterDynamicSkySystem(ADynamicSkySystem* DynamicSkySystem);
	// void StartSimulation();

	virtual TStatId GetStatId() const override { return {}; };
	virtual bool IsTickable() const override;
	void ReadSettings();

protected:
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

	float TimeTickFrequency { 1.f };
	float WeatherTickFrequency { 10.f };
	float TimeTickAccumulator { 0.f };
	float WeatherTickAccumulator { 0.f };

	void TickTimeChanges(float DeltaTime);
	void TickWeatherChanges(float DeltaTime);

	struct FScheduledCallbackInfo
	{
		FScheduledCallback Callback{}; // Will this work? Use TFunction instead?
		FDateTime Timing{};
	};
	TArray<FScheduledCallbackInfo> ScheduledCallbacks{};
	
	UPROPERTY()
	TObjectPtr<ADynamicSkySystem> CurrentDynamicSkySystem;
};
