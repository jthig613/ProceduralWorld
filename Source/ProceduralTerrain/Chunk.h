// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Materials/MaterialInterface.h"
#include "BiomeType.h"
#include "Chunk.generated.h"

UENUM(BlueprintType)
enum class ELODLevel : uint8
{
	LOD_High UMETA(DisplayName = "High"),
	LOD_Medium UMETA(DisplayName = "Medium"),
	LOD_Low UMETA(DisplayName = "Low")
};


UCLASS()
class PROCEDURALTERRAIN_API AChunk : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AChunk();

	// Initialize the chunk with its position and size
	void InitializeChunk(FVector InChunkPosition, int32 InChunkSize, int32 InBlockSize);

	// Generate the blocks in this chunk
	void GenerateChunk(UStaticMesh* BlockMesh, int32 Seed, float NoiseScale, float NoiseStrength, float TempScale, float MoistureScale, EBiomeType NewBiome, ELODLevel LODLevel);

	// Set up LOD for the chunk
	void UpdateLOD(float PlayerDistance);

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Biomes")
	UMaterialInterface* DesertMaterial;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Biomes")
	UMaterialInterface* ForestMaterial;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Biomes")
	UMaterialInterface* TundraMaterial;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Biomes")
	UMaterialInterface* PlainsMaterial;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Biomes")
	UMaterialInterface* DefaultMaterial;

private:
	UPROPERTY()
	class UInstancedStaticMeshComponent* BlockMeshComponent;

	FVector ChunkPosition;
	int32 ChunkSize;
	int32 BlockSize;

	UMaterialInterface* GetBiomeMaterial(EBiomeType Biome);

	void AddBlock(int32 X, int32 Y, int32 Z, UStaticMesh* BlockMesh, UMaterialInterface* BlockMaterial, ELODLevel LODLevel);
};
