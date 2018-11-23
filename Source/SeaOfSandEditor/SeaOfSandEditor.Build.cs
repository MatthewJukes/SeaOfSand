// Fill out your copyright notice in the Description page of Project Settings.

using UnrealBuildTool;

public class SeaOfSandEditor : ModuleRules
{
	public SeaOfSandEditor(ReadOnlyTargetRules Target) : base(Target)
	{
        PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

        PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore" });

        PublicIncludePaths.AddRange(new string[] { "SeaOfSandEditor/Public" });
        PrivateIncludePaths.AddRange(new string[] { "SeaOfSandEditor/Private" });
        PublicDependencyModuleNames.AddRange(new string[] { "SeaOfSand" });
        PrivateDependencyModuleNames.AddRange(new string[] {  });
        PrivateDependencyModuleNames.AddRange(new string[] { "Slate", "SlateCore", "UnrealEd", "PropertyEditor" });
        PrivateIncludePathModuleNames.AddRange(new string[] {  });
        DynamicallyLoadedModuleNames.AddRange(new string[] {  });
	}
}
