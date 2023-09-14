// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "InputActionValue.h"
#include "Core/KinetixCoreSubsystem.h"
#include "UnrealKinetixSampleCharacter.generated.h"

UCLASS(config=Game)
class AUnrealKinetixSampleCharacter : public APawn
{
	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class UCapsuleComponent* CapsuleComponent;
	
	/** Camera boom positioning the camera behind the character */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class USpringArmComponent* CameraBoom;

	/** Follow camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* FollowCamera;
	
	/** MappingContext */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputMappingContext* DefaultMappingContext;

	/** Look Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* LookAction;

	/** Skeletal Mesh Source that will be used to apply Kinetix's Emotes */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Kinetix, meta = (AllowPrivateAccess = "true"))
	class USkeletalMeshComponent* SkeletalMeshSource;

	/** Skeletal Mesh Target - Basically yours */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Kinetix, meta = (AllowPrivateAccess = "true"))
	class USkeletalMeshComponent* SkeletalMeshTarget;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Kinetix, meta = (AllowPrivateAccess = "true"))
	class UKinetixCharacterComponent* KinetixCharacterComponent;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Kinetix, meta = (AllowPrivateAccess = "true"))
	class UAnimSequenceSamplerComponent* AnimSequenceSamplerComponent;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Kinetix, meta = (AllowPrivateAccess = "true"))
	FString Username;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Kinetix, meta = (AllowPrivateAccess = "true"))
	FString AnimationIDToRegister;
	
	UPROPERTY()
	UKinetixCoreSubsystem* KinetixCore;

	UPROPERTY()
	FAnimationID AnimationIDToPlay;
	
public:
	AUnrealKinetixSampleCharacter();

protected:

	/** Called for looking input */
	void Look(const FInputActionValue& Value);

protected:
	// APawn interface
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	// To add mapping context
	virtual void BeginPlay();

	UFUNCTION()
	void OnCoreInitialized();

	UFUNCTION()
	void OnAccountConnected();

	UFUNCTION()
	void OnEmoteAssociated(const FString& Response);

	UFUNCTION()
	void OnMetadatasAvailable(bool bSuccess, const TArray<FAnimationMetadata>& Metadatas);

	UFUNCTION()
	void OnAnimationLoaded(bool bSuccess);

public:
	/** Returns CameraBoom subobject **/
	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }
	/** Returns FollowCamera subobject **/
	FORCEINLINE class UCameraComponent* GetFollowCamera() const { return FollowCamera; }
};

