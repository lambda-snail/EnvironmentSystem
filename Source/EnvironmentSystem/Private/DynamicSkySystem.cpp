// Fill out your copyright notice in the Description page of Project Settings.

#include "DynamicSkySystem.h"

#include "EnvironmentSystemLogging.h"
#include "NiagaraComponent.h"
#include "NiagaraSystem.h"
#include "WeatherDataAssetBase.h"
#include "WorldTimeSubsystem.h"
#include "Components/DirectionalLightComponent.h"
#include "Components/SkyAtmosphereComponent.h"
#include "Components/SkyLightComponent.h"
#include "Components/ExponentialHeightFogComponent.h"
#include "Components/PostProcessComponent.h"
#include "Components/TimelineComponent.h"
#include "Components/VolumetricCloudComponent.h"
#include "Kismet/KismetMaterialLibrary.h"
#include "Kismet/KismetMathLibrary.h"
#include "Logging/StructuredLog.h"
#include "Materials/MaterialParameterCollectionInstance.h"
#include "HAL/Platform.h"

ADynamicSkySystem::ADynamicSkySystem()
{
	// Tick is called by the environment subsystem or manually, not the engine
	PrimaryActorTick.bCanEverTick = false;
	PrimaryActorTick.bStartWithTickEnabled = false;

    Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	SetRootComponent(Root);

	SkySphere = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Sky Sphere"));
	SkySphere->SetupAttachment(Root);
	SkySphere->SetWorldScale3D(FVector(SkySphereScale));
	
	SunDirectionalLight = CreateDefaultSubobject<UDirectionalLightComponent>(TEXT("SunDirectionalLight"));
	SunDirectionalLight->SetupAttachment(Root);
	SunDirectionalLight->SetForwardShadingPriority(1);
	SunDirectionalLight->SetUseTemperature(true);

	MoonDirectionalLight = CreateDefaultSubobject<UDirectionalLightComponent>(TEXT("MoonDirectionalLight"));
	MoonDirectionalLight->SetupAttachment(Root);
	MoonDirectionalLight->Intensity = 1.f;
	MoonDirectionalLight->SetUseTemperature(true);
	
	SkyAtmosphere = CreateDefaultSubobject<USkyAtmosphereComponent>(TEXT("SkyAtmosphere"));
	SkyAtmosphere->SetupAttachment(Root);

	SkyLight = CreateDefaultSubobject<USkyLightComponent>(TEXT("SkyLight"));
	SkyLight->SetupAttachment(Root);
	SkyLight->SetRealTimeCaptureEnabled(true);

	Fog = CreateDefaultSubobject<UExponentialHeightFogComponent>(TEXT("HeightFog"));
	Fog->SetupAttachment(Root);

	VolumetricClouds = CreateDefaultSubobject<UVolumetricCloudComponent>(TEXT("Volumetric Clouds"));
	VolumetricClouds->SetupAttachment(Root);
	
	PostProcessComponent = CreateDefaultSubobject<UPostProcessComponent>(TEXT("PostProcess"));
    PostProcessComponent->SetupAttachment(Root);
	PostProcessComponent->bUnbound = true;

	// Disable auto exposure
	PostProcessComponent->Settings.AutoExposureMinBrightness = .5f;
	PostProcessComponent->Settings.bOverride_AutoExposureMinBrightness = true;
	PostProcessComponent->Settings.AutoExposureMaxBrightness = .5f;
	PostProcessComponent->Settings.bOverride_AutoExposureMaxBrightness = true;

	// WeatherEffectsComponent = CreateDefaultSubobject<UNiagaraComponent>(TEXT("NiagaraComponent"));
	// WeatherEffectsComponent->SetupAttachment(Root);
	// WeatherEffectsComponent->SetAutoActivate(false);
	
	WeatherTransitionAnimationComponent = CreateDefaultSubobject<UTimelineComponent>(TEXT("WeatherTransitionTimeline"));
	WeatherTransitionAnimationComponent->SetPlayRate(.1f); // 10s, hard coded for now
}

void ADynamicSkySystem::StartWeatherAndAnimateTransition()
{
	WeatherTransitionAnimationComponent->Play();
	ToggleWeatherEffects(true);
}

