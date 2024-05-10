// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class EnvironmentSystem : ModuleRules
{
	public EnvironmentSystem(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;
		PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "PhysicsCore", "Engine", "Landscape", "InputCore", "EnhancedInput", "NiagaraCore", "Niagara" });
		CppStandard = CppStandardVersion.Cpp20;
	}
}
