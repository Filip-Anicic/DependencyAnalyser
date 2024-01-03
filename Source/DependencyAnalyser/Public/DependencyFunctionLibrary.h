// Copyright 2024 YAGER Development GmbH All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "AssetRegistry/AssetRegistryModule.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "DependencyFunctionLibrary.generated.h"

struct DEPENDENCYANALYSER_API FDependenciesData
{
	int32 Amount;
	SIZE_T TotalSize;
};

/* Public function library to be used by editor tool and test suite */
UCLASS()
class DEPENDENCYANALYSER_API UDependencyFunctionLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	
	// Retrieves all assets to analyse
	static TArray<FAssetData> RunAssetAudit(const FAssetRegistryModule& AssetRegistryModule);

	// Collects and returns dependencies from an internal recursive call
	static FDependenciesData GetDependencies(const FAssetRegistryModule& AssetRegistryModule, const FName PackageName,
		const bool IncludeSoftReferences, const bool IgnoreDevFolders);

	// Inform about whether an asset is over a specified size
	static bool IsWarningSize(const UClass* Class, const SIZE_T Size, int32& OutWarningSize);
	static bool IsErrorSize(const UClass* Class, const SIZE_T Size, int32& OutErrorSize);
	static bool IsOverMBSize(const SIZE_T Size, const int32 SizeMB);

	// Retrieval and caching of config data
	static void CacheConfig();
	inline static int32 CachedDefaultWarningSize = 50;
	inline static int32 CachedDefaultErrorSize = 500;
	inline static bool bCachedFailForWarnings;

private:
	
	// Collects dependencies recursively
	static void GetDependenciesRecursive(const FAssetRegistryModule& AssetRegistryModule, const FName PackageName,
		const UE::AssetRegistry::EDependencyQuery QueryType, const bool IgnoreDevFolders, TArray<FName>& OutDependencies);

	// Retrieved and cached config data
	inline static TMap<UClass*, int32> CachedWarningSizePerType;
	inline static TMap<UClass*, int32> CachedErrorSizePerType;
};
