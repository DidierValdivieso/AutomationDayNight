#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "SmallDinosaurAnimInstance.generated.h"

UCLASS()
class THE_YUVEA_PROJECT_API USmallDinosaurAnimInstance : public UAnimInstance
{
	GENERATED_BODY()

public:
    USmallDinosaurAnimInstance();

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Animation")
    float Speed;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Animation")
    FVector2D InputData;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Animation")
    bool bIsInAir;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Animation")
    bool bIsJumpingStart;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Animation")
    bool bIsJumpingEnd;

    void NotifyJumpPressed();

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Animation")
    bool bIsInWater;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Animation")
    bool bIsSwimming;

protected:
    virtual void NativeUpdateAnimation(float DeltaSeconds) override;

private:
    float JumpStartTimer;
    bool bJumpStartTimerActive;
};
