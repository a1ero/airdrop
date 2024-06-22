//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Handles aircraft movement and initiates drop ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//class AirdropPlane extends House	//BuildingSuper (original)
//use base to inherite any new planes from.. 
class AirDrop_Plane_Base extends House	//BuildingSuper
{
	// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
	const int m_SecsPerMin = 60;			// Time - Seconds to minutes conversion
	const float m_Knots2MPS = 0.514444;		// Convert KNOTs to Meters per Second //Original
	//const float m_Knots2MPS = 0.22212;		// Convert KNOTs to Meters per Second //(zedmag)
	const int m_MetersPerkilometer = 1000;	// Convert kilometers to meters
	// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
	ref Timer m_SpawnTimer;					// Timer controlling creation of random spawns
	ref AirdropSettings m_Settings;			// Local Airdrop settings (passed in when called)
	ref AD_DropZone m_DropZone;				// Dropzone information for this aircraft
	ref AD_DropType m_DropType;				// Drop type information for this aircraft	
	// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
	// Container types (5) >>> Need to read this from the this settings
	ref TStringArray m_Containers = { "AirdropContainer", "AirdropContainer_Blue", "AirdropContainer_Medical", "AirdropContainer_Military", "AirdropContainer_Yellow" };
	// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
	// Listing of available container types
	// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
	protected EffectSound m_Sound;			// Aircraft sound (needs to come drom aircraft definition)
	protected bool m_RandomDrop;			// Flag to indicate if this a randomly called drop (true = randol, false = player)
	protected bool m_IsActive;				// Flag to indicate if aircraft is active
	protected bool m_IsDropped;				// Flag to indicate if container has been dropped
	protected float m_Angle;				// Aircraft headging (angle from starting point to dropzone)
	protected bool m_InProximity;			// Flag to indicate if aircraft proximity messages were sent
	// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
	FileHandle m_LogFile;					// Reference to log file
	vector m_SpawnLocation;					//
	vector m_DropPoint;						//
	string m_LogMessage;					// Message to be logged (temp)
	string m_TxtMessage;					// Message to be logged (temp)
	int m_MessageColor;						// ARGB beckground color value for Notification messages
	int m_ZombieCount;						// Number of zombies spawned
	PlayerBase m_Player;					//
	string m_Cardinal;						// Carinal heading
	// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
	float m_currentAlt = 0.0;				// Current aircraft altitude (per update)
	float m_targetAlt = 0.0;				// Target aircraft altitude (per update)
	float m_lastDist = 999999;				// Distance to target from previous update
	float m_Dist = 999999;					// Distance to target (from start)
	//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	void AirDrop_Plane_Base() //renamed to match base class
	{
		// Start simulation of flight
		SetEventMask(EntityEvent.CONTACT | EntityEvent.SIMULATE);
		// If this is the client and NOT miltiplayer
		if (GetGame().IsClient() || !GetGame().IsMultiplayer()) {
			// Play the aircraft sound file
			PlayLoop();
		}
	}
	//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	void ~AirDrop_Plane_Base() //renamed to match base class
	{
		// Send closing message
		AD_LogMessage("[~AirdropPlane] Aircraft destroyed.");
		//  If we have a file
		if (m_LogFile) {
			// Close file
			CloseFile(m_LogFile);
		}
		// If we have a sound
		if (m_Sound) {
			// Stop playing sound
			m_Sound.Stop();
		}
	}
	//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	void PlayLoop()
	{
		if (GetGame())
		{
			if (GetGame().GetPlayer())
			{
				PlaySoundLoop("c130j_loop_int", 1000);
			}
		}
	}
	//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	void InitializeAircraft(ref AirdropSettings settings, ref AD_DropZone dropzone, bool randomdrop, PlayerBase player, string cradinal)
	{
		// If settings were passed in...
		if (settings && dropzone) {
			// Set Drop parameters
			m_Cardinal = cradinal;
			m_IsActive = true;
			m_Settings = settings;
			m_DropZone = dropzone;
			m_RandomDrop = randomdrop;
			m_Player = player;
			// Create log file for this aircraft
			string datetimestamp = AirdropBase.ADU_GetFileDateTimeStamp();
			if (m_Settings.Controls.AD_LogAircraft != 0) {
				m_LogFile = OpenFile(TEXT_PATH_LOGS_AIRDROP + datetimestamp + " AirdropAircraft.log", FileMode.WRITE);
			}
			// Log status
			AD_LogMessage("[AirdropPlane] Aircraft created on " + datetimestamp);
			// Set message color
			m_MessageColor = ARGB(m_Settings.Messages.NotificationARGB[0], m_Settings.Messages.NotificationARGB[1], m_Settings.Messages.NotificationARGB[2], m_Settings.Messages.NotificationARGB[3]);
			// Get current position and calculate heading to dropzone
			vector position = GetPosition();
			m_DropPoint = Vector(m_DropZone.X, 0, m_DropZone.Z);
			float m_Dist = AirdropBase.ADU_CalcDistance(m_DropPoint, position);
			m_Angle = Math.Atan2(m_DropZone.Z - position[2], m_DropZone.X - position[0]);
			// Calculate how often to spawn a zomie
			float m_zInterval = (m_Dist / m_DropZone.Zombies) / (m_Settings.Aircraft.AirSpeedKIAS * m_Knots2MPS);
			// Start spawning zombies
			m_SpawnTimer = new Timer();
			m_SpawnTimer.Run(m_zInterval, this, "EOnSpawnZombies", new Param2<bool, vector>(false, vector.Zero), true);
			// Log status
			AD_LogMessage("[InitializeAircraft] Aircraft Initialized: " + m_DropZone.Title + " @ " + m_DropZone.X + "/" + m_DropZone.Z + " (" + m_RandomDrop + ")");
		}
		else
		{
			// Log status
			AirdropBase.ADU_SendClientMessages(AIRDROP_ICON_AIRDROP_ACFT, "[InitializeAircraft] Aircraft NOT Initialized.  You are NOT allowed to spawn in AirdropPlanes!", 3, 30, ARGB(255, 128, 0, 0));
		}
	}
	//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	void AD_LogMessage(string message) {
		// If we have a file... 
		if (m_LogFile && m_Settings.Controls.AD_LogAircraft != 0) {
			// Write message
			FPrintln(m_LogFile, AirdropBase.ADU_GetFileDateTimeStamp(true) + ":  " + message);
		}
	}
	//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	void AD_DestroyPlane(vector position) {
		// Destroy the aircraft
		AD_LogMessage("[EOnBeforeDelete] Aircraft despawned @ " + position);
		GetGame().GetCallQueue(CALL_CATEGORY_SYSTEM).CallLater(GetGame().ObjectDelete, 0, false, this);
	}
	//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	override void EOnSimulate(IEntity owner, float dt)
	{
		// Get aircraft's current position
		vector m_PlanePosition = GetPosition();
		// If this is the server and we have recieved settings...
		if (GetGame().IsServer() && m_IsActive)
		{
			// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
			// Get current & target altitudes and calculate altitude smoothing 
			m_currentAlt = m_PlanePosition[1];
			m_targetAlt = GetGame().SurfaceY(m_PlanePosition[0], m_PlanePosition[2]) + m_Settings.Aircraft.DropAGL;
			// Constrain minimum altitude to drop altitude
			m_targetAlt = Math.Max(m_Settings.Aircraft.DropAGL, m_targetAlt);
			// Calculate new atitude 
			float altitudechange = Math.Lerp(m_currentAlt, m_targetAlt, m_Settings.Aircraft.TerrainFollowing) - m_currentAlt;
			m_PlanePosition[1] = m_PlanePosition[1] + (altitudechange * dt);
			// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
			// Calculate velocity vector
			vector velocity = Vector(Math.Cos(m_Angle), 0, Math.Sin(m_Angle));
			// Caclulate movement vector based on aircraft's KIAS
			velocity[0] = velocity[0] * m_Settings.Aircraft.AirSpeedKIAS * m_Knots2MPS * dt;
			velocity[2] = velocity[2] * m_Settings.Aircraft.AirSpeedKIAS * m_Knots2MPS * dt;
			// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
			// Setup transform
			vector transform[4];
			GetTransform(transform);
			transform[3] = transform[3] + velocity;
			// Set transform based on altitude
			transform[3][1] = m_PlanePosition[1];
			// Move aircraft
			MoveInTime(transform, dt);
			// Get aircraft's updated position
			m_PlanePosition = GetPosition();
			// If not yet dropped  
			if (!m_IsDropped) {
				// Check if time to drop
				EOnBeforeDrop(m_PlanePosition);
			}
			else {
				// Check if airfraft has moved off map 
				EOnBeforeDelete(m_PlanePosition);
			}
		}
	}
	//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	void EOnBeforeDrop(vector position)
	{
		// Calculate distance to release 
		vector droppoint = Vector(m_DropZone.X, 0, m_DropZone.Z);
		float m_Dist = AirdropBase.ADU_CalcDistance(droppoint, position);
		// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
		// If the aircraft is within the proximity warning range
		if (m_Dist <= m_Settings.Messages.Proximity && !m_InProximity)
		{
			// Toggle warning flag & send message
			m_InProximity = true;
			// If this is a random drop...
			if (m_RandomDrop) {
				// Log status
				m_LogMessage = "The Aircraft is " + m_Settings.Messages.Proximity / m_MetersPerkilometer + " Km from " + m_DropZone.Title + ", from the " + m_Cardinal + ".";
				AirdropBase.ADU_SendClientMessages(AIRDROP_ICON_AIRDROP_RANGE, m_LogMessage, m_Settings.Messages.Mode, m_Settings.Messages.Duration, m_MessageColor);
			}
			else {
				// Log status
				m_LogMessage = "The Aircraft is " + m_Settings.Messages.Proximity / m_MetersPerkilometer + " Km from called location, from the " + m_Cardinal + ".";
				AirdropBase.ADU_SendClientMessagesPrivate(AIRDROP_ICON_AIRDROP_RANGE, m_LogMessage, m_Settings.Messages.Mode, m_Settings.Messages.Duration, m_MessageColor, m_Player);
			}
			AD_LogMessage("[EOnBeforeDrop] " + m_LogMessage + " @ " + position);
		}
		// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------		
		// If aircrft is close enought or past the drop point
		if ((m_Dist <= Math.RandomFloat(0, m_DropZone.DropAccuracy)) || (m_Dist > m_lastDist)) {
			// If the container has not yet been dropped...
			if (!m_IsDropped)
			{
				// Drop the container & set the dropped flag
				EOnDrop(position);
				m_IsDropped = true;
			}
		}
		// Record current distance
		m_lastDist = m_Dist;
	}
	//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	void EOnDrop(vector position)
	{
		// >>> This (randomizations & spawning zombies) needs to be moved into the AirdropContainer.InitializeContainer
		// If DropZone is 'RANDOM'...
		if (m_DropZone.DropType == "RANDOM") {
			//AD_LogMessage("[EOnDrop] 2a.");
			// Select random DropType
			m_DropType = m_Settings.DropTypes.GetRandomElement();
		}
		else {
			// Loop through each DropType...
			for (int z0 = 0; z0 < m_Settings.DropTypes.Count(); z0++) {
				// If the DropType is the specified DropType...
				if (m_Settings.DropTypes[z0].Title == m_DropZone.DropType) {
					// Set specified DropType
					m_DropType = m_Settings.DropTypes[z0];
				}
			}
		}
		// Create the drop at the aircraft's current location
		m_SpawnLocation = GetPosition() + Vector(0, m_Settings.Aircraft.DropOffset, 0);
		// If RANDOM select random container
		string ContainerType = m_DropType.Container;
		if (ContainerType == TEXT_ITEM_RANDOM) {
			ContainerType = m_Containers.GetRandomElement();
		}
		else {
			ContainerType = m_DropType.Container;
		}
		// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
		// Create drop container ... AirdropContainer takes over from here
		AirdropContainerBase m_Container = AirdropContainerBase.Cast(GetGame().CreateObject(ContainerType, m_SpawnLocation, false, false, true));
		// If we have a Container
		if (m_Container) {
			// Set container orientation based on current position
			//m_Container.SetOrientation(GetOrientation() + Vector(-90, 0, 0));
			m_Container.InitializeContainer(m_Settings, m_DropType);
			// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
			// Send status messages
			AD_LogMessage("[EOnDrop] Drop spawned with " + m_DropZone.Zombies + " infected at " + m_SpawnLocation + ".");
			// Check drop title
			if (m_DropZone.Title == "") {
				// Notify admin of JSON file issue
				m_DropZone.Title = "'Unknown Location'";
				AD_LogMessage("[EOnDrop] Dropzone name was not set!");
			}
			// If this is a random drop...
			if (m_RandomDrop) {
				m_LogMessage = "Supplies have been dropped near " + m_DropZone.Title + ".";
				AirdropBase.ADU_SendClientMessages(AIRDROP_ICON_AIRDROP_DROP, m_LogMessage, m_Settings.Messages.Mode, m_Settings.Messages.Duration, m_MessageColor);
			}
			else {
				m_LogMessage = "Supplies have been dropped near the player's location.";
				AirdropBase.ADU_SendClientMessagesPrivate(AIRDROP_ICON_AIRDROP_DROP, m_LogMessage, m_Settings.Messages.Mode, m_Settings.Messages.Duration, m_MessageColor, m_Player);
			}
			AD_LogMessage("[EOnDrop] " + m_LogMessage);
		}
		else {
			AD_LogMessage("[EOnDrop] Spawn failed for Container [" + ContainerType + "] it does not exist in DayZ!");
		}
		//----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
	}
	//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	void EOnSpawnZombies()
	{
		// Get list of zombies for this lcoation
		ref TStringArray m_ZombieList;
		EntityAI droped_zomb;
		vector m_SpawnPoint;
		string m_SpawnItem;
		string LifespanSet;
		int LifespanSeconds = m_Settings.Container.Lifespan * m_SecsPerMin;
		int ct;
		// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
		// If lifespan is greater then zero (0)
		if (LifespanSeconds > 0) {
			// Set lifespan to object
			LifespanSet = LifespanSet.Format("Lifespan:  Set to %1 minutes.", m_Settings.Container.Lifespan);
		}
		else {
			// Set left to economy value
			LifespanSet = "Lifespan:  Set by economy (types.xml).";
		}
		// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
		// Increment Zombie Counter
		m_ZombieCount = m_ZombieCount + 1;
		// If we have more zombies to spawn?
		if (m_ZombieCount < m_DropZone.Zombies) {
			// If this location calls for random Zombies
			if (m_DropZone.Location == TEXT_ITEM_RANDOM) {
				// Spawn zombies randomly around drop location
				m_SpawnPoint = AirdropBase.ADU_RandomWithinCircle(m_DropPoint, 5, m_DropZone.Radius, true, 0.5);
				// Select a random location type
				ref AD_Location tmpLocation = m_Settings.Locations.GetRandomElement();
				// Select zombies list from random location 
				m_ZombieList = tmpLocation.Zombies;
				// Spawn random zombie around drop location
				m_SpawnItem = m_ZombieList.GetRandomElement();
				droped_zomb = EntityAI.Cast(GetGame().CreateObject(m_SpawnItem, m_SpawnPoint, false, true, true));
				// If we didn't spawn a zombie...
				if (!droped_zomb) {
					// Log spaw item failure
					AD_LogMessage("[EOnSpawnZombies] Spawn failed for zombie [" + m_ZombieCount + "] '" + m_SpawnItem + "', does not exist in DayZ!");
				}
				else {
					// If Lifespan is greater than zero...
					if (LifespanSeconds > 0) {
						// Set item lifespan
						droped_zomb.SetLifetime(LifespanSeconds);
					}
					// Log the item spawned
					AD_LogMessage("[EOnSpawnZombies] Zombie Spawned [" + m_ZombieCount + "] " + m_SpawnItem + " @ " + m_SpawnPoint + " ... " + LifespanSet);
				}
			}
			else {
				// Loop through the list of location types
				for (int ix = 0; ix < m_Settings.Locations.Count(); ix++) {
					// If the current location type matches the Dropzones location type
					if (m_Settings.Locations[ix].Title == m_DropZone.Location) {
						// Get list of zombies for specified location
						m_ZombieList = m_Settings.Locations[ix].Zombies;
						// If we have zombies to spawn
						if (m_ZombieList.Count() > 0) {
							// If the location count is zero (0) spawn entyre list
							if (m_DropZone.Zombies == 0) {
								// Select random point around drop location
								m_SpawnPoint = AirdropBase.ADU_RandomWithinCircle(m_DropPoint, 5, m_DropZone.Radius, true, 0.5);
								// Spawn zombie
								droped_zomb = EntityAI.Cast(GetGame().CreateObject(m_ZombieList[m_ZombieCount - 1], m_SpawnPoint, false, true, true));
								// If we didn't spawn a zombie...
								if (!droped_zomb) {
									// Log spaw item failure
									AD_LogMessage("[EOnSpawnZombies] Spawn failed for zombie [" + m_ZombieCount + "] '" + m_SpawnItem + "', does not exist in DayZ!");
								}
								else {
									// If Lifespan is greater than zero...
									if (LifespanSeconds > 0) {
										// Set item lifespan
										droped_zomb.SetLifetime(LifespanSeconds);
									}
									// Log the item spawned
									AD_LogMessage("[EOnSpawnZombies] Zombie Spawned [" + m_ZombieCount + "] " + m_SpawnItem + " @ " + m_SpawnPoint + " ... " + LifespanSet);
								}
								break;
							}
							else {
								// Select random point around drop location
								m_SpawnPoint = AirdropBase.ADU_RandomWithinCircle(m_DropPoint, 5, m_DropZone.Radius, true, 0.5);
								// Spawn zombie
								m_SpawnItem = m_ZombieList.GetRandomElement();
								droped_zomb = EntityAI.Cast(GetGame().CreateObject(m_SpawnItem, m_SpawnPoint, false, true, true));
								// If we didn't spawn a zombie...
								if (!droped_zomb) {
									// Log spaw item failure
									AD_LogMessage("[EOnSpawnZombies] Spawn failed for zombie [" + m_ZombieCount + "] '" + m_SpawnItem + "', it does not exist in DayZ!");
								}
								else {
									// If Lifespan is greater than zero...
									if (LifespanSeconds > 0) {
										// Set item lifespan
										droped_zomb.SetLifetime(LifespanSeconds);
									}
									// Log the item spawned
									AD_LogMessage("[EOnSpawnZombies] Zombie Spawned [" + m_ZombieCount + "] " + m_SpawnItem + " @ " + m_SpawnPoint + " ... " + LifespanSet);
								}
								break;
							}
						}
					}
				}
			}
		}
		else {
			// Remove timer
			m_SpawnTimer.Stop();
		}
	}
	//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	void EOnBeforeDelete(vector position)
	{
		/* not needed here (zedmag)
		if ( !GetGame().IsMultiplayer() || GetGame().IsClient() )
		{
			if ( m_ParticleEfx )
				m_ParticleEfx.Stop();
		}
		*/

		// If the aircraft is OFF off the map
		if (!AirdropBase.ADU_IsInRect(position))
		{
			// Destroy the aircraft
			AD_DestroyPlane(position);
		}
	}
	//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
}
