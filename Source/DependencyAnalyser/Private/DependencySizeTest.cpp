﻿// Copyright 2024 YAGER Development GmbH All Rights Reserved.

#include "DependencyAnalyserWidget.h"
#include "DependencyFunctionLibrary.h"
#include "PackageTools.h"
#include "Engine/ObjectLibrary.h"
#include "Misc/AutomationTest.h"

IMPLEMENT_COMPLEX_AUTOMATION_TEST(FDependencySizeTest, "DependencyAnalyser.DependencySizeTest",
                                 EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

void FDependencySizeTest::GetTests(TArray<FString>& OutBeautifiedNames, TArray<FString>& OutTestCommands) const
{
	UDependencyFunctionLibrary::CacheConfig();
	
	const FAssetRegistryModule& AssetRegistryModule = FModuleManager::LoadModuleChecked<FAssetRegistryModule>("AssetRegistry");
	TArray<FAssetData> Results = UDependencyFunctionLibrary::RunAssetAudit(AssetRegistryModule);
	for (int i = 0; i < Results.Num(); i++)
	{
		const FString& Filename = Results[i].PackageName.ToString();
		OutBeautifiedNames.Add(FPaths::GetBaseFilename(Filename));
		OutTestCommands.Add(Filename);
	}
}

bool FDependencySizeTest::RunTest(const FString& Parameters)
{
	const FAssetRegistryModule& AssetRegistryModule = FModuleManager::LoadModuleChecked<FAssetRegistryModule>("AssetRegistry");

	const DependenciesData Dependencies = UDependencyFunctionLibrary::GetDependencies(
		AssetRegistryModule,
		FName(Parameters),
		true,
		true);

	TArray<FAssetData> AssetData;
	AssetRegistryModule.Get().GetAssetsByPackageName(*Parameters, AssetData, true);

	if (!AssetData.IsEmpty())
	{
		int32 ErrorSize;
		if (UDependencyFunctionLibrary::IsErrorSize(AssetData[0].GetClass(), Dependencies.TotalSize, ErrorSize))
		{
			const FString& ErrorMsg = FString::Printf(TEXT("Asset %s of size %llu MB is larger than max %d MB size"), *Parameters, Dependencies.TotalSize / 1000000, ErrorSize); 
			AddError(ErrorMsg);
			return false;
		}

		int32 WarningSize;
		if (UDependencyFunctionLibrary::IsWarningSize(AssetData[0].GetClass(), Dependencies.TotalSize, WarningSize))
		{
			const FString& WarningMsg = FString::Printf(TEXT("Asset %s of size %llu MB is larger than recommended %d MB size"), *Parameters, Dependencies.TotalSize / 1000000, WarningSize); 

			if (UDependencyFunctionLibrary::CachedFailForWarnings)
			{
				AddError(WarningMsg);
				return false;
			}
		
			AddWarning(WarningMsg);
			return true;
		}
	}
	
	return true;
}