void ADynamicSkySystem::BeginPlay()
{
	Super::BeginPlay();

	InitSubsystems();

	if (UWorldTimeSubsystem* WorldTimeSystem = GetWorld()->GetSubsystem<UWorldTimeSubsystem>())
	{
		WorldTimeSystem->RegisterDynamicSkySystem(this);
	}
	else
	{
		UE_LOGFMT(LogEnvironmentSystem, Warning, "Unable to obtain a reference to the UWorldTimeSubsystem - day and night cycle will be disabled");
	}
	
	// TODO: Stubs to test weather change blending
	if(CurrentWeatherPreset->WeatherType == EWeatherTypes::Snowy || CurrentWeatherPreset->WeatherType == EWeatherTypes::Rainy)
	{
		StartWeatherAndAnimateTransition();
	}
}

void ADynamicSkySystem::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	if (UWorldTimeSubsystem* WorldTimeSystem = GetWorld()->GetSubsystem<UWorldTimeSubsystem>())
	{
		WorldTimeSystem->UnregisterDynamicSkySystem(this);
	}
	
	Super::EndPlay(EndPlayReason);
}

void ADynamicSkySystem::InitSubsystems()
{
	InitSkySphere();
	HandleWeatherSettings();
	HandleCloudMode();
	HandleSunAndMoonRotation();
}

void ADynamicSkySystem::InitSkySphere()
{
	UMaterialInterface* Material = SkySphere->GetMaterial(0);
	if(not Material)
	{
		return;
	}

	SkySphereMaterialInstance = SkySphere->CreateDynamicMaterialInstance(0, Material); // TODO: Should this be called on begin play? If the editor caches this it could be related to the weird behavior that we are observing sometimes
	if(SkySphereMaterialInstance)
	{
		FLinearColor const Params { MoonScale, MoonRotation, MoonBrightness, 1.f };
		SkySphereMaterialInstance->SetVectorParameterValue(MoonSizeMaterialParameterName, Params);
	}
}

void ADynamicSkySystem::OnConstruction(const FTransform& Transform)
{
	InitSubsystems();
}

void ADynamicSkySystem::HandleSunAndMoonRotation()
{
	// Sun rotation between dawn and dusk
	double const SunAngle = UKismetMathLibrary::MapRangeUnclamped(TimeOfDay, DawnTime, DuskTime, static_cast<double>(ESunPositions::SunRise), static_cast<double>(ESunPositions::SunSet));
	SunDirectionalLight->SetWorldRotation(FRotator::MakeFromEuler(FVector{ 0, SunAngle, SunMoonRotationYaw }));

	// Moon rotation after dusk
	if(TimeOfDay > GetTrueDuskTime())
	{
		double const MoonAnglePreMidnight = UKismetMathLibrary::MapRangeUnclamped(TimeOfDay, GetTrueDuskTime(), 24.f, static_cast<double>(EMoonPositions::MoonRise), static_cast<double>(EMoonPositions::Midnight));
		MoonDirectionalLight->SetWorldRotation(FRotator::MakeFromEuler(FVector{ 0, MoonAnglePreMidnight, SunMoonRotationYaw }));	
	}

	// Moon rotation before dawn
	if(TimeOfDay < GetTrueDawnTime())
	{
		double const MoonAnglePostMidnight = UKismetMathLibrary::MapRangeUnclamped(TimeOfDay, 0, GetTrueDawnTime(), static_cast<double>(EMoonPositions::Midnight), static_cast<double>(EMoonPositions::MoonSet));
		MoonDirectionalLight->SetWorldRotation(FRotator::MakeFromEuler(FVector{ 0, MoonAnglePostMidnight, SunMoonRotationYaw }));	
	}
	
	HandleVisibility();
}

void ADynamicSkySystem::HandleVisibility() const
{
	SunDirectionalLight->SetVisibility(IsDaytime());
	MoonDirectionalLight->SetVisibility(not IsDaytime());
}

float ADynamicSkySystem::GetTrueDawnTime() const
{
	return DawnTime - DawnTimeOffset;
}

