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

	GetCapsuleComponent()->SetSimulatePhysics(true); //otherwise only simulates once controlled

	//moving the mesh to the bottom is harder than just reducing the collider size. 
	//The pivot of the mesh should be at the bottom for this, otherwise it will clip through the floor if gravity is enabled
	GetCapsuleComponent()->SetCapsuleRadius(5.f); 
	GetCapsuleComponent()->SetCapsuleHalfHeight(5.f); 

	m_SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("Spring Arm"));
	m_SpringArm->bInheritPitch = true;
	m_SpringArm->bInheritYaw = true;
	m_SpringArm->bInheritRoll = true;
	m_SpringArm->AddRelativeLocation(FVector(0, 0, 100)); //Move the camera away from the feet
	m_SpringArm->SetupAttachment(GetMesh());

	m_SceneCapture = CreateDefaultSubobject<USceneCaptureComponent2D>(TEXT("Scene Capture"));
	m_SceneCapture->SetupAttachment(m_SpringArm);

	m_StaticMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Static Mesh"));
	m_StaticMesh->SetupAttachment(GetCapsuleComponent());

	GetMesh()->SetCollisionEnabled(ECollisionEnabled::NoCollision); //only use collison of the tiny collision collider
	m_StaticMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
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

	m_PlayerController = GetWorld()->GetFirstPlayerController();

	PlayerInputComponent->BindAxis("Turn", this, &AStatisticsViewportCharacter::RotateSceneX);
	PlayerInputComponent->BindAxis("LookUp", this, &AStatisticsViewportCharacter::RotateSceneY);
	PlayerInputComponent->BindAxis("MouseScroll", this, &AStatisticsViewportCharacter::ZoomViewport);
	PlayerInputComponent->BindAction("MouseClick", IE_Released, this, &AStatisticsViewportCharacter::RemoveMouseInput);
	PlayerInputComponent->BindAction("MouseClick", IE_Pressed, this, &AStatisticsViewportCharacter::AddMouseInput);
}

void AStatisticsViewportCharacter::InitializeCharacter(UTextureRenderTarget2D* scene_capture)
{
	m_SceneCapture->TextureTarget = scene_capture;
}

void AStatisticsViewportCharacter::DisplayMesh(UStreamableRenderAsset* mesh_to_show)
{
	UStaticMesh* cast_to_static = Cast<UStaticMesh>(mesh_to_show);
	if (cast_to_static)
	{
		m_StaticMesh->SetVisibility(true);
		m_StaticMesh->SetStaticMesh(cast_to_static);
		GetMesh()->SetVisibility(false);
		return;
	}

	USkeletalMesh* cast_to_skeletal = Cast<USkeletalMesh>(mesh_to_show);
	if (cast_to_skeletal)
	{
		GetMesh()->SetVisibility(true);
		GetMesh()->SetSkeletalMesh(cast_to_skeletal);
		m_StaticMesh->SetVisibility(false);
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
		m_SpringArm->AddLocalRotation(FRotator(0, rotate_amount, 0));
	}
}

void AStatisticsViewportCharacter::RotateSceneY(float value)
{
	if (brotate_is_active)
	{
		const float rotate_amount = value * rotate_speed * GetWorld()->DeltaTimeSeconds;
		m_SpringArm->AddLocalRotation(FRotator(rotate_amount, 0, 0));

		ResetRoll();

		//save where the mouse is so it doesn't move after releasing
		m_PlayerController->bShowMouseCursor = false;
		if (m_MouseX && m_MouseY)
		{
			m_PlayerController->SetMouseLocation(m_MouseX, m_MouseY);
		}
	}
}

void AStatisticsViewportCharacter::ResetRoll()
{
	FRotator current_rotation = m_SpringArm->GetRelativeRotation();
	current_rotation.Roll = 0;
	m_SpringArm->SetRelativeRotation(current_rotation);
}

void AStatisticsViewportCharacter::AddMouseInput()
{
	if (brotate_is_active)
	{
		m_PlayerController->bShowMouseCursor = false;
		m_PlayerController->GetMousePosition(m_MouseX, m_MouseY);
	}
}

void AStatisticsViewportCharacter::RemoveMouseInput()
{
	if (brotate_is_active)
	{
		m_PlayerController->bShowMouseCursor = true;
	}
	brotate_is_active = false;
}

void AStatisticsViewportCharacter::ZoomViewport(float value)
{
	m_SpringArm->TargetArmLength += value * zoom_speed;
	if (m_SpringArm->TargetArmLength <= 100)
	{
		m_SpringArm->TargetArmLength = 100;
	}
	else if (m_SpringArm->TargetArmLength >= 500)
	{
		m_SpringArm->TargetArmLength = 500;
	}
}


