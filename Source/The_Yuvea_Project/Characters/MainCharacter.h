#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include <Components/BoxComponent.h>
#include "MainCharacter.generated.h"

UCLASS()
class THE_YUVEA_PROJECT_API AMainCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	AMainCharacter();

	UPROPERTY(BlueprintReadOnly, Category = "Movement")
	FVector2D InputData;

	UFUNCTION()
	void StartRunning();

	UFUNCTION()
	void StopRunning();

	UPROPERTY(BlueprintReadOnly)
	bool bIsInWater = false;

	UPROPERTY(BlueprintReadOnly)
	bool bCanDrink = false;

	virtual void SetIsInWater(bool bNewState);
	bool IsInWater() const;
	void TryDrink();
	float GetEnergyRatio() const;
	void DecreaseEnergy(float Amount);
	void SetIsNearFoodZone(bool bNewState);

	UFUNCTION(BlueprintCallable)
	float GetHydrationRatio() const;

	void DecreaseHydration(float Amount);
	void IncreaseHydration(float Amount);

	// Initial hydration
	UPROPERTY(BlueprintReadOnly, Category = "Status")
	float Hydration = 50.f;

	// Initial energy
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Status")
	float Energy = 50.f;

	bool bIsNearFoodZone = false;
	bool bHasShownEatPrompt = false;
	
	FVector MovementInput;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Water")
	UBoxComponent* WaterOverlapBox;

protected:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;
	virtual void HandleMovement(float DeltaTime);
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	UPROPERTY(EditAnywhere, Category = "Movement")
	float WalkSpeed = 600.0f;

	UPROPERTY(EditAnywhere, Category = "Movement")
	float RunSpeed = 1200.0f;

private:
	void MoveForward_Pressed();
	void MoveForward_Released();
	void MoveBackward_Pressed();
	void MoveBackward_Released();
	void MoveRight_Pressed();
	void MoveRight_Released();
	void MoveLeft_Pressed();
	void MoveLeft_Released();

	void OnMouseX(float Value);
	void OnMouseY(float Value);
	void UpdateInputDirection();

	void Eat();

};
