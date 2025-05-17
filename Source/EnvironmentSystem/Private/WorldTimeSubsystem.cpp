#include "WorldTimeSubsystem.h"
#include "EnvironmentSystemSettings.h"
#include "EnvironmentSystemLogging.h"
#include "Logging/StructuredLog.h"

void UWorldTimeSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	UEnvironmentSystemSettings const* Settings = GetDefault<UEnvironmentSystemSettings>();
	if (Settings)
	{
		RealWorldTickFrequency = FMath::Abs( Settings->RealWorldTickFrequency );
		TickRate = Settings->TickRate;
	}
	else
	{
		UE_LOGFMT(EnvironmentSystem, Error, "Unable to get instance of UEnvironmentSystemSettings. This is required for the environmemt system to work.");
	}
}

void UWorldTimeSubsystem::Deinitialize()
{
	Super::Deinitialize();
}

void UWorldTimeSubsystem::Tick(float const DeltaTime)
{
	TickAccumulator += DeltaTime;
	if (TickAccumulator < RealWorldTickFrequency)
	{
		return;
	}

	TickInternal(DeltaTime);
	
	TickAccumulator = 0;
}

void UWorldTimeSubsystem::HandleNotifications(FDateTime const OldDate) const
{
	if (OldDate.GetHour() == WorldDateTime.GetHour() and OnHourChanged.IsBound())
	{
		OnHourChanged.Broadcast(WorldDateTime);
	}
	
	if (OldDate.GetDayOfWeek() == WorldDateTime.GetDayOfWeek() and OnDayChanged.IsBound())
	{
		OnDayChanged.Broadcast(WorldDateTime);
	}

	if (OldDate.GetDayOfWeek() == EDayOfWeek::Sunday
		and WorldDateTime.GetDayOfWeek() == EDayOfWeek::Monday
		and OnWeekChanged.IsBound())
	{
		OnWeekChanged.Broadcast(WorldDateTime);
	}

	// TODO: Notify season change
}

void UWorldTimeSubsystem::TickInternal(float const DeltaTime)
{
	FDateTime const OldDate = WorldDateTime;
	WorldDateTime += TickRate;

	HandleNotifications(OldDate);
}
