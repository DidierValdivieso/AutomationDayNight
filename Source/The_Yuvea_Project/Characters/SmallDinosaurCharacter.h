#pragma once

#include "CoreMinimal.h"
#include "MainCharacter.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "SmallDinosaurCharacter.generated.h"

UCLASS()
class THE_YUVEA_PROJECT_API ASmallDinosaurCharacter : public AMainCharacter
{
	GENERATED_BODY()
	
public:
	ASmallDinosaurCharacter();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera)
	UCameraComponent* CameraComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera)
	USpringArmComponent* SpringArm;

	UFUNCTION()
	void HandleJump();

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Animation")
	bool bIsSwimming = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Swimming")
	float SwimUpSpeed = 300.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Swimming")
	float SwimDownSpeed = 200.f;

	bool bSwimUpInput = false;
	bool bSwimDownInput = false;

	void SwimUp_Pressed();
	void SwimUp_Released();
	void SwimDown_Pressed();
	void SwimDown_Released();

	FVector TargetClimbPoint;
	bool bHasClimbTarget = false;

	UPROPERTY(BlueprintReadOnly)
	bool bIsClimbing = false;

	UPROPERTY()
	AActor* ClimbTargetActor = nullptr;

	FVector ClimbAttachLocation;

	FVector LastClimbLaunchDir;

	bool bWantsToAttach = false;
	FVector DesiredAttachPoint;

	FVector ClimbSurfaceNormal;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Swimming")
	float SwimForwardSpeed = 600.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Swimming")
	float SwimBackwardSpeed = 600.f;

	bool bSwimForwardInput = false;
	bool bSwimBackwardInput = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Swimming")
	float SwimStrafeSpeed = 600.f;

	bool bSwimRightInput = false;
	bool bSwimLeftInput = false;

protected:
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	virtual void Tick(float DeltaTime) override;
	void StartSwimRight();
	void StopSwimRight();
	void StartSwimLeft();
	void StopSwimLeft();
	void HandleSwimmingHorizontalMovement(float DeltaTime);
	void HandleSwimmingStrafeMovement(float DeltaTime);
	void HandleSwimmingForwardMovement(float DeltaTime);
	void StartSwimForward();
	void StopSwimForward();
	void StartSwimBackward();
	void StopSwimBackward();
	virtual void SetIsInWater(bool bNewState) override;
	void HandleSwimmingVerticalMovement(float DeltaTime);
	virtual void BeginPlay() override;

	void HandleClimbDebug();
	void HandleClimbingLock();
	void HandleClimbAttach();
	void DetachFromClimb();
	void TryClimbAttach();
	void JumpToClimbPoint();
	void TraceForClimbTarget();
	float WaterSurfaceZ = 300.f;

private:
	FVector MovementInput;
	void Jump();
	FTimerHandle TimerHandle_ClimbCheck;
};
