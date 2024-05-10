// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/TimelineComponent.h"
#include "GameFramework/Actor.h"
#include "DynamicSkySystem.generated.h"

struct FWeatherConfiguration;
class UWeatherDataAssetBase;

class UVolumetricCloudComponent;
class UPostProcessComponent;
class UDirectionalLightComponent;
class USkyAtmosphereComponent;
class USkyLightComponent;
class UExponentialHeightFogComponent;

class UMaterialParameterCollection;
class UMaterialParameterCollectionInstance;
class UNiagaraComponent;

UENUM()
enum class EMoonPositions
{
	MoonRise = -180,
	Midnight = -90,
	MoonSet = 0
};

UENUM()
enum class ESunPositions
{
	SunRise = 0,
	Noon = -90,
	SunSet = -180
};

UENUM()
enum class ECloudTypes
{
	None = 0,
	Texture2D,
	Volumetric
};

UCLASS()
class ENVIRONMENTSYSTEM_API ADynamicSkySystem : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ADynamicSkySystem();

	virtual void Tick(float DeltaTime) override;

	inline bool IsDaytime() const;
	inline bool IsNightTime() const;

	// Start increasing the snow strength and enable the current weather effect
	void StartWeatherAndAnimateTransition();

	// Immediately go to a snowy landscape without intermediate  transition
	void SetIsSNowing(bool bIsSNowing) const;
	
	static constexpr float Midnight = 24.f;
protected:
	virtual void BeginPlay() override;
	virtual void OnConstruction(const FTransform& Transform) override;

	TObjectPtr<USceneComponent> Root;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TObjectPtr<UStaticMeshComponent> SkySphere;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TObjectPtr<UDirectionalLightComponent> SunDirectionalLight;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TObjectPtr<UDirectionalLightComponent> MoonDirectionalLight;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TObjectPtr<USkyAtmosphereComponent> SkyAtmosphere;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TObjectPtr<USkyLightComponent> SkyLight;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TObjectPtr<UExponentialHeightFogComponent> Fog;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TObjectPtr<UVolumetricCloudComponent> VolumetricClouds;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TObjectPtr<UPostProcessComponent> PostProcessComponent;

	// UPROPERTY(EditAnywhere, BlueprintReadOnly)
	// TObjectPtr<UNiagaraComponent> WeatherEffectsComponent;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TArray<TObjectPtr<UNiagaraComponent>> WeatherEffectsComponents;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TObjectPtr<UTimelineComponent> WeatherTransitionAnimationComponent;


	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Dynamic Sky|Basic Settings", meta = (ClampMin=0, ClampMax=24))
	float TimeOfDay { 9.f };

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Dynamic Sky|Basic Settings", meta = (ClampMin=0, ClampMax=24))
	float DawnTime { 5.f };
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Dynamic Sky|Basic Settings")
	float DawnTimeOffset { .2f };
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Dynamic Sky|Basic Settings", meta = (ClampMin=0, ClampMax=24))
	float DuskTime { 19.f };
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Dynamic Sky|Basic Settings")
	float DuskTimeOffset { .2f };

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Dynamic Sky|Basic Settings", meta = (ClampMin=0, ClampMax=360))
	float SunMoonRotationYaw { .0f };

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Dynamic Sky|Basic Settings")
	TObjectPtr<UWeatherDataAssetBase> CurrentWeatherPreset;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Dynamic Sky|Basic Settings")
	TObjectPtr<UMaterialParameterCollection> WeatherMaterialParameterCollection;

	// Currently only affects how snow blends in onto the landscape
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Dynamic Sky|Basic Settings")
	TObjectPtr<UCurveFloat> WeatherTransitionCurve;


	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Dynamic Sky|Basic Settings|Moon", meta = (ClampMin=0))
	float MoonlightIntensity { 1.f };

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Dynamic Sky|Basic Settings|Moon", meta = (ClampMin=0))
	float MoonlightTemperature { 8500.f };

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Dynamic Sky|Basic Settings|Moon")
    FLinearColor MoonlightColor;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Dynamic Sky|Basic Settings|Moon", meta = (ClampMin=0))
	float MoonlightSourceAngle { 0.f };

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Dynamic Sky|Basic Settings|Moon", meta = (ClampMin=0))
	float MoonScale { .1f };

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Dynamic Sky|Basic Settings|Moon", meta = (ClampMin=0))
	float MoonRotation { 1.f };

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Dynamic Sky|Basic Settings|Moon", meta = (ClampMin=0))
	float MoonBrightness { 1.f };


	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Dynamic Sky|Sky Sphere", meta = (ClampMin=0))
	float SkySphereScale { 100000.f };

	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Dynamic Sky|Clouds")
	ECloudTypes CurrentCloudMode; // TODO: Move some cloud settings to weather asset? Especially tint/brightness so we can controll that for darker weather types

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Dynamic Sky|Clouds|2D", meta = (ClampMin=0, ClampMax=1, EditCondition="CurrentCloudMode == ECloudTypes::Texture2D"))
	float Tiling { 3.f };

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Dynamic Sky|Clouds|2D", meta = (ClampMin=0, ClampMax=1, EditCondition="CurrentCloudMode == ECloudTypes::Texture2D"))
	float PanningSpeed { 1.f };

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Dynamic Sky|Clouds|2D", meta = (ClampMin=0, ClampMax=1, EditCondition="CurrentCloudMode == ECloudTypes::Texture2D"))
	float Brightness { 1.f };

	// The extent to which the atmosphere affects the colors of the clouds during daytime
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Dynamic Sky|Clouds|2D", meta = (ClampMin=0, ClampMax=1, EditCondition="CurrentCloudMode == ECloudTypes::Texture2D"))
	float DaytimeAtmosphereCloudTint { .1f };

	// The extent to which the atmosphere affects the colors of the clouds during night time
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Dynamic Sky|Clouds|2D", meta = (ClampMin=0, ClampMax=1, EditCondition="CurrentCloudMode == ECloudTypes::Texture2D"))
	float NighttimeAtmosphereCloudTint { .95f };

	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Dynamic Sky|Clouds|Volumetric", meta = (EditCondition="CurrentCloudMode == ECloudTypes::Volumetric"))
	TObjectPtr<UMaterialInterface> VolumetricCloudMasterMaterial;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Dynamic Sky|Clouds|Volumetric", meta = (ClampMin=0, ClampMax=20, EditCondition="CurrentCloudMode == ECloudTypes::Volumetric"))
	float VolumetricCloudLayerBottomAltitude { 5.f };

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Dynamic Sky|Clouds|Volumetric", meta = (ClampMin=0.1, ClampMax=20, EditCondition="CurrentCloudMode == ECloudTypes::Volumetric"))
	float VolumetricCloudLayerHeight { 8.f };

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Dynamic Sky|Clouds|Volumetric", meta = (ClampMin=0, EditCondition="CurrentCloudMode == ECloudTypes::Volumetric"))
	float VolumetricCloudPanningSpeed { .1f };
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Dynamic Sky|Clouds|Volumetric", meta = (ClampMin=0, ClampMax=1, EditCondition="CurrentCloudMode == ECloudTypes::Volumetric"))
	float DayVolumetricCloudBrightness { 1.f };
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Dynamic Sky|Clouds|Volumetric", meta = (ClampMin=0, ClampMax=1, EditCondition="CurrentCloudMode == ECloudTypes::Volumetric"))
	float NightVolumetricCloudBrightness { .2f };
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Dynamic Sky|Clouds|Volumetric", meta = (EditCondition="CurrentCloudMode == ECloudTypes::Volumetric"))
	FLinearColor VolumetricCloudTint;
	
