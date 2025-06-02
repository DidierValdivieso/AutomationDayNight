#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "BatAnimInstance.generated.h"

UCLASS()
class THE_YUVEA_PROJECT_API UBatAnimInstance : public UAnimInstance
{
	GENERATED_BODY()

public:
    UBatAnimInstance();

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Animation")
    float Speed;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Animation")
    FVector2D InputData;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Animation")
    bool bIsInAir;

protected:
    virtual void NativeUpdateAnimation(float DeltaSeconds) override;

};