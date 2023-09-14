// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class UnrealKinetixSample : ModuleRules
{
	public UnrealKinetixSample(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[]
			{ "Core", "CoreUObject", "Engine", "InputCore", "HeadMountedDisplay", "EnhancedInput", "KinetixRuntime" });
	}
}