// Fill out your copyright notice in the Description page of Project Settings.

using UnrealBuildTool;
using System.Collections.Generic;

public class StudyGameTarget : TargetRules
{
	public StudyGameTarget(TargetInfo Target) : base(Target)
	{
		Type = TargetType.Game;
		IncludeOrderVersion = EngineIncludeOrderVersion.Latest;
		DefaultBuildSettings = BuildSettingsVersion.V2;
		CppStandard = CppStandardVersion.Cpp17;

		ExtraModuleNames.AddRange( new string[] { "StudyGame" } );
	}
}
