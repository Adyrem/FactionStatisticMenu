// Fill out your copyright notice in the Description page of Project Settings.

#include "GameFramework/SpringArmComponent.h"
#include "Components/SceneCaptureComponent2D.h"
#include "Components/CapsuleComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "StatisticsViewportCharacter.h"
#include "Components/StaticMeshComponent.h"

// Sets default values
AStatisticsViewportCharacter::AStatisticsViewportCharacter()
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	GetCapsuleComponent()->SetSimulatePhysics(true); //otherwise only has gravity once controlled

	spring_arm = CreateDefaultSubobject<USpringArmComponent>(TEXT("Spring Arm"));
	spring_arm->bInheritPitch = true;
	spring_arm->bInheritYaw = true;
	spring_arm->AddRelativeLocation(FVector(0, 0, 100)); //Move the camera away from the feet
	spring_arm->SetupAttachment(GetMesh());

	scene_capture = CreateDefaultSubobject<USceneCaptureComponent2D>(TEXT("Scene Capture"));
	scene_capture->SetupAttachment(spring_arm);

	static_mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Static Mesh"));
	static_mesh->SetupAttachment(GetCapsuleComponent());

	GetMesh()->AddRelativeLocation(FVector(0, 0, -88)); //by default move it to the bottom of the collider
	static_mesh->AddRelativeLocation(FVector(0, 0, -88));
}

// Called when the game starts or when spawned
void AStatisticsViewportCharacter::BeginPlay()
{
	Super::BeginPlay();

}

// Called every frame
void AStatisticsViewportCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void AStatisticsViewportCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	player_controller = GetWorld()->GetFirstPlayerController();

	PlayerInputComponent->BindAxis("Turn", this, &AStatisticsViewportCharacter::RotateSceneX);
	PlayerInputComponent->BindAxis("LookUp", this, &AStatisticsViewportCharacter::RotateSceneY);
	PlayerInputComponent->BindAxis("MouseScroll", this, &AStatisticsViewportCharacter::ZoomViewport);
	PlayerInputComponent->BindAction("MouseClick", IE_Released, this, &AStatisticsViewportCharacter::RemoveMouseInput);
	PlayerInputComponent->BindAction("MouseClick", IE_Pressed, this, &AStatisticsViewportCharacter::AddMouseInput);
}

void AStatisticsViewportCharacter::DisplayMesh(UStreamableRenderAsset* mesh_to_show)
{
	UStaticMesh* cast_to_static = Cast<UStaticMesh>(mesh_to_show);
	if (cast_to_static)
	{
		static_mesh->SetVisibility(true);
		static_mesh->SetStaticMesh(cast_to_static);
		GetMesh()->SetVisibility(false);
		return;
	}

	USkeletalMesh* cast_to_skeletal = Cast<USkeletalMesh>(mesh_to_show);
	if (cast_to_skeletal)
	{
		GetMesh()->SetVisibility(true);
		GetMesh()->SetSkeletalMesh(cast_to_skeletal);
		static_mesh->SetVisibility(false);
		return;
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Unrecognised Mesh attached to viewport_character"))
	}

}

void AStatisticsViewportCharacter::RotateSceneX(float value)
{
	if (brotate_is_active)
	{
		const float rotate_amount = value * rotate_speed * GetWorld()->DeltaTimeSeconds;
		spring_arm->AddLocalRotation(FRotator(0, rotate_amount, 0));
	}
}

void AStatisticsViewportCharacter::RotateSceneY(float value)
{
	if (brotate_is_active)
	{
		const float rotate_amount = value * rotate_speed * GetWorld()->DeltaTimeSeconds;
		spring_arm->AddLocalRotation(FRotator(rotate_amount, 0, 0));

		ResetRoll();

		//save where the mouse is so it doesn't move after releasing
		player_controller->bShowMouseCursor = false;
		if (m_MouseX && m_MouseY)
		{
			player_controller->SetMouseLocation(m_MouseX, m_MouseY);
		}
	}
}

void AStatisticsViewportCharacter::ResetRoll()
{
	FRotator current_rotation = spring_arm->GetRelativeRotation();
	current_rotation.Roll = 0;
	spring_arm->SetRelativeRotation(current_rotation);
}

void AStatisticsViewportCharacter::AddMouseInput()
{
	if (brotate_is_active)
	{
		player_controller->bShowMouseCursor = false;
		player_controller->GetMousePosition(m_MouseX, m_MouseY);
	}
}

void AStatisticsViewportCharacter::RemoveMouseInput()
{
	if (brotate_is_active)
	{
		player_controller->bShowMouseCursor = true;
	}
	brotate_is_active = false;
}

void AStatisticsViewportCharacter::ZoomViewport(float value)
{
	spring_arm->TargetArmLength += value * zoom_speed;
	if (spring_arm->TargetArmLength <= 100)
	{
		spring_arm->TargetArmLength = 100;
	}
	else if (spring_arm->TargetArmLength >= 500)
	{
		spring_arm->TargetArmLength = 500;
	}
}


