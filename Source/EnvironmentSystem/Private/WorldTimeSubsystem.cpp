// Fill out your copyright notice in the Description page of Project Settings.


#include "WorldTimeSubsystem.h"

#include "EnvironmentSystemSettings.h"
#include "Logging/StructuredLog.h"

void UWorldTimeSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	UEnvironmentSystemSettings const* Settings = GetDefault<UEnvironmentSystemSettings>();
	if (Settings)
	{
		UE_LOGFMT(LogTemp, Warning, "Settings text: {T}", Settings->TestText.ToString());	
	}
}

void UWorldTimeSubsystem::Deinitialize()
{
	Super::Deinitialize();
}
