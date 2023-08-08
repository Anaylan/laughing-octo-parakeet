// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;
using System.Collections.Generic;

public class StudyGameClientTarget : TargetRules //Change this line according to the name of your project
{
    public StudyGameClientTarget(TargetInfo Target) : base(Target) //Change this line according to the name of your project
    {
        Type = TargetType.Client;
        IncludeOrderVersion = EngineIncludeOrderVersion.Latest;
        DefaultBuildSettings = BuildSettingsVersion.V2;
        CppStandard = CppStandardVersion.Cpp17;
        
        ExtraModuleNames.AddRange( new string[] { "StudyGame" } ); //Change this line according to the name of your project
    }
}