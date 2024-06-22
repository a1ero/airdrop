//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Handles container movement and initiates spawn of zombies & loot ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
class AirdropContainerBase : BuildingSuper //House		Building	//House		/HouseNoDestruct 
{
	// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
	const int m_SecsPerMin  = 60;		// Time - Seconds to minutes conversion
	const int m_Miliseconds = 1000;		// Time - Seconds to miliseconds conversion
	// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
	ref Timer m_Timer;					// Timer controlling movement of container
	ref AirdropSettings m_Settings;		// Reference to main Airdrop settings (passen in)
	ref AD_DropType m_DropType;			// Reference to the drop type speciified in AirdropPlane
	// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
	Weather m_Weather;					// Refernce to game weather
	vector m_WindVector;				// Current wind vector
	vector m_HitPosition;				// Position where object is hit
	AirdropParachuteBase Parachute;		// Parachute object
	// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
	protected Particle m_Smoke;		// Refernce to particle effect
	// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
	protected bool m_IsActive;			// Is the container active (true = yes, false = no {default})
	protected bool m_Loot;				// Is the loot dropped? (true = yes, false = no {default})
	protected bool m_Unstuck;			// Is the container unstuck/uprighted? (true = yes, false = no {default})
	protected bool m_Grounded;			// Is the container on the ground? (true = yes, false = no {default})
	protected bool m_Parachute;	        // Is the Parachute Open or Closed (true or false)
	// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
	FileHandle m_LogFile;				// Reference to log file
	string m_LogMessage;				// Message to be logged (temp)
	//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	void AirdropContainerBase()
	{
		// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
		// Activate simulation events
		SetEventMask(EntityEvent.CONTACT | EntityEvent.SIMULATE | EntityEvent.POSTSIMULATE);
		// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
		// Initialize log file
		m_LogFile = OpenFile(TEXT_PATH_LOGS_AIRDROP + AirdropBase.ADU_GetFileDateTimeStamp() + " AirdropContainer.log", FileMode.WRITE);
		ADU_LogMessage("[AirdropContainerBase] Container created.");
		// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
		// Register synced vars
		RegisterNetSyncVariableBool("m_IsActive");
		RegisterNetSyncVariableBool("m_Loot");
		RegisterNetSyncVariableBool("m_Unstuck");
		RegisterNetSyncVariableBool("m_Grounded");
		RegisterNetSyncVariableBool("m_Parachute");
		// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
		if (GetGame().IsClient() || !GetGame().IsMultiplayer()) 
		{
			// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
			// Initialize effects
			m_Smoke = Particle.PlayOnObject(ParticleList.ADU_AIRDROP_SMOKE1, this, GetMemoryPointPos("light"));
			ADU_LogMessage("[InitializeContainer] Airdrop Smoke 1 started.");
			// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
		}
	}
	//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	void ~AirdropContainerBase()
	{
		// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
		// If we have a smoke effect 1
		if (m_Smoke) 
		{
			// Close effect
			m_Smoke.Stop();
			ADU_LogMessage("[~AirdropContainerBase] Airdrop Smoke 1 stopped.");
		}
		// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
		// If we have timer
		if (m_Timer)
		{
			// Stop timer
			m_Timer.Stop();
			delete m_Timer;
			ADU_LogMessage("[~AirdropContainerBase] Timer stopped/removed.");
		}
		// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
		// Send closing message
		ADU_LogMessage("[~AirdropContainerBase] Log file closed.");
		ADU_LogMessage("[~AirdropContainerBase] Container Destroyed.");
		// Close file
		CloseFile(m_LogFile);
		// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
	}
	//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	void InitializeContainer(AirdropSettings settings, AD_DropType droptype)
	{
		// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
		// If we have settings and a drop type...
		if (settings && droptype) 
		{
			// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
			// Set Container parameters
			m_IsActive = true;
			m_Loot = false;
			m_Unstuck = false;
			m_Grounded = false;
			m_Parachute = true;
			// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
			m_Settings = settings;
			m_DropType = droptype;
			// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
			// Get current game weather	
			m_Weather = g_Game.GetWeather();
			ADU_LogMessage("[InitializeContainer] Winds: Vector " + m_Weather.GetWind() + ", Speed " + m_Weather.GetWindSpeed() + "mps, Strength " + m_Settings.Container.WindStrength);
			// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
			// Delete the container after a time set by airdrop interval 
			GetGame().GetCallQueue(CALL_CATEGORY_GAMEPLAY).CallLater(GetGame().ObjectDelete, m_Settings.Container.Lifespan * m_SecsPerMin * m_Miliseconds, false, this);
			// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
			// If we are on the server
			if (GetGame().IsServer())
			{
				// Create the Parachute
				Parachute = AirdropParachuteBase.Cast(GetGame().CreateObject("AirdropParachute", Vector(0, 0, 0), false, false, true));
				// If we have a parachute
				if (Parachute) 
				{
					// Attache parachute
					this.AddChild(IEntity.Cast(Parachute), -1, false);
					this.Update();
					// Initialize the parachute
					Parachute.ADU_InitializeParachute(m_Settings);
					m_Parachute = true;
					ADU_LogMessage("[InitializeContainer] Parachute created on @ " + Parachute.GetPosition());
				} 
				else 
				{
					// Let admin know the parachute didn't spawn
					ADU_LogMessage("[InitializeContainer] Parachute wsa NOT created!");
					m_Parachute = false;
				}
			}
			// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
			// If we are on the client side
			if (GetGame().IsClient() || !GetGame().IsMultiplayer()) 
			{
				bool is_playing;
				// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
				// If we have a particle effect and we do NOT want it...
				if (m_Smoke && m_Settings.Controls.SmokeTrails == 0) 
				{
					// Close effect
					m_Smoke.Stop();
					ADU_LogMessage("[InitializeContainer] Airdrop Smoke 1 stopped.");
				}
				// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
			}
			// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
			SetSynchDirty();
		}
		else 
		{
			// Log status
			AirdropBase.ADU_SendClientMessages(AIRDROP_ICON_AIRDROP_DROP, "[InitializeContainer] Container NOT Initialized.  You are NOT allowed to spawn in AirdropContainers!", 3, 30, ARGB(255, 128, 0, 0));
		}
	}
	//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	override void EOnSimulate(IEntity owner, float dt) {
		// If we are on the server
		if (GetGame().IsServer()) 
		{
			// If we are active and NOT on the ground... 
			if (m_IsActive) 
			{
				// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
				// If we are NOT yet on the ground
				if (!m_Grounded) 
				{
					// Get current position
					vector position = GetPosition();
					// Set the current winds aloft
					vector winddrift = (ADU_GetWindDrift() * m_Settings.Container.WindStrength);
					// Set velocities
					vector velocity  = Vector(winddrift[0], -m_Settings.Container.FallRate, winddrift[2]);
					// Create the transform
					vector transform[4];
					// Load the transform
					GetTransform(transform);
					// Update the transform
					transform[3] = position + (velocity * dt);
					// Move the object
					MoveInTime( transform, dt );
					// Calculate distance to ground
					float HAT = position[1] - GetGame().SurfaceY(position[0], position[2]);
					// If we are on the ground...
					if (HAT <= m_Settings.Container.TriggerAGL || ADU_IsObject()) 
					{
						m_Grounded = true;
						ADU_LogMessage("[AirdropContainerFall] Airdrop Container Touchdown @ " + GetPosition());
					}
				// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
				} 
				else 
				{
				// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
					// Remove parachute
					if (m_Parachute) {
						ADU_RemoveParachute();
					}
					// Reposition container
					if (!m_Unstuck) {
						ADU_RepositionContainer();
					}
					// Spawn loot
					if (!m_Loot) {
						ADU_SpawnItems();
					}
					// Deactivate container
					m_IsActive = false
					ADU_LogMessage("[EOnSimulate] Airdrop Container deactivated.");
					// Synchronize updated flags
					SetSynchDirty();
				}
				// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
			}
		}
	}
	//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	override private void EOnContact(IEntity other, Contact extra) {
		// Set grounded flag
		m_Grounded = true;
		ADU_LogMessage("[EOnContact] Airdrop Container Touchdown @ " + GetPosition());
	}
	//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	private vector ADU_GetWindDrift() 
	{
		// Get current game weather
		m_Weather = g_Game.GetWeather();
		// Get wind strength vectors
		m_WindVector = m_Weather.GetWind() * (m_Settings.Container.WindStrength) * 0.1;
		// Return wind vectors
		return m_WindVector;
	}
	//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	private void ADU_RemoveParachute() 
	{
		// Detach the parachute from the container
		this.RemoveChild(Parachute);
		// Destroy the parachute
		//Parachute.GetGame().GetCallQueue(CALL_CATEGORY_SYSTEM).CallLater(GetGame().ObjectDelete, 0, false, this);
		GetGame().ObjectDelete(Parachute);
		// Cleanup
		m_Parachute = false;
		ADU_LogMessage("[ADU_RemoveParachute] Parachute removed.");
	}
	//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	private void ADU_RepositionContainer() 
	{
		// If we are not on an object then place the container on the ground
		if (!ADU_IsObject())
		{
			this.PlaceOnSurface();
		} 
		else
		{	
			this.SetPosition(m_HitPosition);
		}
		// Cleanup
		m_Unstuck = true;
		ADU_LogMessage("[ADU_RepositionContainer] Container repositioned.");
	}
	//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	private bool ADU_IsObject()
	{
		// Need to return the object hit and determine if it is something we can land on.
		set<Object> objects = new set<Object>;
		objects.Clear();
		vector m_Start = this.GetPosition();
		vector m_End = this.GetPosition() - Vector(0, m_Settings.Container.TriggerAGL, 0);
		vector m_Hit;
		vector m_HitPos;
		int m_HitIndex;
		DayZPhysics.RaycastRV(m_Start, m_End, m_HitPos, m_Hit, m_HitIndex, objects, NULL, this);
		m_HitPosition = m_HitPos;
		return objects.Count() != 0;
	}
	//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	private void ADU_SpawnItems() {
		// Store current drop location & object
		vector drop_point = GetPosition();
		vector m_SpawnPoint;
		ItemBase dropped_item;
		string item;
		// Set lifespan text
		string LifespanSet;
		int LifespanSeconds = m_DropType.Lifespan * m_SecsPerMin;
		int ct;
		if (LifespanSeconds > 0) 
		{
			LifespanSet = LifespanSet.Format("Lifespan:  Set to %1 minutes.", m_DropType.Lifespan);
		}
		else 
		{
			LifespanSet = "Lifespan:  Set by economy.";
		}
		// If there is loot to be spawned?
		if (m_DropType.Items.Count() > 0) 
		{
			// If we are spawning ALL items
			if (m_DropType.Quantity == 0) 
			{
				// Log spaw info
				ADU_LogMessage("[ADU_SpawnItems] Drop Type '" + m_DropType.Title + "', Container: '" + m_DropType.Container + "', Mode/Quantity/Iems: All/" + m_DropType.Quantity + "/" + m_DropType.Items.Count() + ", Flare:  " + m_DropType.AddFlare + "%");
				ADU_LogMessage("------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------");
				// For each item in the list
				for (int i1 = 0; i1 < m_DropType.Items.Count(); i1++)
				{
					// Calculate random point in vacinity
					m_SpawnPoint = AirdropBase.ADU_RandomWithinCircle(drop_point, m_DropType.SpawnMin, m_DropType.SpawnMax, true, m_DropType.SpawnOffset);
					// Spawn random item at point
					item = m_DropType.Items[i1];
					// Spawn Item
					dropped_item = ItemBase.Cast(GetGame().CreateObject(item, m_SpawnPoint, false, false, true));
					// If we didn't spawn an item...
					ct = i1 + 1;
					if (!dropped_item) 
					{
						// Log spaw item failure
						ADU_LogMessage("[ADU_SpawnItems] Spawn failed for item [" + ct + "] '" + item + "', it does not exist in DayZ!");
					}
					else 
					{
						// If Lifespan is greater than zero...
						if (LifespanSeconds > 0) 
						{
							// Set item lifespan
							dropped_item.SetLifetime(LifespanSeconds);
						}
						// Log the item spawned
						ADU_LogMessage("[ADU_SpawnItems] Loot Spawned [" + ct + "] " + item + " @ " + m_SpawnPoint + " ... " + LifespanSet);
					}
				}
			}
			else 
			{
				// Log spaw info
				ADU_LogMessage("[ADU_SpawnItems] Drop Type '" + m_DropType.Title + "', Container: '" + m_DropType.Container + "', Mode/Quantity/Iems: Random/" + m_DropType.Quantity + "/" + m_DropType.Items.Count() + ", Flare:  " + m_DropType.AddFlare + "%");
				ADU_LogMessage("------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------");
				// For each item to be spawned
				for (int i2 = 1; i2 <= m_DropType.Quantity; i2++)
				{
					// Calculate random point within prop point vacinity
					m_SpawnPoint = AirdropBase.ADU_RandomWithinCircle(drop_point, m_DropType.SpawnMin, m_DropType.SpawnMax, true, m_DropType.SpawnOffset);
					// Spawn random item at point
					item = m_DropType.Items.GetRandomElement();
					// Spawn Item
					dropped_item = ItemBase.Cast(GetGame().CreateObject(item, m_SpawnPoint, false, false, true));
					// If we didn't spawn an item...
					if (!dropped_item) 
					{
						// Log spaw item failure
						ADU_LogMessage("[ADU_SpawnItems] Spawn failed for item [" + i2 + "] '" + item + "', does not exist in DayZ!");
					}
					else 
					{
						// If Lifespan is greater than zero...
						if (LifespanSeconds > 0) 
						{
							// Set item lifespan
							dropped_item.SetLifetime(LifespanSeconds);
						}
						// Log the item spawned
						ADU_LogMessage("[ADU_SpawnItems] Loot Spawned [" + i2 + "] " + item + " @ " + m_SpawnPoint + " ... " + LifespanSet);
					}
				}
			}
		}
		else 
		{
			ADU_LogMessage("[ADU_SpawnItems] Drop Type Error '" + m_DropType.Title + "', Container: '" + m_DropType.Container + "', There are no items in the list to spawn!");
		}
		// Check if we need to add an Airdrop flare
		if (m_DropType.AddFlare != 0) {
			// If add flase is 100 or less thatn a random number between 0 and 100
			if (m_DropType.AddFlare == 100 || Math.RandomFloat(0, 100) <= m_DropType.AddFlare) 
			{
				// Calculate random point within prop point vacinity
				m_SpawnPoint = AirdropBase.ADU_RandomWithinCircle(drop_point, m_DropType.SpawnMin, m_DropType.SpawnMax, true, m_DropType.SpawnOffset);
				// Spawn AirdropFlare
				dropped_item = ItemBase.Cast(GetGame().CreateObject(TEXT_ITEM_AIRDROPFLARE, m_SpawnPoint, false, false, true));
				// If we didn't spawn an item...
				if (!dropped_item) 
				{
					// Log spaw item failure
					ADU_LogMessage("[ADU_SpawnItems] Spawn failed for item '" + TEXT_ITEM_AIRDROPFLARE + "', does not exist in DayZ!");
				}
				else 
				{
					// If Lifespan is greater than zero...
					if (LifespanSeconds > 0) 
					{
						// Set item lifespan
						dropped_item.SetLifetime(LifespanSeconds);
					}
					// Log the item spawned
					ADU_LogMessage("[ADU_SpawnItems] Airdrop Flare Spawned @ " + m_SpawnPoint + " ... " + LifespanSet);
				}
			}
		}
		ADU_LogMessage("------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------");
		// Cleanup
		m_Loot = true
	}
	//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	private void ADU_LogMessage(string message) 
	{
		// If we have settings...
		if (m_Settings) {
			// If we have an open log file... 
			if (m_LogFile && m_Settings.Controls.AD_LogContainer != 0) {
				// Write message
				FPrintln(m_LogFile, AirdropBase.ADU_GetFileDateTimeStamp(true) + ":  " + message);
			}
		}
	}
	//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	override bool IsInventoryVisible()
	{
		return true;
	}
	//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	override bool CanUseConstruction()
	{
		return false;
	}
	//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	override bool CanPutIntoHands(EntityAI parent)
	{
		return false;
	}
	//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	override bool CanPutInCargo(EntityAI parent)
	{
		return false;
	}
	//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
}
