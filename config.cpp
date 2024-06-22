class CfgPatches
{
	class AirdropUpgraded
	{
		units[]={};
		weapons[]={};
		requiredVersion=0.1;
		requiredAddons[]=
		{
			"DZ_Data"
		};
	};
};
class CfgMods
{
	class AirdropUpgraded
	{
		dir="Airdrop-Upgraded";
		picture="";
		action="";
		hideName=1;
		hidePicture=1;
		name="Airdrop-Upgraded";
		credits="Camino, Philipp, Zedmag";
		author="DiGiaCom Tech";
		authorID="28458972";
		version="1.21.05.10";
		extra=0;
		type="mod";
		dependencies[]=
		{
			"Mission",
			"World",
			"Game"
		};
		class defs
		{
			class gameScriptModule
			{
				value="";
				files[]=
				{
					"Airdrop-Upgraded/Scripts/3_Game"
				};
			};
			class worldScriptModule
			{
				value="";
				files[]=
				{
					"Airdrop-Upgraded/Scripts/4_World"
				};
			};
			class missionScriptModule
			{
				value="";
				files[]=
				{
					"Airdrop-Upgraded/Scripts/5_Mission"
				};
			};
		};
	};
};
class CfgSoundShaders
{
	class baseEngineC130J_SoundShader
	{
		range=1000;
	};
	class c130j_loop_int_SoundShader: baseEngineC130J_SoundShader
	{
		samples[]=
		{
			
			{
				"\Airdrop-Upgraded\Sounds\airplane",
				1
			}
		};
		volume=1;
	};
};
class CfgSoundSets
{
	class baseHelicopterEngine_EXT_SoundSet
	{
		sound3DProcessingType="Vehicle_Ext_3DProcessingType";
		distanceFilter="softVehiclesDistanceFreqAttenuationFilter";
		volumeCurve="vehicleEngineAttenuationCurve";
		volumeFactor=1;
		spatial=1;
		loop=0;
	};
	class c130j_loop_int_SoundSet: baseHelicopterEngine_EXT_SoundSet
	{
		soundShaders[]=
		{
			"c130j_loop_int_SoundShader"
		};
		volumeFactor=1;
	};
};
class CfgSounds
{
	class default
	{
		name="";
		titles[]={};
	};
	class c130j_loop_int: default
	{
		sound[]=
		{
			"\Airdrop-Upgraded\Sounds\airplane",
			1,
			1,
			1000
		};
	};
};
