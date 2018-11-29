// Fill out your copyright notice in the Description page of Project Settings.

using UnrealBuildTool;

public class SeaOfSandEditor : ModuleRules
{
	public SeaOfSandEditor(ReadOnlyTargetRules Target) : base(Target)
	{
        PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

        PublicDependencyModuleNames.AddRange(new string[] 
        {
            "Core",
            "CoreUObject",
            "Engine",
            "Slate",
            "UnrealEd",
            "SeaOfSand" });

        PrivateDependencyModuleNames.AddRange(new string[] 
        {
             "InputCore",
             "SlateCore",
             "PropertyEditor",
             "LevelEditor"
        });


        PublicIncludePaths.AddRange(new string[] { "SeaOfSandEditor/Public" });
        PrivateIncludePaths.AddRange(new string[] { "SeaOfSandEditor/Private" });

        PrivateIncludePathModuleNames.AddRange(new string[] {  });
        DynamicallyLoadedModuleNames.AddRange(new string[] {  });
	}
}
