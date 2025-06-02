#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include <Engine/DirectionalLight.h>
#include <Components/ExponentialHeightFogComponent.h>
#include <Components/VolumetricCloudComponent.h>
#include "UltraDynamicSkyActorBase.generated.h"

UCLASS()
class THE_YUVEA_PROJECT_API AUltraDynamicSkyActorBase : public AActor
{
	GENERATED_BODY()
	
public:
    AUltraDynamicSkyActorBase();

protected:
    virtual void BeginPlay() override;

public:
    virtual void Tick(float DeltaTime) override;

    void UpdateLighting();

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Time")
    float TimeOfDay = 6.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Time")
    bool bCycleActive = true;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Time")
    float DayDurationInSeconds = 24.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Time")
    ADirectionalLight* MoonLightActor;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Time")
    ADirectionalLight* SunLightActor;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Time")
    USkyAtmosphereComponent* SkyAtmosphereComponent;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Time")
    USkyLightComponent* SkyLightComponent;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Sky")
    UStaticMeshComponent* SkyMesh;

    UPROPERTY(VisibleAnywhere, Category = "Sky")
    UBillboardComponent* SunIcon;

    UPROPERTY(VisibleAnywhere, Category = "Sky")
    UBillboardComponent* MoonIcon;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Lighting")
    float MaxMoonIntensity = 0.5f;

    UPROPERTY(EditAnywhere)
    class UPostProcessComponent* PostProcessComponent;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Time")
    UExponentialHeightFogComponent* ExponentialFogComponent;
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Time")
    UVolumetricCloudComponent* VolumetricCloudComponent;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Time")
    float MaxSunIntensity = 10.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Time")
    FLinearColor DayColor = FLinearColor(1.0f, 0.95f, 0.8f);

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Time")
    FLinearColor MoonColor = FLinearColor(0.35f, 0.4f, 0.55f);

    float TimeSpeed;

    UFUNCTION(BlueprintCallable)
    float GetTimeOfDay() const;
};