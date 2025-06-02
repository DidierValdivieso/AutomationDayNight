#include "UltraDynamicSkyActorBase.h"
#include "Engine/DirectionalLight.h"
#include "GameFramework/Actor.h"
#include "Components/LightComponent.h"
#include "Components/SkyLightComponent.h"
#include "Components/DirectionalLightComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include <Components/SkyAtmosphereComponent.h>
#include <Components/ExponentialHeightFogComponent.h>
#include "Components/PostProcessComponent.h"
#include "Components/BillboardComponent.h"
#include <Components/VolumetricCloudComponent.h>

AUltraDynamicSkyActorBase::AUltraDynamicSkyActorBase()
{
    PrimaryActorTick.bCanEverTick = true;

    SkyAtmosphereComponent = CreateDefaultSubobject<USkyAtmosphereComponent>(TEXT("SkyAtmosphere"));
    SkyAtmosphereComponent->SetupAttachment(RootComponent);

    SkyLightComponent = CreateDefaultSubobject<USkyLightComponent>(TEXT("SkyLight"));
    SkyLightComponent->SetupAttachment(RootComponent);
    SkyLightComponent->Mobility = EComponentMobility::Movable;
    SkyLightComponent->SetRealTimeCapture(true);

    ExponentialFogComponent = CreateDefaultSubobject<UExponentialHeightFogComponent>(TEXT("ExponentialFog"));
    ExponentialFogComponent->SetupAttachment(RootComponent);
    ExponentialFogComponent->SetFogInscatteringColor(FLinearColor(0.5f, 0.5f, 0.6f));
    ExponentialFogComponent->SetFogDensity(0.01f);
    ExponentialFogComponent->SetVolumetricFog(true);

    VolumetricCloudComponent = CreateDefaultSubobject<UVolumetricCloudComponent>(TEXT("VolumetricCloud"));
    VolumetricCloudComponent->SetupAttachment(RootComponent);

    PostProcessComponent = CreateDefaultSubobject<UPostProcessComponent>(TEXT("PostProcess"));
    PostProcessComponent->SetupAttachment(RootComponent);

    PostProcessComponent->Settings.bOverride_AutoExposureMethod = true;
    PostProcessComponent->Settings.AutoExposureMethod = EAutoExposureMethod::AEM_Histogram;
    PostProcessComponent->Settings.bOverride_AutoExposureMinBrightness = true;
    PostProcessComponent->Settings.bOverride_AutoExposureMaxBrightness = true;
    PostProcessComponent->Settings.AutoExposureMinBrightness = 0.3f;
    PostProcessComponent->Settings.AutoExposureMaxBrightness = 2.0f;

    SkyMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("SkyMesh"));
    SkyMesh->SetupAttachment(RootComponent);
    SkyMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
    SkyMesh->SetCastShadow(false);
    SkyMesh->SetRelativeScale3D(FVector(1000.0f));

    SunIcon = CreateDefaultSubobject<UBillboardComponent>(TEXT("SunIcon"));
    SunIcon->SetupAttachment(RootComponent);
    SunIcon->bIsScreenSizeScaled = true;

    MoonIcon = CreateDefaultSubobject<UBillboardComponent>(TEXT("MoonIcon"));
    MoonIcon->SetupAttachment(RootComponent);
    MoonIcon->bIsScreenSizeScaled = true;
}

void AUltraDynamicSkyActorBase::BeginPlay()
{
    Super::BeginPlay();
    TimeOfDay = 6.0f;
    PostProcessComponent->bUnbound = true;
    TimeSpeed = 24.0f / DayDurationInSeconds;
}

void AUltraDynamicSkyActorBase::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    TimeOfDay += TimeSpeed * DeltaTime;
    if (TimeOfDay >= 24.0f)
        TimeOfDay -= 24.0f;

    if (bCycleActive)
    {
        UpdateLighting();
    }
}

void AUltraDynamicSkyActorBase::UpdateLighting()
{
    float TimeRatio = FMath::Fmod(TimeOfDay, 24.0f) / 24.0f;
    float SunPitch = FMath::Lerp(-90.0f, 90.0f, FMath::Sin(TimeRatio * PI));
    float SunYaw = TimeRatio * 360.0f - 90.0f;
    float DayFactor = FMath::Clamp(FMath::Sin(TimeRatio * PI), 0.0f, 1.0f);

    if (SunLightActor)
    {
        SunLightActor->SetActorRotation(FRotator(-SunPitch, -SunYaw, 0.0f));

        UDirectionalLightComponent* SunDirLight = Cast<UDirectionalLightComponent>(SunLightActor->GetLightComponent());
        if (SunDirLight)
        {
            SunDirLight->ForwardShadingPriority = 1;
        }

        ULightComponent* LightComp = SunLightActor->GetLightComponent();
        if (LightComp)
        {
            LightComp->SetIntensity(DayFactor * MaxSunIntensity);
            LightComp->SetLightColor(DayColor);
        }
    }

    if (MoonLightActor)
    {
        float MoonYaw = SunYaw + 180.0f;
        float MoonPitch = SunPitch + 180.0f;

        MoonLightActor->SetActorRotation(FRotator(MoonPitch, MoonYaw, 0.0f));

        UDirectionalLightComponent* MoonDirLight = Cast<UDirectionalLightComponent>(MoonLightActor->GetLightComponent());
        if (MoonDirLight)
        {
            MoonDirLight->ForwardShadingPriority = 0;
        }

        ULightComponent* MoonComp = MoonLightActor->GetLightComponent();
        if (MoonComp)
        {
            float NightFactor = 1.0f - DayFactor;
            MoonComp->SetIntensity(NightFactor * MaxMoonIntensity);
            MoonComp->SetLightColor(MoonColor);
        }
    }

    FVector SunDirection = FRotationMatrix(FRotator(0.0f, SunYaw, 0.0f)).GetUnitAxis(EAxis::X);
    FVector MoonDirection = -SunDirection;

    if (SunIcon)
        SunIcon->SetWorldLocation(GetActorLocation() + SunDirection * 1000.0f);

    if (MoonIcon)
        MoonIcon->SetWorldLocation(GetActorLocation() + MoonDirection * 1000.0f);

    if (SkyLightComponent)
    {
        SkyLightComponent->RecaptureSky();
        SkyLightComponent->SetIntensity(FMath::Lerp(0.1f, 1.0f, DayFactor));
    }

    if (ExponentialFogComponent)
    {
        FLinearColor NightFogColor(0.03f, 0.04f, 0.08f);
        FLinearColor DayFogColor(0.5f, 0.5f, 0.6f);
        FLinearColor CurrentFogColor = FMath::Lerp(NightFogColor, DayFogColor, DayFactor);
        ExponentialFogComponent->SetFogInscatteringColor(CurrentFogColor);
    }

    if (PostProcessComponent)
    {
        float MinExp = FMath::Lerp(0.3f, 1.0f, DayFactor);
        float MaxExp = FMath::Lerp(0.6f, 2.0f, DayFactor);
        PostProcessComponent->Settings.AutoExposureMinBrightness = MinExp;
        PostProcessComponent->Settings.AutoExposureMaxBrightness = MaxExp;
    }
}

float AUltraDynamicSkyActorBase::GetTimeOfDay() const
{
    return TimeOfDay;
}