float ADynamicSkySystem::GetTrueDuskTime() const
{
	return DuskTime + DuskTimeOffset;
}

// TODO: Generalize
void ADynamicSkySystem::ToggleWeatherEffects(bool bShowEffect) const
{
	for(auto const& NC : WeatherEffectsComponents)
	{
		if(NC && NC->GetAsset())
		{
			NC->SetActive(bShowEffect);
		}
	}
	
	// if(WeatherEffectsComponent->GetAsset())
	// {
	// 	WeatherEffectsComponent->SetActive(bShowEffect);
	// }
}

void ADynamicSkySystem::SetSnowStrength(float SnowStrength) const
{
	UMaterialParameterCollectionInstance* ParametersCollection = GetWorld()->GetParameterCollectionInstance(WeatherMaterialParameterCollection);
	ParametersCollection->SetScalarParameterValue(SnowStrengthParameterName, SnowStrength);
}

void ADynamicSkySystem::SetRainStrength(float RainStrength) const
{
	UMaterialParameterCollectionInstance* ParametersCollection = GetWorld()->GetParameterCollectionInstance(WeatherMaterialParameterCollection);
	ParametersCollection->SetScalarParameterValue(ShowPuddlesCollectionParameterName, RainStrength);
}

void ADynamicSkySystem::SetIsSnowing(bool bIsSnowing) const
{
	UMaterialParameterCollectionInstance* ParametersCollection = GetWorld()->GetParameterCollectionInstance(WeatherMaterialParameterCollection);
	ParametersCollection->SetScalarParameterValue(SnowStrengthParameterName, bIsSnowing);
}

// Temporary hack 
// TODO: Animation backwards?
// TODO: Define animation in weather presets?
void ADynamicSkySystem::WeatherAnimationUpdate(float Update)
{
	switch(CurrentWeatherPreset->WeatherType)
	{
	case EWeatherTypes::Snowy:
		SetSnowStrength(Update);
		break;
	case EWeatherTypes::Rainy:
		SetRainStrength(Update);
		break;
	}
}

void ADynamicSkySystem::TickTimeOfDay(float const Time)
{
	// TODO: Add weather change function that checks and sets everything about the weather
	//HandleWeatherSettings();
	//HandleCloudMode();

	TimeOfDay = FMath::Clamp(Time, 0, Midnight); 
	
	HandleSunAndMoonRotation();
}

bool ADynamicSkySystem::IsDaytime() const
{
	return TimeOfDay > GetTrueDawnTime() and TimeOfDay < GetTrueDuskTime();
}

bool ADynamicSkySystem::IsNightTime() const
{
	return not IsDaytime();
}

void ADynamicSkySystem::HandleCloudMode()
{
	if(CurrentWeatherPreset && CurrentWeatherPreset->bShouldHideClouds)
	{
		ToggleClouds2D(false);
		ToggleVolumetricClouds(false);
		return;
	}
	
	switch (CurrentCloudMode)
	{
	case ECloudTypes::None:
		ToggleClouds2D(false);
		ToggleVolumetricClouds(false);
		break;
	case ECloudTypes::Texture2D:
		ToggleClouds2D(true);
		SetCloud2DSettings();

		ToggleVolumetricClouds(false);
		break;
	case ECloudTypes::Volumetric:
		ToggleClouds2D(false);
		
		ToggleVolumetricClouds(true);
		SetVolumetricCloudSettings();
		break;
	}
}

void ADynamicSkySystem::ToggleClouds2D(bool bShouldShow)
{
	if(SkySphereMaterialInstance)
	{
		SkySphereMaterialInstance->SetScalarParameterValue(Clouds2DVisibleMaterialParameterName, static_cast<float>(bShouldShow));
	}
}

void ADynamicSkySystem::SetCloud2DSettings()
{
	if(SkySphereMaterialInstance)
	{
		FLinearColor const Params { Tiling, PanningSpeed, Brightness, IsDaytime() ? DaytimeAtmosphereCloudTint : NighttimeAtmosphereCloudTint };
		SkySphereMaterialInstance->SetVectorParameterValue(Clouds2DSettingsMaterialParameterName, Params);
	}
}

