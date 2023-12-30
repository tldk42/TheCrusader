using UnrealBuildTool;

public class TCAISystem : ModuleRules
{
	public TCAISystem(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(
			new string[]
			{
				"Core",
			}
		);

		PrivateDependencyModuleNames.AddRange(
			new string[]
			{
				"CoreUObject",
				"Engine",
				"Slate",
				"SlateCore",
				"AIModule",
				"GameplayTasks",
				"NavigationSystem",
				"GameplayAbilities"
			}
		);
	}
}