private:

	void InitSubsystems();
	void InitSkySphere();
	void HandleSunAndMoonRotation();
	void HandleVisibility() const;
	void HandleCloudMode();

	void ToggleClouds2D(bool bShouldShow);
	void SetCloud2DSettings();

	void ToggleVolumetricClouds(bool bShouldShow);
	void SetVolumetricCloudSettings();

	void HandleWeatherSettings();
	void SetWeatherEffects();
	void SetWeatherLightProperties() const;
	void SetWeatherLightProperties(FWeatherConfiguration const& Configuration, UDirectionalLightComponent* SunOrMoon) const;
	
	inline float GetTrueDawnTime() const;
	inline float GetTrueDuskTime() const;

	inline void ToggleWeatherEffects(bool bShowEffect) const;
	inline void SetSnowStrength(float SnowStrength) const;
	inline void SetRainStrength(float RainStrength) const;
	
	inline void TogglePuddleMaterialEffects(bool bShowPuddles, bool bShowRipples) const;

	
	TObjectPtr<UMaterialInstanceDynamic> SkySphereMaterialInstance;
	TObjectPtr<UMaterialInstanceDynamic> VolumetricCloudMaterialInstance;

	FName StarsVisibleMaterialParameterName { "AreStarsVisible" };
	FName MoonVisibleMaterialParameterName { "IsMoonVisible" };
	FName MoonSizeMaterialParameterName { "MoonFundamentalSettings" };

	FName Clouds2DVisibleMaterialParameterName { "Are2DCloudsVisible" };
	FName Clouds2DSettingsMaterialParameterName { "Cloud2DSettings" };

	FName VolumetricCloudSettingsMaterialParameterName { "PanningSpeed" };
	FName VolumetricCloudAlbedoMaterialParameterName { "CloudAlbedo" };

	// Parameter name for weather material collection
	FName SnowStrengthParameterName { "SnowStrength" };  
	FName ShowPuddlesCollectionParameterName { "ShowPuddles" }; // TODO: Change this to PuddleStrength, RainStrength or similar?
	FName ShowRipplesCollectionParameterName { "ShowRipples" };
	

	FOnTimelineFloat WeatherAnimationUpdateCallback;

	UFUNCTION()
	void WeatherAnimationUpdate(float Update);
};