void ADynamicSkySystem::ToggleVolumetricClouds(bool bShouldShow)
{
	VolumetricClouds->SetComponentTickEnabled(bShouldShow);
	VolumetricClouds->SetVisibility(bShouldShow);
}

void ADynamicSkySystem::SetVolumetricCloudSettings()
{
	if(VolumetricCloudMasterMaterial)
	{
		VolumetricCloudMaterialInstance = UKismetMaterialLibrary::CreateDynamicMaterialInstance(GetWorld(), VolumetricCloudMasterMaterial);
		VolumetricClouds->SetMaterial(VolumetricCloudMaterialInstance);
	}

	VolumetricClouds->SetLayerBottomAltitude(VolumetricCloudLayerBottomAltitude);
	VolumetricClouds->SetLayerHeight(VolumetricCloudLayerHeight);

	if(VolumetricCloudMaterialInstance)
	{
		VolumetricCloudMaterialInstance->SetScalarParameterValue(VolumetricCloudSettingsMaterialParameterName, VolumetricCloudPanningSpeed);

		float const CloudBrightness = IsDaytime() ? DayVolumetricCloudBrightness : NightVolumetricCloudBrightness;
		FLinearColor const Tint { VolumetricCloudTint.R, VolumetricCloudTint.G, VolumetricCloudTint.B, CloudBrightness };
		VolumetricCloudMaterialInstance->SetVectorParameterValue(VolumetricCloudAlbedoMaterialParameterName, Tint);
	}
}

void ADynamicSkySystem::SetWeatherEffects()
{
	// TODO: Refactor - move to own member function
	if(WeatherEffectsComponents.Num() < CurrentWeatherPreset->WeatherEffects.Num())
	{
		auto NumComponents = CurrentWeatherPreset->WeatherEffects.Num() - WeatherEffectsComponents.Num();
		for(decltype(NumComponents) i = 0; i < NumComponents; ++i)
		{
			UNiagaraComponent* NC = NewObject<UNiagaraComponent>(this, UNiagaraComponent::StaticClass());
			NC->SetupAttachment(Root);
			NC->RegisterComponent(); 
			
			WeatherEffectsComponents.Add(NC);
		}
	}

	for(auto const& NC : WeatherEffectsComponents)
	{
		NC->Deactivate();
		NC->SetAsset(nullptr);
	}

	for(auto i = 0; i < CurrentWeatherPreset->WeatherEffects.Num(); ++i)
	{
		FWeatherEffectDefinition const& Effect = CurrentWeatherPreset->WeatherEffects[i];
		UNiagaraComponent* NC = WeatherEffectsComponents[i];
		
		if(Effect.WeatherEffects)
		{
			NC->SetAsset(Effect.WeatherEffects);
			
			for(auto [Name, Float] : Effect.WeatherEffectsFloatParameters)
			{
				NC->SetVariableFloat(Name, Float);
			}

			for(auto [Name, Vector] : Effect.WeatherEffectsVectorParameters)
			{
				NC->SetVariableVec3(Name, Vector);
			}
		}
	}

	TogglePuddleMaterialEffects(CurrentWeatherPreset->bShouldShowRainPuddles, CurrentWeatherPreset->bShouldShowRainPuddleRipples);
	
	//WeatherEffectsComponent->SetAsset(CurrentWeatherPreset->WeatherEffects);

	
	// if(CurrentWeatherPreset->WeatherEffects)
	// {
	// 	for(auto [Name, Float] : CurrentWeatherPreset->WeatherEffectsFloatParameters)
	// 	{
	// 		WeatherEffectsComponent->SetVariableFloat(Name, Float);
	// 	}
	//
	// 	for(auto [Name, Vector] : CurrentWeatherPreset->WeatherEffectsVectorParameters)
	// 	{
	// 		WeatherEffectsComponent->SetVariableVec3(Name, Vector);
	// 	}
	// }

	// TogglePuddleMaterialEffects(CurrentWeatherPreset->bShouldShowRainPuddles, CurrentWeatherPreset->bShouldShowRainPuddleRipples);
}

