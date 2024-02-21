#pragma once

#include <CoreMinimal.h>

USTRUCT(BlueprintType)
struct FMovementParameters
{
	GENERATED_BODY()

	// Walking
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Walking")
	float WalkSpeed;

	// Running
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Running")
	float RunSpeed;

	// Jumping
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Jumping")
	float JumpHeight;
	float DoubleJumpMultiplier;

	// Wall Sliding
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Wall Sliding")
	float WallSlideSpeed;
};