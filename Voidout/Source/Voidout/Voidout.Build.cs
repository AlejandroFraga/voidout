// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class Voidout : ModuleRules
{
	public Voidout(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;  // Set in build.cs files to enables IWYU-style PCH model. See https://docs.unrealengine.com/en-US/Programming/BuildTools/UnrealBuildTool/IWYU/index.html. (Previously: PCHUsageMode.UseSharedPCHs).
		bEnforceIWYU = true;
		bEnableExceptions = true;
		bLegacyPublicIncludePaths = true; // Omits subfolders from public include paths to reduce compiler command line length. (Previously: true).
		ShadowVariableWarningLevel = WarningLevel.Error; // Treats shadowed variable warnings as errors. (Previously: WarningLevel.Warning).

		/*
		 Erased Modules:
			InputCore
			HeadMountedDisplay
		 */
		PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "NavigationSystem", "AIModule", "UMG", "Slate", "SlateCore", "GameplayTasks", "LevelSequence", "TimeManagement", "MovieScene", "InputCore" });
        PrivateDependencyModuleNames.AddRange(new string[] { "FMODStudio" });

		// PCHUsage = PCHUsageMode.NoSharedPCHs;
		// PrivatePCHHeaderFile = "$pch.h";
		// CppStandard = CppStandardVersion.Cpp17;
	}
}