void ADynamicSkySystem::TogglePuddleMaterialEffects(bool bShowPuddles, bool bShowRipples) const
{
	UMaterialParameterCollectionInstance* ParametersCollection = GetWorld()->GetParameterCollectionInstance(WeatherMaterialParameterCollection);
	ParametersCollection->SetScalarParameterValue(ShowPuddlesCollectionParameterName, static_cast<float>(bShowPuddles));
	ParametersCollection->SetScalarParameterValue(ShowRipplesCollectionParameterName, static_cast<float>(bShowRipples));
}

void ADynamicSkySystem::HandleWeatherSettings()
{
	if(not CurrentWeatherPreset)
	{
		return;
	}

	SetWeatherEffects();
	SetWeatherLightProperties();
	
	if(WeatherMaterialParameterCollection && GetWorld())
	{
		switch(CurrentWeatherPreset->WeatherType)
		{
		case EWeatherTypes::Snowy:
			ToggleWeatherEffects(true); // TODO: Generalize name to include rain or any effect
			SetSnowStrength(1.f);
			break;
		case EWeatherTypes::Rainy:
			ToggleWeatherEffects(true); 
			CurrentCloudMode = ECloudTypes::Texture2D; // TODO: Move to data asset
			SetSnowStrength(0.f);
			break;
		default:
			ToggleWeatherEffects(false);
			SetSnowStrength(0.f);
		}
	}

	if(WeatherTransitionCurve)
	{
		WeatherAnimationUpdateCallback.BindDynamic(this, &ADynamicSkySystem::WeatherAnimationUpdate);
		WeatherTransitionAnimationComponent->AddInterpFloat(WeatherTransitionCurve, WeatherAnimationUpdateCallback);
	}
}

void ADynamicSkySystem::SetWeatherLightProperties() const
{
	bool bIsDaytime = IsDaytime();
	if(bIsDaytime)
	{
		SetWeatherLightProperties(CurrentWeatherPreset->DayTimeConfiguration, SunDirectionalLight);
	}
	else
	{
		SetWeatherLightProperties(CurrentWeatherPreset->NightTimeConfiguration, MoonDirectionalLight);
	}

	if(SkySphereMaterialInstance)
	{
		SkySphereMaterialInstance->SetScalarParameterValue(StarsVisibleMaterialParameterName, static_cast<float>(not bIsDaytime && CurrentWeatherPreset->bShouldShowStars));
		SkySphereMaterialInstance->SetScalarParameterValue(MoonVisibleMaterialParameterName, static_cast<float>(not bIsDaytime && CurrentWeatherPreset->bShouldShowMoon));	
	}
}

void ADynamicSkySystem::SetWeatherLightProperties(FWeatherConfiguration const& Configuration, UDirectionalLightComponent* SunOrMoon) const
{
	SunOrMoon->SetIntensity(Configuration.DirectionalLightSettings.Intensity);
	SunOrMoon->SetLightColor(Configuration.DirectionalLightSettings.Color);
	SunOrMoon->SetLightSourceAngle(Configuration.DirectionalLightSettings.SourceAngle);
	SunOrMoon->SetTemperature(Configuration.DirectionalLightSettings.Temperature);

	SkyLight->SetIntensity(Configuration.SkylightSettings.Intensity);
	
	SkyAtmosphere->SetMultiScatteringFactor(Configuration.AtmosphereSettings.MultiScattering);
	SkyAtmosphere->SetRayleighScattering(Configuration.AtmosphereSettings.RaylieghScattering);
	SkyAtmosphere->SetMieScatteringScale(Configuration.AtmosphereSettings.MieScatteringScale);
	SkyAtmosphere->SetMieAbsorptionScale(Configuration.AtmosphereSettings.MieAbsorbtionScale);
	SkyAtmosphere->SetMieAnisotropy(Configuration.AtmosphereSettings.MieAnisotropy);
	SkyAtmosphere->SetAerialPespectiveViewDistanceScale(Configuration.AtmosphereSettings.AerialPerspectiveViewDistance);
	
	Fog->SetVolumetricFogEmissive(Configuration.ExponentialHeightfogSettings.EmissiveScale);
	Fog->SetVolumetricFogExtinctionScale(Configuration.ExponentialHeightfogSettings.ExtinctionScale);
}
