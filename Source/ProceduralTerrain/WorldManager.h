// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Chunk.h"
#include "BiomeType.h"
#include "WorldManager.generated.h"

UCLASS()
class PROCEDURALTERRAIN_API AWorldManager : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AWorldManager();

	UPROPERTY(EditAnywhere, Category = "World Settings")
	int32 ChunkSize;

	FVector PlayerPosition;

	// Distances for LOD thresholds
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LOD")
	float LODHighDistance = 1000.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LOD")
	float LODMediumDistance = 3000.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LOD")
	float LODLowDistance = 6000.0f;

	UPROPERTY(EditAnywhere, Category = "World Settings")
	int32 RenderDistance;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "LOD")
	float LoadDistance = 5000.0f;   // Distance within which chunks should be loaded
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "LOD")
	float UnloadDistance = 6000.0f; // Distance beyond which chunks should be unloaded

	// Player tracking for chunk updates
	FVector LastChunkUpdatePosition; // Last player position when chunks were updated
	float LastUpdateTime;            // Last time the chunks were updated

	// Adjustable parameters
	float MinDistanceToUpdateChunks; // Minimum distance player needs to move to trigger chunk update
	float UpdateInterval;



	//the grid of the map
	UPROPERTY(EditAnywhere, Category = "World Settings")
	int32 GridSize;

	UPROPERTY(EditAnywhere, Category = "World Settings")
	UStaticMesh* BlockMesh;

	UPROPERTY(EditAnywhere, Category = "World Settings")
	int32 Seed;              // A fixed seed value or you could generate one

	//Temp seed 8622241

	UPROPERTY(EditAnywhere, Category = "World Settings")
	float NoiseScale;          // The scale of the noise
	UPROPERTY(EditAnywhere, Category = "World Settings")
	float NoiseStrength;

	UPROPERTY(EditAnywhere, Category = "World Settings")
	float CaveNoiseScale;          // The scale of the noise
	UPROPERTY(EditAnywhere, Category = "World Settings")
	float CaveNoiseStrength;
	UPROPERTY(EditAnywhere, Category = "World Settings")
	float CaveThreshold;

	UPROPERTY(EditAnywhere, Category = "World Settings")
	float TempScale;          // The scale of the noise
	UPROPERTY(EditAnywhere, Category = "World Settings")
	float MoistureScale;          // The scale of the noise


	//Need to adjust biome generation to create "regions" rather than each chunk loading a single biome
	// Adjust the number of chunks a biome should span
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Terrain")
	int32 BiomeSizeInChunks; // Number of chunks per biome

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Biome")
	int32 BiomeMapSize; // Number of biome cells on each axis


	/*Seed: Controls the randomness of the noise generation.Using the same seed will produce the same terrain.You can change this dynamically to get different terrain features.

	NoiseScale : Affects how stretched or compressed the noise pattern is.Smaller values(e.g., 0.01) will result in larger, smoother hills, while larger values(e.g., 0.1) will produce more detail and "noise" in the terrain.

	NoiseStrength : Multiplies the noise value to scale it to a usable terrain height.Larger values will produce taller mountains or deeper valleys.*/
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;


public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

private:
	TMap<FIntVector, AChunk*> LoadedChunks;

	void LoadChunk(FIntVector ChunkCoords);
	void UnloadChunk(FIntVector ChunkCoords);

	UPROPERTY(EditAnywhere, Category = "World Settings")
	TSubclassOf<class AChunk> ChunkClass;

	// Biome map for determining biome per region of chunks
	TArray<EBiomeType> BiomeMap;

	void GenerateBiomeMap(int32 Seed);
	EBiomeType GetBiomeForChunk(FIntVector ChunkCoords);

	ELODLevel CalculateLODLevel(FVector ChunkPosition);


	//Chunk loading
	void UpdateChunks(FVector playerposition);

	FVector GetChunkCenter(FIntVector ChunkCoord);

	FIntVector GetChunkCoordinates(FVector Position);

	bool ShouldUpdateChunks(FVector playerposition);
};
