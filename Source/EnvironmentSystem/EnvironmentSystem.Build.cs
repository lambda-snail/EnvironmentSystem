// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class EnvironmentSystem : ModuleRules
{
	public EnvironmentSystem(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;
		PublicDependencyModuleNames.AddRange([ "Core", "CoreUObject", "PhysicsCore", "Engine", "Landscape", "InputCore", "EnhancedInput", "NiagaraCore", "Niagara" ]);
		PrivateDependencyModuleNames.AddRange([ "DeveloperSettings" ]);
		CppStandard = CppStandardVersion.Latest;
	}
}
