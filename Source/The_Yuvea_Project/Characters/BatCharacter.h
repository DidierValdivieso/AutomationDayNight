#pragma once

#include "CoreMinimal.h"
#include "MainCharacter.h"
#include "Camera/CameraComponent.h"
#include <GameFramework/SpringArmComponent.h>
#include "BatCharacter.generated.h"

UCLASS()
class THE_YUVEA_PROJECT_API ABatCharacter : public AMainCharacter
{
	GENERATED_BODY()

public:
	ABatCharacter();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera)
	UCameraComponent* CameraComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera)
	USpringArmComponent* SpringArm;

	UPROPERTY(VisibleAnywhere, Category = "Flight")
	float MinFlyHeight = -30000.0f;

	void UpdateMinFlyHeight();

protected:
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	virtual void Tick(float DeltaTime) override;
	virtual void HandleMovement(float DeltaTime) override;
	virtual void BeginPlay() override;

	float VerticalFlightInput = 0.0f;

	UPROPERTY(EditAnywhere, Category = "Flight")
	float VerticalFlightSpeed = 400.0f;

	UPROPERTY(EditAnywhere, Category = "Flight")
	float MaxFlyHeight = 2500.0f;

	UPROPERTY(VisibleAnywhere, Category = "Flight")
	bool bIsFlying = false;

	void StartFlyUp();
	void StartFlyDown();
	void StopFlyVertical();
};