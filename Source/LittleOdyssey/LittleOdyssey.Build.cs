// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class LittleOdyssey : ModuleRules
{
	public LittleOdyssey(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[] {
			"Core",
			"CoreUObject",
			"Engine",
			"InputCore",
			"EnhancedInput",
			"AIModule",
			"StateTreeModule",
			"GameplayStateTreeModule",
			"UMG",
			"Slate"
		});

		PrivateDependencyModuleNames.AddRange(new string[] { });

		PublicIncludePaths.AddRange(new string[] {
			"LittleOdyssey",
			"LittleOdyssey/Variant_Platforming",
			"LittleOdyssey/Variant_Platforming/Animation",
			"LittleOdyssey/Variant_Combat",
			"LittleOdyssey/Variant_Combat/AI",
			"LittleOdyssey/Variant_Combat/Animation",
			"LittleOdyssey/Variant_Combat/Gameplay",
			"LittleOdyssey/Variant_Combat/Interfaces",
			"LittleOdyssey/Variant_Combat/UI",
			"LittleOdyssey/Variant_SideScrolling",
			"LittleOdyssey/Variant_SideScrolling/AI",
			"LittleOdyssey/Variant_SideScrolling/Gameplay",
			"LittleOdyssey/Variant_SideScrolling/Interfaces",
			"LittleOdyssey/Variant_SideScrolling/UI"
		});

		// Uncomment if you are using Slate UI
		// PrivateDependencyModuleNames.AddRange(new string[] { "Slate", "SlateCore" });

		// Uncomment if you are using online features
		// PrivateDependencyModuleNames.Add("OnlineSubsystem");

		// To include OnlineSubsystemSteam, add it to the plugins section in your uproject file with the Enabled attribute set to true
	}
}
