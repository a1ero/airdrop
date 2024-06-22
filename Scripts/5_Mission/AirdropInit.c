
modded class MissionServer
{
	void MissionServer()
	{
		if (GetGame().IsServer())
		{
			g_AirdropBase = null;
			GetAirdropBase();
		}
	}

	void ~MissionServer()
	{
		delete g_AirdropBase;
	}
}

modded class MissionGameplay
{
	void MissionGameplay()
	{
		if (GetGame().IsServer())
		{
			g_AirdropBase = null;
			GetAirdropBase();
		}
	}
}