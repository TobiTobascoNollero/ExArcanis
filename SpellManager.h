// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

/**
 * 
 */
class EXARCANIS_API SpellManager
{
public:
	UFUNCTION(BlueprintImplementableEvent, Category = "ExArcanis")
	void CastFailed();

	SpellManager();
	~SpellManager();
};
