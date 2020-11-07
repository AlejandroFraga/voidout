// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;
using System.Collections.Generic;

public class VoidoutTarget : TargetRules
{
	public VoidoutTarget( TargetInfo Target) : base(Target)
	{
		Type = TargetType.Game;
		bUseUnityBuild = true; // Whether to unify C++ code into larger files for faster compilation.
		bForceUnityBuild = true; // Whether to force C++ source files to be combined into larger files for faster compilation.
		bAdaptiveUnityCreatesDedicatedPCH = true; // Creates a dedicated PCH for each source file in the working set, allowing faster iteration on cpp-only changes.
		bAdaptiveUnityEnablesEditAndContinue = true; // Creates a dedicated PCH for each source file in the working set, allowing faster iteration on cpp-only changes.
		ExtraModuleNames.AddRange( new string[] { "Voidout" } );
		DefaultBuildSettings = BuildSettingsVersion.V2;
		//CppStandard = CppStandardVersion.Cpp17; // SET STANDARD TO 17!!!!!!!
	}
}
