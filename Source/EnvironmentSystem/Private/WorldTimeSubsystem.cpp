#include "WorldTimeSubsystem.h"

#include "DynamicSkySystem.h"
#include "EnvironmentSystemSettings.h"
#include "EnvironmentSystemLogging.h"
#include "Logging/StructuredLog.h"

void UWorldTimeSubsystem::RegisterDynamicSkySystem(TObjectPtr<ADynamicSkySystem> DynamicSkySystem)
{
	CurrentDynamicSkySystem = DynamicSkySystem;
	ReadSettings();
}

void UWorldTimeSubsystem::UnregisterDynamicSkySystem(ADynamicSkySystem* DynamicSkySystem)
{
	if (CurrentDynamicSkySystem == DynamicSkySystem)
	{
		CurrentDynamicSkySystem = {};
	}
}

bool UWorldTimeSubsystem::IsTickable() const
{
	return CurrentDynamicSkySystem != nullptr;
}

void UWorldTimeSubsystem::ReadSettings()
{
	UEnvironmentSystemSettings const* Settings = GetDefault<UEnvironmentSystemSettings>();
	if (Settings)
	{
		TimeTickFrequency = FMath::Abs( Settings->TimeTickFrequency );
		WeatherTickFrequency = FMath::Abs( Settings->WeatherTickFrequency );
		TickRate = Settings->TickRate;
	}
	else
	{
		UE_LOGFMT(LogEnvironmentSystem, Error, "Unable to get instance of UEnvironmentSystemSettings. This is required for the environment system to work.");
	}
	
	WorldDateTime = {};
	WorldDateTime += FTimespan::FromHours(12);
	
	UE_LOGFMT(LogEnvironmentSystem, Display, "Read subsystem settings");
}

void UWorldTimeSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
	
	ReadSettings();

	ScheduledCallbacks.Reserve(16);
	ScheduledCallbacks.Heapify([](FScheduledCallbackInfo const& A, FScheduledCallbackInfo const& B) { return A.Timing < B.Timing; });
}

void UWorldTimeSubsystem::Deinitialize()
{
	Super::Deinitialize();
}

void UWorldTimeSubsystem::Tick(float const DeltaTime)
{
	TimeTickAccumulator += DeltaTime;
	if (TimeTickAccumulator >= TimeTickFrequency)
	{
		TickTimeChanges(DeltaTime);
		TimeTickAccumulator = 0;
	}
	
	WeatherTickAccumulator += DeltaTime;
	if (WeatherTickAccumulator >= WeatherTickFrequency)
	{
		TickWeatherChanges(DeltaTime);
		WeatherTickAccumulator = 0;
	}
}

void UWorldTimeSubsystem::HandleNotifications(FDateTime const OldDate) const
{
	if (OldDate.GetHour() != WorldDateTime.GetHour() and OnHourChanged.IsBound())
	{
		OnHourChanged.Broadcast(WorldDateTime);
	}
	
	if (OldDate.GetDayOfWeek() != WorldDateTime.GetDayOfWeek() and OnDayChanged.IsBound())
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

void UWorldTimeSubsystem::TickTimeChanges(float const DeltaTime)
{
	FDateTime const PreviousDateTime = WorldDateTime;
	WorldDateTime += TickRate; // TODO: Scale with delta time?

	CurrentDynamicSkySystem->TickTimeOfDay(static_cast<float>(WorldDateTime.GetHour()) + WorldDateTime.GetMinute() / 60.f);
	HandleNotifications(PreviousDateTime);

	UE_LOGFMT(LogEnvironmentSystem, Display, "TOD: {Time}", static_cast<float>(WorldDateTime.GetHour()) + WorldDateTime.GetMinute() / 60.f);
	UE_LOGFMT(LogEnvironmentSystem, Display, "Current time: {Hours}:{Minutes}", WorldDateTime.GetHour(), WorldDateTime.GetMinute());
	
	if (PreviousDateTime.GetHour() != WorldDateTime.GetHour() and CurrentDynamicSkySystem)
	{
		UE_LOGFMT(LogEnvironmentSystem, Display, "Current time: {Hours}:{Minutes}", WorldDateTime.GetHour(), WorldDateTime.GetMinute());
	}
}

void UWorldTimeSubsystem::TickWeatherChanges(float DeltaTime)
{
	// TODO: Check for  weather changes, generate new weather events
}
