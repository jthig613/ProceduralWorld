// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Chunk.generated.h"

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
	void GenerateChunk(UStaticMesh* BlockMesh, int32 Seed, float NoiseScale, float NoiseStrength);

	// Set up LOD for the chunk
	void UpdateLOD(float PlayerDistance);

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;


private:
	UPROPERTY()
	class UInstancedStaticMeshComponent* BlockMeshComponent;

	FVector ChunkPosition;
	int32 ChunkSize;
	int32 BlockSize;

	void AddBlock(int32 X, int32 Y, int32 Z, UStaticMesh* BlockMesh);
};
