// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "StatisticsViewportCharacter.generated.h"

class USpringArmComponent;
class USceneCaptureComponent2D;
class UStaticMeshComponent;

/**
* Character that is shown in the mini viewport. To acomplish this, it has a spring arm with a scene capture component.
* Input is added for rotating the camera
*/
UCLASS()
class FACTIONSTATISTICMENU_API AStatisticsViewportCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AStatisticsViewportCharacter();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
		USpringArmComponent* spring_arm;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
		USceneCaptureComponent2D* scene_capture;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
		UStaticMeshComponent* static_mesh;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
		float rotate_speed = 100.f;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
		float zoom_speed = 10.f;

	/**
	* Displays either a skeleton mesh or static mesh and hides the other type if already present
	*/
	void DisplayMesh(UStreamableRenderAsset* mesh_to_show);

	/**
	* Rotates around the character by a certain amount multiplied by rotate_speed
	* @param value - amount to rotate by, by default input from mouse axis X
	*/
	void RotateSceneX(float value);
	/**
	* Rotates up or down by a certain amount multiplied by rotate_speed
	* @param value - amount to rotate by, by default input from mouse axis Y
	*/
	void RotateSceneY(float value);
	/**
	* Changes the length of the spring arm, multiplied by zoom_speed
	* @param value - amount to zoom by, by default input from scroll wheel
	*/
	void ZoomViewport(float value);

	/**
	* rotation around the character is only active if this bool is set to true
	*/
	bool brotate_is_active = true;

private:
	void ResetRoll();
	void AddMouseInput();
	void RemoveMouseInput();

	float m_MouseX;
	float m_MouseY;

	APlayerController* player_controller;
};
