// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class TheCrusader : ModuleRules
{
	public TheCrusader(ReadOnlyTargetRules Target) : base(Target)
	{
		PrivateDependencyModuleNames.AddRange(new string[] { "MotionWarping" });
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(
			new string[]
			{
				"Core", "CoreUObject", "Engine",
				"InputCore", "EnhancedInput",
				"HeadMountedDisplay", "UMG", "SlateCore", "Slate",
				"GameplayAbilities", "GameplayTasks", "GameplayTags",
				"MotionWarping",
			});
		PrivateDependencyModuleNames.AddRange(
			new string[]
			{
				"CinematicCamera",
			});
	}
}